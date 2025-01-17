# Copyright 2023 The ChromiumOS Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Configure-time checks for the led-policy node."""

import logging
import sys
from typing import List, Optional

from scripts import util


def check_policy(charge_state, charge_port, chipset_state, batt_lvl, policies):
    """Checks if a given state has valid policy coverage

    Args:
        charge_state: charge state to be tested
        charge_port: charge port to be tested
        chipset_state: chipset state to be tested
        batt_lvl: battery level to be tested

    Return:
        A boolean: True if state is in the LED policies, false if not.
    """
    for node in policies.children.values():
        # check_policy is a replica of the match_node() function in led.c
        if "charge-state" in node.props:
            if charge_state != node.props["charge-state"].val:
                continue

            if "charge-port" in node.props:
                if charge_port != node.props["charge-port"].val:
                    continue

        if "chipset-state" in node.props:
            if chipset_state != node.props["chipset-state"].val:
                continue

        if "batt-lvl" in node.props:
            if (
                batt_lvl < node.props["batt-lvl"].val[0]
                or batt_lvl > node.props["batt-lvl"].val[1]
            ):
                continue

        return True

    # None of the devicetree nodes match this combination of states
    return False


def log_state(
    project_name, charge_state, charge_port, chipset_state, batt_min, batt_range
):
    """Checks if current state is missing coverage and logs error if it is

    Args:
        project_name: A string containing the project/test name
        charge_state: current charge state being tested
        charge_port: current charge port being tested
        chipset_state: current chipset state being tested
        batt_min: lower bound of battery level range being tested
        batt_range: range of battery level that is missing coverage, 0 for state
                    is covered

    Returns:
        A boolean: true if error is logged, false if not
    """

    if batt_range == 0:
        return False
    if batt_range == 101:
        logging.error(
            "%s: No LED policy found for %s%s%s",
            project_name,
            (charge_state + ", ") if charge_state else "",
            f"port {charge_port}, " if charge_port is not None else "",
            (chipset_state + ", ") if chipset_state else "",
        )
    else:
        logging.error(
            "%s: No LED policy for battery range %i%% to %i%% for %s%s%s",
            project_name,
            batt_min,
            batt_min + batt_range - 1,
            (charge_state + ", ") if charge_state else "",
            f"port {charge_port}, " if charge_port is not None else "",
            (chipset_state + ", ") if chipset_state else "",
        )
    return True


def iterate_power_states(edt, project_name):
    """Iterate all combinations of states and test the led policy coverage.

    Args:
        edt: EDT object representation of a devicetree

    Returns:
        num_errors: Number of missing policies detected.
    """
    led_policy_nodes = edt.compat2okay["cros-ec,led-policy"]

    if len(led_policy_nodes) != 1:
        return 0
    policies = led_policy_nodes[0]

    charge_state_list = [None]

    charge_port_list = [None]

    chipset_state_list = [None]

    # no Zephyr project currently uses batt_state to determine LED policy

    # Try to figure out which states this LED policy scheme doesn't care about
    # to minimize the number of error messages.
    care_about = {
        "charge-state": False,
        "charge-port": False,
        "chipset-state": False,
        "batt-lvl": False,
    }
    for node in policies.children.values():
        if "charge-state" in node.props:
            care_about["charge-state"] = True
            if "charge-port" in node.props:
                care_about["charge-port"] = True
        if "chipset-state" in node.props:
            care_about["chipset-state"] = True
        if "batt-lvl" in node.props:
            care_about["batt-lvl"] = True

    if care_about["charge-state"]:
        charge_state_list = [
            "LED_PWRS_CHARGE",
            "LED_PWRS_DISCHARGE",
            "LED_PWRS_ERROR",
            "LED_PWRS_IDLE",
            "LED_PWRS_FORCED_IDLE",
            "LED_PWRS_CHARGE_NEAR_FULL",
        ]

    if care_about["charge-port"]:
        usbc_nodes = edt.compat2okay["named-usbc-port"]
        if len(usbc_nodes) == 0:
            logging.error(
                "LED policy found for charge ports \
                but board has no named usbc ports."
            )
            return 1
        charge_port_list = range(len(usbc_nodes))

    if care_about["chipset-state"]:
        chipset_state_list = [
            "POWER_S0",
            "POWER_S3",
            "POWER_S5",
        ]

    num_errors = 0
    for charge_state in charge_state_list:
        for charge_port in charge_port_list:
            for chipset_state in chipset_state_list:
                no_policy_batt_lvl_min = 0
                no_policy_batt_range = 0
                for batt_lvl in range(101):
                    if check_policy(
                        charge_state,
                        charge_port,
                        chipset_state,
                        batt_lvl,
                        policies,
                    ):
                        num_errors += log_state(
                            project_name,
                            charge_state,
                            charge_port,
                            chipset_state,
                            no_policy_batt_lvl_min,
                            no_policy_batt_range,
                        )
                        no_policy_batt_range = 0
                        no_policy_batt_lvl_min = batt_lvl + 1
                    else:
                        no_policy_batt_range += 1

                num_errors += log_state(
                    project_name,
                    charge_state,
                    charge_port,
                    chipset_state,
                    no_policy_batt_lvl_min,
                    no_policy_batt_range,
                )
    return num_errors


def parse_args(argv: Optional[List[str]] = None):
    """Returns parsed command-line arguments"""
    parser = util.EdtArgumentParser(
        prog="led_policy_check",
        description="Zephyr EC specific devicetree checks",
    )

    return parser.parse_args(argv)


def main(argv: Optional[List[str]] = None) -> Optional[int]:
    """The main function.

    Args:
        argv: Optionally, the command-line to parse, not including argv[0].

    Returns:
        Zero upon success, or non-zero upon failure.
    """
    args = parse_args(argv)

    log_format = "%(levelname)s: %(message)s"

    logging.basicConfig(format=log_format, level=args.log_level)

    edtlib, edt, project_dir = util.load_edt(args.zephyr_base, args.edt_pickle)

    if edtlib is None:
        return 0

    # not all tests will want to test a fully covered policy
    if util.is_test(args.edt_pickle):
        return 0

    if iterate_power_states(edt, project_dir.name):
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
