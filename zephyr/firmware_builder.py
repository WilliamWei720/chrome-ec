#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright 2021 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Build and test all of the Zephyr boards.

This is the entry point for the custom firmware builder workflow recipe.
"""

import argparse
import multiprocessing
import pathlib
import re
import shlex
import subprocess
import sys

import zmake.project
from chromite.api.gen_sdk.chromite.api import firmware_pb2
from google.protobuf import json_format  # pylint: disable=import-error

DEFAULT_BUNDLE_DIRECTORY = "/tmp/artifact_bundles"
DEFAULT_BUNDLE_METADATA_FILE = "/tmp/artifact_bundle_metadata"

# Boards that we want to track the coverage of our own files specifically.
SPECIAL_BOARDS = ["herobrine"]


def run_twister(platform_ec, code_coverage=False, extra_args=None):
    """Build the tests using twister."""
    cmd = [
        platform_ec / "twister",
        "--outdir",
        platform_ec / "twister-out",
        "-v",
        "-i",
        "-p",
        "native_posix",
        "-p",
        "unit_testing",
    ]

    if extra_args:
        cmd.extend(extra_args)

    if code_coverage:
        # Tell Twister to collect coverage data. We must specify an explicit platform
        # type in this case, as well.
        cmd.extend(
            [
                "--coverage",
            ]
        )
    print(" ".join(shlex.quote(str(x)) for x in cmd))
    subprocess.run(cmd, check=True, cwd=platform_ec, stdin=subprocess.DEVNULL)


def build(opts):
    """Builds all Zephyr firmware targets"""
    metric_list = firmware_pb2.FwBuildMetricList()

    zephyr_dir = pathlib.Path(__file__).parent.resolve()
    platform_ec = zephyr_dir.parent
    subprocess.run(
        [platform_ec / "util" / "check_clang_format.py"],
        check=True,
        cwd=platform_ec,
        stdin=subprocess.DEVNULL,
    )

    cmd = ["zmake", "-D", "build", "-a"]
    if opts.code_coverage:
        cmd.append("--coverage")
    print(" ".join(shlex.quote(str(x)) for x in cmd))
    subprocess.run(cmd, cwd=zephyr_dir, check=True, stdin=subprocess.DEVNULL)
    if not opts.code_coverage:
        for project in zmake.project.find_projects(zephyr_dir).values():
            if project.config.is_test:
                continue
            build_dir = (
                platform_ec / "build" / "zephyr" / project.config.project_name
            )
            metric = metric_list.value.add()
            metric.target_name = project.config.project_name
            metric.platform_name = project.config.zephyr_board
            for (variant, _) in project.iter_builds():
                build_log = build_dir / f"build-{variant}" / "build.log"
                parse_buildlog(build_log, metric, variant.upper())
    with open(opts.metrics, "w") as file:
        file.write(json_format.MessageToJson(metric_list))

    run_twister(platform_ec, opts.code_coverage, ["--build-only"])


UNITS = {
    "B": 1,
    "KB": 1024,
    "MB": 1024 * 1024,
    "GB": 1024 * 1024 * 1024,
}


def parse_buildlog(filename, metric, variant):
    """Parse the build.log generated by zmake to find the size of the image."""
    with open(filename, "r") as infile:
        # Skip over all lines until the memory report is found
        while True:
            line = infile.readline()
            if not line:
                return
            if line.startswith("Memory region"):
                break

        for line in infile.readlines():
            # Skip any lines that are not part of the report
            if not line.startswith(" "):
                continue
            parts = line.split()
            fw_section = metric.fw_section.add()
            fw_section.region = variant + "_" + parts[0][:-1]
            fw_section.used = int(parts[1]) * UNITS[parts[2]]
            fw_section.total = int(parts[3]) * UNITS[parts[4]]
            fw_section.track_on_gerrit = False


def bundle(opts):
    """Bundle the artifacts for either firmware or code coverage."""
    if opts.code_coverage:
        bundle_coverage(opts)
    else:
        bundle_firmware(opts)


def get_bundle_dir(opts):
    """Get the directory for the bundle from opts or use the default.

    Also create the directory if it doesn't exist.
    """
    if opts.output_dir:
        bundle_dir = opts.output_dir
    else:
        bundle_dir = DEFAULT_BUNDLE_DIRECTORY
    bundle_dir = pathlib.Path(bundle_dir)
    if not bundle_dir.is_dir():
        bundle_dir.mkdir()
    return bundle_dir


def write_metadata(opts, info):
    """Write the metadata about the bundle."""
    bundle_metadata_file = (
        opts.metadata if opts.metadata else DEFAULT_BUNDLE_METADATA_FILE
    )
    with open(bundle_metadata_file, "w") as file:
        file.write(json_format.MessageToJson(info))


def bundle_coverage(opts):
    """Bundles the artifacts from code coverage into its own tarball."""
    info = firmware_pb2.FirmwareArtifactInfo()
    info.bcs_version_info.version_string = opts.bcs_version
    bundle_dir = get_bundle_dir(opts)
    zephyr_dir = pathlib.Path(__file__).parent.resolve()
    platform_ec = zephyr_dir.parent
    build_dir = platform_ec / "build" / "zephyr"
    tarball_name = "coverage.tbz2"
    tarball_path = bundle_dir / tarball_name
    cmd = ["tar", "cvfj", tarball_path, "lcov.info"]
    print(" ".join(shlex.quote(str(x)) for x in cmd))
    subprocess.run(cmd, cwd=build_dir, check=True, stdin=subprocess.DEVNULL)
    meta = info.objects.add()
    meta.file_name = tarball_name
    meta.lcov_info.type = (
        firmware_pb2.FirmwareArtifactInfo.LcovTarballInfo.LcovType.LCOV
    )
    (bundle_dir / "html").mkdir(exist_ok=True)
    cmd = ["mv", "lcov_rpt", bundle_dir / "html/lcov_rpt"]
    print(" ".join(shlex.quote(str(x)) for x in cmd))
    subprocess.run(cmd, cwd=build_dir, check=True, stdin=subprocess.DEVNULL)
    meta = info.objects.add()
    meta.file_name = "html"
    meta.coverage_html.SetInParent()

    write_metadata(opts, info)


def bundle_firmware(opts):
    """Bundles the artifacts from each target into its own tarball."""
    info = firmware_pb2.FirmwareArtifactInfo()
    info.bcs_version_info.version_string = opts.bcs_version
    bundle_dir = get_bundle_dir(opts)
    zephyr_dir = pathlib.Path(__file__).parent.resolve()
    platform_ec = zephyr_dir.parent
    for project in zmake.project.find_projects(zephyr_dir).values():
        if project.config.is_test:
            continue
        build_dir = (
            platform_ec / "build" / "zephyr" / project.config.project_name
        )
        artifacts_dir = build_dir / "output"
        tarball_name = f"{project.config.project_name}.firmware.tbz2"
        tarball_path = bundle_dir.joinpath(tarball_name)
        cmd = ["tar", "cvfj", tarball_path, "."]
        print(" ".join(shlex.quote(str(x)) for x in cmd))
        subprocess.run(
            cmd, cwd=artifacts_dir, check=True, stdin=subprocess.DEVNULL
        )
        meta = info.objects.add()
        meta.file_name = tarball_name
        meta.tarball_info.type = (
            firmware_pb2.FirmwareArtifactInfo.TarballInfo.FirmwareType.EC
        )
        # TODO(kmshelton): Populate the rest of metadata contents as it
        # gets defined in infra/proto/src/chromite/api/firmware.proto.

    write_metadata(opts, info)


def test(opts):
    """Runs all of the unit tests for Zephyr firmware"""
    metrics = firmware_pb2.FwTestMetricList()

    zephyr_dir = pathlib.Path(__file__).parent.resolve()

    # Run zmake tests to ensure we have a fully working zmake before
    # proceeding.
    subprocess.run(
        [zephyr_dir / "zmake" / "run_tests.sh"],
        check=True,
        cwd=zephyr_dir,
        stdin=subprocess.DEVNULL,
    )

    # Twister-based tests
    platform_ec = zephyr_dir.parent
    third_party = platform_ec.parent.parent / "third_party"
    run_twister(platform_ec, opts.code_coverage, ["--test-only"])

    if opts.code_coverage:
        build_dir = platform_ec / "build" / "zephyr"
        # Merge lcov files here because bundle failures are "infra" failures.
        output = subprocess.run(
            [
                "/usr/bin/lcov",
                "--summary",
                platform_ec / "twister-out" / "coverage.info",
            ],
            cwd=zephyr_dir,
            check=True,
            stdout=subprocess.PIPE,
            universal_newlines=True,
            stdin=subprocess.DEVNULL,
        ).stdout
        _extract_lcov_summary("EC_ZEPHYR_TESTS", metrics, output)

        cmd = ["make", "test-coverage", f"-j{opts.cpus}"]
        print(" ".join(shlex.quote(str(x)) for x in cmd))
        subprocess.run(
            cmd, cwd=platform_ec, check=True, stdin=subprocess.DEVNULL
        )

        output = subprocess.run(
            [
                "/usr/bin/lcov",
                "--summary",
                platform_ec / "build/coverage/lcov.info",
            ],
            cwd=zephyr_dir,
            check=True,
            stdout=subprocess.PIPE,
            universal_newlines=True,
            stdin=subprocess.DEVNULL,
        ).stdout
        _extract_lcov_summary("EC_LEGACY_TESTS", metrics, output)

        cmd = [
            "/usr/bin/lcov",
            "-o",
            build_dir / "all_tests.info",
            "--rc",
            "lcov_branch_coverage=1",
            "-a",
            platform_ec / "build/coverage/lcov.info",
            "-a",
            platform_ec / "twister-out" / "coverage.info",
        ]
        print(" ".join(shlex.quote(str(x)) for x in cmd))
        output = subprocess.run(
            cmd,
            cwd=zephyr_dir,
            check=True,
            stdout=subprocess.PIPE,
            universal_newlines=True,
            stdin=subprocess.DEVNULL,
        ).stdout
        _extract_lcov_summary("ALL_TESTS", metrics, output)

        cmd = [
            "/usr/bin/lcov",
            "-o",
            build_dir / "zephyr_merged.info",
            "--rc",
            "lcov_branch_coverage=1",
            "-a",
            build_dir / "all_builds.info",
            "-a",
            build_dir / "all_tests.info",
        ]
        print(" ".join(shlex.quote(str(x)) for x in cmd))
        output = subprocess.run(
            cmd,
            cwd=zephyr_dir,
            check=True,
            stdout=subprocess.PIPE,
            universal_newlines=True,
            stdin=subprocess.DEVNULL,
        ).stdout
        _extract_lcov_summary("EC_ZEPHYR_MERGED", metrics, output)

        cmd = [
            "/usr/bin/lcov",
            "-o",
            build_dir / "lcov_unfiltered.info",
            "--rc",
            "lcov_branch_coverage=1",
            "-a",
            build_dir / "zephyr_merged.info",
            "-a",
            platform_ec / "build/coverage/lcov.info",
        ]
        print(" ".join(shlex.quote(str(x)) for x in cmd))
        subprocess.run(
            cmd,
            cwd=zephyr_dir,
            check=True,
            stdin=subprocess.DEVNULL,
        )

        cmd = [
            "/usr/bin/lcov",
            "-o",
            build_dir / "lcov.info",
            "--rc",
            "lcov_branch_coverage=1",
            "-r",
            build_dir / "lcov_unfiltered.info",
            platform_ec / "build/**",
            platform_ec / "twister-out*/**",
            "/usr/include/**",
            "/usr/lib/**",
        ]
        print(" ".join(shlex.quote(str(x)) for x in cmd))
        output = subprocess.run(
            cmd,
            cwd=zephyr_dir,
            check=True,
            stdout=subprocess.PIPE,
            universal_newlines=True,
            stdin=subprocess.DEVNULL,
        ).stdout
        _extract_lcov_summary("ALL_MERGED", metrics, output)

        # Create an info file without any test code, just for the metric.
        cmd = [
            "/usr/bin/lcov",
            "-o",
            build_dir / "lcov_no_tests.info",
            "--rc",
            "lcov_branch_coverage=1",
            "-r",
            build_dir / "lcov.info",
            platform_ec / "test/**",
            zephyr_dir / "test/**",
            zephyr_dir / "emul/**",
            third_party / "zephyr/main/subsys/emul/**",
            third_party / "zephyr/main/subsys/testsuite/**",
        ]
        print(" ".join(shlex.quote(str(x)) for x in cmd))
        output = subprocess.run(
            cmd,
            cwd=zephyr_dir,
            check=True,
            stdout=subprocess.PIPE,
            universal_newlines=True,
            stdin=subprocess.DEVNULL,
        ).stdout
        _extract_lcov_summary("ALL_FILTERED", metrics, output)

        subprocess.run(
            [
                "/usr/bin/genhtml",
                "--branch-coverage",
                "-q",
                "-o",
                build_dir / "lcov_rpt",
                "-t",
                "All boards and tests merged",
                "-s",
                build_dir / "lcov.info",
            ],
            cwd=zephyr_dir,
            check=True,
            stdin=subprocess.DEVNULL,
        )

        for board in SPECIAL_BOARDS:
            # Merge board coverage with tests
            cmd = [
                "/usr/bin/lcov",
                "-o",
                build_dir / (board + "_merged.info"),
                "--rc",
                "lcov_branch_coverage=1",
                "-a",
                build_dir / "all_tests.info",
                "-a",
                build_dir / board / "output/zephyr.info",
            ]
            print(" ".join(shlex.quote(str(x)) for x in cmd))
            subprocess.run(
                cmd,
                cwd=zephyr_dir,
                check=True,
                stdin=subprocess.DEVNULL,
            )
            # Exclude file patterns we don't want
            cmd = [
                "/usr/bin/lcov",
                "-o",
                build_dir / (board + "_filtered.info"),
                "--rc",
                "lcov_branch_coverage=1",
                "-r",
                build_dir / (board + "_merged.info"),
                # Exclude generated files
                platform_ec / "build/**",
                platform_ec / "twister-out*/**",
                # Exclude system headers
                "/usr/include/**",
                # Exclude third_party code (specifically zephyr)
                third_party / "**",
                # These are questionable, but they are essentically untestable
                zephyr_dir / "drivers/**",
                zephyr_dir / "include/drivers/**",
                zephyr_dir / "projects/**",
                zephyr_dir / "shim/chip/**",
                zephyr_dir / "shim/chip/npcx/npcx_monitor/**",
                zephyr_dir / "shim/core/**",
                # Exclude tests
                platform_ec / "test/**",
                zephyr_dir / "emul/**",
                zephyr_dir / "test/**",
            ]
            print(" ".join(shlex.quote(str(x)) for x in cmd))
            subprocess.run(
                cmd,
                cwd=zephyr_dir,
                check=True,
                stdin=subprocess.DEVNULL,
            )
            # Then keep only files present in the board build
            filenames = set()
            with open(
                build_dir / board / "output/zephyr.info", "r"
            ) as board_cov:
                for line in board_cov.readlines():
                    if line.startswith("SF:"):
                        filenames.add(line[3:-1])
            cmd = [
                "/usr/bin/lcov",
                "-o",
                build_dir / (board + "_final.info"),
                "--rc",
                "lcov_branch_coverage=1",
                "-e",
                build_dir / (board + "_filtered.info"),
            ] + list(filenames)
            print(" ".join(shlex.quote(str(x)) for x in cmd))
            output = subprocess.run(
                cmd,
                cwd=zephyr_dir,
                check=True,
                stdout=subprocess.PIPE,
                universal_newlines=True,
                stdin=subprocess.DEVNULL,
            ).stdout
            _extract_lcov_summary(f"BOARD_{board}".upper(), metrics, output)

    with open(opts.metrics, "w") as file:
        file.write(json_format.MessageToJson(metrics))  # type: ignore


COVERAGE_RE = re.compile(
    r"lines\.*: *([0-9\.]+)% \(([0-9]+) of ([0-9]+) lines\)"
)


def _extract_lcov_summary(name, metrics, output):
    re_match = COVERAGE_RE.search(output)
    if re_match:
        metric = metrics.value.add()
        metric.name = name
        metric.coverage_percent = float(re_match.group(1))
        metric.covered_lines = int(re_match.group(2))
        metric.total_lines = int(re_match.group(3))


def main(args):
    """Builds and tests all of the Zephyr targets and reports build metrics"""
    opts = parse_args(args)

    if not hasattr(opts, "func"):
        print("Must select a valid sub command!")
        return -1

    # Run selected sub command function
    opts.func(opts)
    return 0


def parse_args(args):
    """Parse command line args."""
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument(
        "--cpus",
        default=multiprocessing.cpu_count(),
        help="The number of cores to use.",
    )

    parser.add_argument(
        "--metrics",
        dest="metrics",
        required=True,
        help="File to write the json-encoded MetricsList proto message.",
    )

    parser.add_argument(
        "--metadata",
        required=False,
        help=(
            "Full pathname for the file in which to write build artifact "
            "metadata."
        ),
    )

    parser.add_argument(
        "--output-dir",
        required=False,
        help=(
            "Full pathname for the directory in which to bundle build "
            "artifacts."
        ),
    )

    parser.add_argument(
        "--code-coverage",
        required=False,
        action="store_true",
        help="Build host-based unit tests for code coverage.",
    )

    parser.add_argument(
        "--bcs-version",
        dest="bcs_version",
        default="",
        required=False,
        # TODO(b/180008931): make this required=True.
        help="BCS version to include in metadata.",
    )

    # Would make this required=True, but not available until 3.7
    sub_cmds = parser.add_subparsers()

    build_cmd = sub_cmds.add_parser("build", help="Builds all firmware targets")
    build_cmd.set_defaults(func=build)

    build_cmd = sub_cmds.add_parser(
        "bundle",
        help="Creates a tarball containing build "
        "artifacts from all firmware targets",
    )
    build_cmd.set_defaults(func=bundle)

    test_cmd = sub_cmds.add_parser("test", help="Runs all firmware unit tests")
    test_cmd.set_defaults(func=test)

    return parser.parse_args(args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
