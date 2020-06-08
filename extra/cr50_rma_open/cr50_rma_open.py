#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright 2018 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Used to access the cr50 console and handle RMA Open
"""Open cr50 using RMA authentication.

Run RMA Open to enable CCD on Cr50. The utility can be used to get a
url that will generate an authcode to open cr50. It can also be used to
try opening cr50 with the generated authcode.

The last challenge is the only valid one, so don't generate a challenge
10 times and then use the first URL. You can only use the last one.

For RMA Open:
Connect suzyq to the dut and your workstation.

Check the basic setup with
    sudo python cr50_rma_open.py -c

If the setup is broken. Follow the debug print statements to try to fix
the error. Rerun until the script says Cr50 setup ok.

After the setup is verified, run the following command to generate the
challenge url
    sudo python cr50_rma_open.py -g -i $HWID

Go to the URL from by that command to generate an authcode. Once you have
the authcode, you can use it to open cr50.
    sudo python cr50_rma_open.py -a $AUTHCODE

If for some reason hardware write protect doesn't get disabled during rma
open or gets enabled at some point the script can be used to disable
write protect.
    sudo python cr50_rma_open.py -w

When prepping devices for the testlab, you need to enable testlab mode.
Prod cr50 images can't enable testlab mode. If the device is running a
prod image, you can skip this step.
    sudo python cr50_rma_open.py -t
"""

import argparse
import glob
import logging
import re
import subprocess
import sys
import time

import serial

SCRIPT_VERSION = 5
CCD_IS_UNRESTRICTED = 1 << 0
WP_IS_DISABLED = 1 << 1
TESTLAB_IS_ENABLED = 1 << 2
RMA_OPENED = CCD_IS_UNRESTRICTED | WP_IS_DISABLED
URL = ('https://www.google.com/chromeos/partner/console/cr50reset?'
       'challenge=%s&hwid=%s')
RMA_SUPPORT_PROD = '0.3.3'
RMA_SUPPORT_PREPVT = '0.4.5'
DEV_MODE_OPEN_PROD = '0.3.9'
DEV_MODE_OPEN_PREPVT = '0.4.7'
TESTLAB_PROD = '0.3.10'
CR50_USB = '18d1:5014'
CR50_LSUSB_CMD = ['lsusb', '-vd', CR50_USB]
ERASED_BID = 'ffffffff'

DEBUG_MISSING_USB = """
Unable to find Cr50 Device 18d1:5014

DEBUG MISSING USB:
    - Make sure suzyq is plugged into the correct DUT port
    - Try flipping the cable
    - unplug the cable for 5s then plug it back in
"""

DEBUG_DEVICE = """
DEBUG DEVICE COMMUNICATION:
Issues communicating with %s

A 18d1:5014 device exists, so make sure you have selected the correct
/dev/ttyUSB
"""

DEBUG_SERIALNAME = """
DEBUG SERIALNAME:
Found the USB device, but can't match the usb serialname. Check the
serialname you passed into cr50_rma_open or try running without a
serialname.
"""

DEBUG_CONNECTION = """
DEBUG CONNECTION:
Found the USB device but cant communicate with any of the consoles.

Try Running cr50_rma_open again. If it still fails unplug the ccd cable
for 5 seconds and plug it back in.
"""

DEBUG_TOO_MANY_USB_DEVICES = """
DEBUG SELECT USB:
More than one cr50 usb device was found. Disconnect all but one device
or use the -s option with the correct usb serialname.
"""

DEBUG_ERASED_BOARD_ID = """
DEBUG ERASED BOARD ID:
If you are using a prePVT device run
/usr/share/cros/cr50-set-board-id.sh proto

If you are running a MP device, please talk to someone.
"""

DEBUG_AUTHCODE_MISMATCH = """
DEBUG AUTHCODE MISMATCH:
    - Check the URL matches the one generated by the last cr50_rma_open
      run.
    - Check you used the correct authcode.
    - Make sure the cr50 version is greater than 3.3.
    - try generating another URL by rerunning the generate command and
      rerunning the process.
"""

DEBUG_DUT_CONTROL_OSERROR = """
Run from chroot if you are trying to use a /dev/pts ccd servo console
"""

class RMAOpen(object):
    """Used to find the cr50 console and run RMA open"""

    ENABLE_TESTLAB_CMD = 'ccd testlab enabled\n'

    def __init__(self, device=None, usb_serial=None, servo_port=None, ip=None):
        self.servo_port = servo_port if servo_port else '9999'
        self.ip = ip
        if device:
            self.set_cr50_device(device)
        elif servo_port:
            self.find_cr50_servo_uart()
        else:
            self.find_cr50_device(usb_serial)
        logging.info('DEVICE: %s', self.device)
        self.check_version()
        self.print_platform_info()
        logging.info('Cr50 setup ok')
        self.update_ccd_state()
        self.using_ccd = self.device_is_running_with_servo_ccd()

    def _dut_control(self, control):
        """Run dut-control and return the response"""
        try:
            cmd = ['dut-control', '-p', self.servo_port, control]
            return subprocess.check_output(cmd, encoding='utf-8').strip()
        except OSError:
            logging.warning(DEBUG_DUT_CONTROL_OSERROR)
            raise

    def find_cr50_servo_uart(self):
        """Save the device used for the console.

        Find the console and configure it, so it can be used with this script.
        """
        self._dut_control('cr50_uart_timestamp:off')
        self.device = self._dut_control('cr50_uart_pty').split(':')[-1]

    def set_cr50_device(self, device):
        """Save the device used for the console"""
        self.device = device

    def send_cmd_get_output(self, cmd, nbytes=0):
        """Send a cr50 command and get the output

        Args:
            cmd: The cr50 command string
            nbytes: The number of bytes to read from the console. If 0 read all
                    of the console output.
        Returns:
            The command output
        """
        try:
            ser = serial.Serial(self.device, timeout=1)
        except OSError:
            logging.warning('Permission denied %s', self.device)
            logging.warning('Try running cr50_rma_open with sudo')
            raise
        write_cmd = cmd + '\n\n'
        ser.write(write_cmd.encode('utf-8'))
        if nbytes:
            output = ser.read(nbytes)
        else:
            output = ser.readall()
        ser.close()

        output = output.decode('utf-8').strip() if output else ''
        # Return only the command output
        split_cmd = cmd + '\r'
        if cmd and split_cmd in output:
            return ''.join(output.rpartition(split_cmd)[1::]).split('>')[0]
        return output

    def device_is_running_with_servo_ccd(self):
        """Return True if the device is a servod ccd console"""
        # servod uses /dev/pts consoles. Non-servod uses /dev/ttyUSBX
        if '/dev/pts' not in self.device:
            return False
        # If cr50 doesn't show rdd is connected, cr50 the device must not be
        # a ccd device
        if 'Rdd:     connected' not in self.send_cmd_get_output('ccdstate'):
            return False
        # Check if the servod is running with ccd. This requires the script
        # is run in the chroot, so run it last.
        if 'ccd_cr50' not in self._dut_control('servo_type'):
            return False
        logging.info('running through servod ccd')
        return True

    def get_rma_challenge(self):
        """Get the rma_auth challenge

        There are two challenge formats

        "
        ABEQ8 UGA4F AVEQP SHCKV
        DGGPR N8JHG V8PNC LCHR2
        T27VF PRGBS N3ZXF RCCT2
        UBMKP ACM7E WUZUA A4GTN
        "
        and
        "
        generated challenge:

        CBYRYBEMH2Y75TC...rest of challenge
        "
        support extracting the challenge from both.

        Returns:
            The RMA challenge with all whitespace removed.
        """
        output = self.send_cmd_get_output('rma_auth').strip()
        logging.info('rma_auth output:\n%s', output)
        # Extract the challenge from the console output
        if 'generated challenge:' in output:
            return output.split('generated challenge:')[-1].strip()
        challenge = ''.join(re.findall(r' \S{5}' * 4, output))
        # Remove all whitespace
        return re.sub(r'\s', '', challenge)

    def generate_challenge_url(self, hwid):
        """Get the rma_auth challenge

        Returns:
            The RMA challenge with all whitespace removed.
        """

        challenge = self.get_rma_challenge()
        self.print_platform_info()
        logging.info('CHALLENGE: %s', challenge)
        logging.info('HWID: %s', hwid)
        url = URL % (challenge, hwid)
        logging.info('GOTO:\n %s', url)
        logging.info('If the server fails to debug the challenge make sure the '
                     'RLZ is whitelisted')

    def try_authcode(self, authcode):
        """Try opening cr50 with the authcode

        Raises:
            ValueError if there was no authcode match and ccd isn't open
        """
        # rma_auth may cause the system to reboot. Don't wait to read all that
        # output. Read the first 300 bytes and call it a day.
        output = self.send_cmd_get_output('rma_auth ' + authcode, nbytes=300)
        logging.info('CR50 RESPONSE: %s', output)
        logging.info('waiting for cr50 reboot')
        # Cr50 may be rebooting. Wait a bit
        time.sleep(5)
        if self.using_ccd:
            # After reboot, reset the ccd endpoints
            self._dut_control('power_state:ccd_reset')
        # Update the ccd state after the authcode attempt
        self.update_ccd_state()

        authcode_match = 'process_response: success!' in output
        if not self.check(CCD_IS_UNRESTRICTED):
            if not authcode_match:
                logging.warning(DEBUG_AUTHCODE_MISMATCH)
                message = 'Authcode mismatch. Check args and url'
            else:
                message = 'Could not set all capability privileges to Always'
            raise ValueError(message)

    def wp_is_force_disabled(self):
        """Returns True if write protect is forced disabled"""
        output = self.send_cmd_get_output('wp')
        wp_state = output.split('Flash WP:', 1)[-1].split('\n', 1)[0].strip()
        logging.info('wp: %s', wp_state)
        return wp_state == 'forced disabled'

    def testlab_is_enabled(self):
        """Returns True if testlab mode is enabled"""
        output = self.send_cmd_get_output('ccd testlab')
        testlab_state = output.split('mode')[-1].strip().lower()
        logging.info('testlab: %s', testlab_state)
        return testlab_state == 'enabled'

    def ccd_is_restricted(self):
        """Returns True if any of the capabilities are still restricted"""
        output = self.send_cmd_get_output('ccd')
        if 'Capabilities' not in output:
            raise ValueError('Could not get ccd output')
        logging.debug('CURRENT CCD SETTINGS:\n%s', output)
        restricted = 'IfOpened' in output or 'IfUnlocked' in output
        logging.info('ccd: %srestricted', '' if restricted else 'Un')
        return restricted

    def update_ccd_state(self):
        """Get the wp and ccd state from cr50. Save it in _ccd_state"""
        self._ccd_state = 0
        if not self.ccd_is_restricted():
            self._ccd_state |= CCD_IS_UNRESTRICTED
        if self.wp_is_force_disabled():
            self._ccd_state |= WP_IS_DISABLED
        if self.testlab_is_enabled():
            self._ccd_state |= TESTLAB_IS_ENABLED

    def check(self, setting):
        """Returns true if the all of the 1s in setting are 1 in _ccd_state"""
        return self._ccd_state & setting == setting

    def _has_testlab_support(self):
        """Return True if you can enable testlab mode"""
        # all prepvt images can enable testlab
        if self.is_prepvt:
            return True
        return not self._running_version_is_older(DEV_MODE_OPEN_PROD)

    def _capabilities_allow_open_from_console(self):
        """Return True if ccd open is Always allowed from usb"""
        output = self.send_cmd_get_output('ccd')
        return (re.search('OpenNoDevMode.*Always', output) and
                re.search('OpenFromUSB.*Always', output))

    def _requires_dev_mode_open(self):
        """Return True if the image requires dev mode to open"""
        if self._capabilities_allow_open_from_console():
            return False
        # All prod images that support 'open' require dev mode
        if not self.is_prepvt:
            return True
        return not self._running_version_is_older(DEV_MODE_OPEN_PREPVT)

    def _run_on_dut(self, command):
        """Run the command on the DUT."""
        return subprocess.check_output(['ssh', self.ip, command],
                                       encoding='utf-8')

    def _open_in_dev_mode(self):
        """Open Cr50 when it's in dev mode"""
        output = self.send_cmd_get_output('ccd')
        # If the device is already open, nothing needs to be done.
        if 'State: Open' not in output:
            # Verify the device is in devmode before trying to run open.
            if 'dev_mode' not in output:
                logging.warning('Enter dev mode to open ccd or update to %s',
                                TESTLAB_PROD)
                raise ValueError('DUT not in dev mode')
            if not self.ip:
                logging.warning("If your DUT doesn't have ssh support, run "
                                "'gsctool -a -o' from the AP")
                raise ValueError('Cannot run ccd open without dut ip')
            self._run_on_dut('gsctool -a -o')
            # Wait >1 second for cr50 to update ccd state
            time.sleep(3)
            output = self.send_cmd_get_output('ccd')
            if 'State: Open' not in output:
                raise ValueError('Could not open cr50')
        logging.info('ccd is open')

    def enable_testlab(self):
        """Disable write protect"""
        if not self._has_testlab_support():
            logging.warning('Testlab mode is not supported in prod iamges')
            return
        # Some cr50 images need to be in dev mode before they can be opened.
        if self._requires_dev_mode_open():
            self._open_in_dev_mode()
        else:
            self.send_cmd_get_output('ccd open')
        logging.info('Enabling testlab mode reqires pressing the power button.')
        logging.info('Once the process starts keep tapping the power button '
                     'for 10 seconds.')
        input("Press Enter when you're ready to start...")
        end_time = time.time() + 15

        ser = serial.Serial(self.device, timeout=1)
        printed_lines = ''
        output = ''
        # start ccd testlab enable
        ser.write(self.ENABLE_TESTLAB_CMD.encode('utf-8'))
        logging.info('start pressing the power button\n\n')
        # Print all of the cr50 output as we get it, so the user will have more
        # information about pressing the power button. Tapping the power button
        # a couple of times should do it, but this will give us more confidence
        # the process is still running/worked.
        try:
            while time.time() < end_time:
                output += ser.read(100).decode('utf-8')
                full_lines = output.rsplit('\n', 1)[0]
                new_lines = full_lines
                if printed_lines:
                    new_lines = full_lines.split(printed_lines, 1)[-1].strip()
                logging.info('\n%s', new_lines)
                printed_lines = full_lines

                # Make sure the process hasn't ended. If it has, print the last
                # of the output and exit.
                new_lines = output.split(printed_lines, 1)[-1]
                if 'CCD test lab mode enabled' in output:
                    # print the last of the ou
                    logging.info(new_lines)
                    break
                elif 'Physical presence check timeout' in output:
                    logging.info(new_lines)
                    logging.warning('Did not detect power button press in time')
                    raise ValueError('Could not enable testlab mode try again')
        finally:
            ser.close()
        # Wait for the ccd hook to update things
        time.sleep(3)
        # Update the state after attempting to disable write protect
        self.update_ccd_state()
        if not self.check(TESTLAB_IS_ENABLED):
            raise ValueError('Could not enable testlab mode try again')

    def wp_disable(self):
        """Disable write protect"""
        logging.info('Disabling write protect')
        self.send_cmd_get_output('wp disable')
        # Update the state after attempting to disable write protect
        self.update_ccd_state()
        if not self.check(WP_IS_DISABLED):
            raise ValueError('Could not disable write protect')

    def check_version(self):
        """Make sure cr50 is running a version that supports RMA Open"""
        output = self.send_cmd_get_output('version')
        if not output.strip():
            logging.warning(DEBUG_DEVICE, self.device)
            raise ValueError('Could not communicate with %s' % self.device)

        version = re.search(r'RW.*\* ([\d\.]+)/', output).group(1)
        logging.info('Running Cr50 Version: %s', version)
        self.running_ver_fields = [int(field) for field in version.split('.')]

        # prePVT images have even major versions. Prod have odd
        self.is_prepvt = self.running_ver_fields[1] % 2 == 0
        rma_support = RMA_SUPPORT_PREPVT if self.is_prepvt else RMA_SUPPORT_PROD

        logging.info('%s RMA support added in: %s',
                     'prePVT' if self.is_prepvt else 'prod', rma_support)
        if not self.is_prepvt and self._running_version_is_older(TESTLAB_PROD):
            raise ValueError('Update cr50. No testlab support in old prod '
                             'images.')
        if self._running_version_is_older(rma_support):
            raise ValueError('%s does not have RMA support. Update to at '
                             'least %s' % (version, rma_support))

    def _running_version_is_older(self, target_ver):
        """Returns True if running version is older than target_ver."""
        target_ver_fields = [int(field) for field in target_ver.split('.')]
        if target_ver_fields == self.running_ver_fields:
            return False
        for i, field in enumerate(self.running_ver_fields):
            if field > target_ver_fields[i]:
                return False
        return True

    def device_matches_devid(self, devid, device):
        """Return True if the device matches devid.

        Use the sysinfo output from device to determine if it matches devid

        Returns:
            True if sysinfo from device shows the given devid. False if there
            is no output or sysinfo doesn't contain the devid.
        """
        self.set_cr50_device(device)
        sysinfo = self.send_cmd_get_output('sysinfo')
        # Make sure there is some output, and it shows it's from Cr50
        if not sysinfo or 'cr50' not in sysinfo:
            return False
        logging.debug('Sysinfo output: %s', sysinfo)
        # The cr50 device id should be in the sysinfo output, if we found
        # the right console. Make sure it is
        return devid in sysinfo

    def find_cr50_device(self, usb_serial):
        """Find the cr50 console device

        The Cr50 usb serialname matches the cr50 devid. Convert the serialname
        to devid. Use that to check all of the consoles and find cr50's.

        Args:
            usb_serial: an optional string. The serialname of the cr50 usb
                        device
        Raises:
            ValueError if the console can't be found with the given serialname
        """
        usb_serial = self.find_cr50_usb(usb_serial)
        logging.info('SERIALNAME: %s', usb_serial)
        devid = '0x' + ' 0x'.join(usb_serial.lower().split('-'))
        logging.info('DEVID: %s', devid)

        # Get all the usb devices
        devices = glob.glob('/dev/ttyUSB*')
        # Typically Cr50 has the lowest number. Sort the devices, so we're more
        # likely to try the cr50 console first.
        devices.sort()

        # Find the one that is the cr50 console
        for device in devices:
            logging.info('testing %s', device)
            if self.device_matches_devid(devid, device):
                logging.info('found device: %s', device)
                return
        logging.warning(DEBUG_CONNECTION)
        raise ValueError('Found USB device, but could not communicate with '
                         'cr50 console')

    def print_platform_info(self):
        """Print the cr50 BID RLZ code"""
        bid_output = self.send_cmd_get_output('bid')
        bid = re.search(r'Board ID: (\S+?)[:,]', bid_output).group(1)
        if bid == ERASED_BID:
            logging.warning(DEBUG_ERASED_BOARD_ID)
            raise ValueError('Cannot run RMA Open when board id is erased')
        bid = int(bid, 16)
        chrs = [chr((bid >> (8 * i)) & 0xff) for i in range(4)]
        logging.info('RLZ: %s', ''.join(chrs[::-1]))

    @staticmethod
    def find_cr50_usb(usb_serial):
        """Make sure the Cr50 USB device exists"""
        try:
            output = subprocess.check_output(CR50_LSUSB_CMD, encoding='utf-8')
        except:
            logging.warning(DEBUG_MISSING_USB)
            raise ValueError('Could not find Cr50 USB device')
        serialnames = re.findall(r'iSerial +\d+ (\S+)\s', output)
        if usb_serial:
            if usb_serial not in serialnames:
                logging.warning(DEBUG_SERIALNAME)
                raise ValueError('Could not find usb device "%s"' % usb_serial)
            return usb_serial
        if len(serialnames) > 1:
            logging.info('Found Cr50 device serialnames %s',
                         ', '.join(serialnames))
            logging.warning(DEBUG_TOO_MANY_USB_DEVICES)
            raise ValueError('Too many cr50 usb devices')
        return serialnames[0]

    def print_dut_state(self):
        """Print CCD RMA and testlab mode state."""
        if not self.check(CCD_IS_UNRESTRICTED):
            logging.info('CCD is still restricted.')
            logging.info('Run cr50_rma_open.py -g -i $HWID to generate a url')
            logging.info('Run cr50_rma_open.py -a $AUTHCODE to open cr50 with '
                         'an authcode')
        elif not self.check(WP_IS_DISABLED):
            logging.info('WP is still enabled.')
            logging.info('Run cr50_rma_open.py -w to disable write protect')
        if self.check(RMA_OPENED):
            logging.info('RMA Open complete')

        if not self.check(TESTLAB_IS_ENABLED) and self.is_prepvt:
            logging.info('testlab mode is disabled.')
            logging.info('If you are prepping a device for the testlab, you '
                         'should enable testlab mode.')
            logging.info('Run cr50_rma_open.py -t to enable testlab mode')


def parse_args(argv):
    """Get cr50_rma_open args."""
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-g', '--generate_challenge', action='store_true',
                        help='Generate Cr50 challenge. Must be used with -i')
    parser.add_argument('-t', '--enable_testlab', action='store_true',
                        help='enable testlab mode')
    parser.add_argument('-w', '--wp_disable', action='store_true',
                        help='Disable write protect')
    parser.add_argument('-c', '--check_connection', action='store_true',
                        help='Check cr50 console connection works')
    parser.add_argument('-s', '--serialname', type=str, default='',
                        help='The cr50 usb serialname')
    parser.add_argument('-D', '--debug', action='store_true',
                        help='print debug messages')
    parser.add_argument('-d', '--device', type=str, default='',
                        help='cr50 console device ex /dev/ttyUSB0')
    parser.add_argument('-i', '--hwid', type=str, default='',
                        help='The board hwid. Needed to generate a challenge')
    parser.add_argument('-a', '--authcode', type=str, default='',
                        help='The authcode string from the challenge url')
    parser.add_argument('-P', '--servo_port', type=str, default='',
                        help='the servo port')
    parser.add_argument('-I', '--ip', type=str, default='',
                        help='The DUT IP. Necessary to do ccd open')
    return parser.parse_args(argv)


def main(argv):
    """Run cr50 rma open."""
    opts = parse_args(argv)

    loglevel = logging.INFO
    log_format = '%(levelname)7s'
    if opts.debug:
        loglevel = logging.DEBUG
        log_format += ' - %(lineno)3d:%(funcName)-15s'
    log_format += ' - %(message)s'
    logging.basicConfig(level=loglevel, format=log_format)

    tried_authcode = False
    logging.info('Running cr50_rma_open version %s', SCRIPT_VERSION)

    cr50_rma_open = RMAOpen(opts.device, opts.serialname, opts.servo_port,
                            opts.ip)
    if opts.check_connection:
        sys.exit(0)

    if not cr50_rma_open.check(CCD_IS_UNRESTRICTED):
        if opts.generate_challenge:
            if not opts.hwid:
                logging.warning('--hwid necessary to generate challenge url')
                sys.exit(0)
            cr50_rma_open.generate_challenge_url(opts.hwid)
            sys.exit(0)
        elif opts.authcode:
            logging.info('Using authcode: %s', opts.authcode)
            cr50_rma_open.try_authcode(opts.authcode)
            tried_authcode = True

    if not cr50_rma_open.check(WP_IS_DISABLED) and (tried_authcode or
                                                    opts.wp_disable):
        if not cr50_rma_open.check(CCD_IS_UNRESTRICTED):
            raise ValueError("Can't disable write protect unless ccd is "
                             "open. Run through the rma open process first")
        if tried_authcode:
            logging.warning('RMA Open did not disable write protect. File a '
                            'bug')
            logging.warning('Trying to disable it manually')
        cr50_rma_open.wp_disable()

    if not cr50_rma_open.check(TESTLAB_IS_ENABLED) and opts.enable_testlab:
        if not cr50_rma_open.check(CCD_IS_UNRESTRICTED):
            raise ValueError("Can't enable testlab mode unless ccd is open."
                             "Run through the rma open process first")
        cr50_rma_open.enable_testlab()

    cr50_rma_open.print_dut_state()


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
