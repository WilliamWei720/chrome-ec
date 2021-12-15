#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright 2015 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Module for initializing and driving a SPI TPM."""

from __future__ import print_function

import getopt
import os
import struct
import sys
import traceback

# Suppressing pylint warning about an import not at the top of the file. The
# path needs to be set *before* the last import.
# pylint: disable=wrong-import-position
ROOT_DIR = os.path.dirname(os.path.abspath(sys.argv[0]))
sys.path.append(os.path.join(ROOT_DIR, '..', '..', 'build', 'tpm_test'))

import crypto_test
import drbg_test
import ecc_test
import ecies_test
import ftdi_spi_tpm
import hash_test
import hkdf_test
import rsa_test
import subcmd
import trng_test
import upgrade_test
import u2f_test

# Extension command for dcypto testing
EXT_CMD = 0xbaccd00a


class TPM:
    """TPM accessor class.

    Object of this class allows to send valid and extended TPM commands (using
    the command() method. The wrap_command/unwrap_response methods provide a
    means of encapsulating extended commands in proper TPM data packets, as well
    as extracting extended command responses.

    Attributes:
        _handle: a ftdi_spi_tpm object, a USB/FTDI/SPI driver which allows
        communicate with a TPM connected over USB dongle.
    """

    HEADER_FMT = '>H2IH'
    STARTUP_CMD = '80 01 00 00 00 0c 00 00 01 44 00 00'
    STARTUP_RSP = ('80 01 00 00 00 0a 00 00 00 00',
                   '80 01 00 00 00 0a 00 00 01 00')

    def __init__(self, freq=2000*1000, debug_mode=False):
        self._debug_enabled = debug_mode
        self._handle = ftdi_spi_tpm
        if not self._handle.FtdiSpiInit(freq, debug_mode):
            raise subcmd.TpmTestError('Failed to connect')

    def parse_response(self, data_blob):
        (tag, size, cmd_code, _) = struct.unpack_from(
          self.HEADER_FMT, data_blob + b'  ')
        return tag, size, cmd_code

    def validate(self, data_blob, response_mode=False, check_cmd=True):
        """Validate TPM header format

        Check if a data blob complies with TPM command/response
        header format.
        """
        tag, size, cmd_code = self.parse_response(data_blob)
        prefix = 'Misformatted blob: '
        if tag not in (0x8001, 0x8002):
            raise subcmd.TpmTestError(prefix + 'bad tag value 0x%4.4x' % tag)
        if size != len(data_blob):
            raise subcmd.TpmTestError(prefix +
                                      'size mismatch: header %d, actual %d'
                                      % (size, len(data_blob)))
        if size > 4096:
            raise subcmd.TpmTestError(prefix + 'invalid size %d' % size)
        if response_mode:
            # Startup response code, extension or vendor command response code
            if cmd_code not in (0, 0x100, 0x500):
                raise subcmd.TpmTestError(
                  prefix + 'invalid response code 0x%x' % cmd_code)
            return
        if check_cmd == False:
            return size, cmd_code
        if 0x11f <= cmd_code <= 0x18f:
            return size, cmd_code # This is a valid command
        if cmd_code == EXT_CMD:
            return size, cmd_code # This is an extension command
        if 0x20000000 <= cmd_code <= 0x200001ff:
            return size, cmd_code # this is vendor command
        raise subcmd.TpmTestError(prefix + 'invalid command code 0x%x'
                                  % cmd_code)

    def command(self, cmd_data):
        """Verify command header"""
        self.validate(cmd_data)
        response = self._handle.FtdiSendCommandAndWait(cmd_data)
        self.validate(response, response_mode=True)
        return response

    def command_unchecked(self, cmd_data):
        """Verify command header"""
        self.validate(cmd_data)
        response = self._handle.FtdiSendCommandAndWait(cmd_data)
        size, cmd_code = self.validate(response, response_mode=False, check_cmd=False)
        return response, size, cmd_code

    def wrap_ext_command(self, subcmd_code, cmd_body):
        """Wrap TPM command into extension command header"""
        return struct.pack(self.HEADER_FMT, 0x8001,
                           len(cmd_body) + struct.calcsize(self.HEADER_FMT),
                           EXT_CMD, subcmd_code) + cmd_body

    def unwrap_ext_response(self, expected_subcmd, response):
        """Verify basic validity and strip off TPM extended command header.

        Get the response generated by the device, as it came off the wire,
        verify that header fields match expectations, then strip off the
        extension command header and return the payload to the caller.

        Args:
            expected_subcmd: an int, up to 16 bits in size, the extension
                command this response is supposed to be for.
            response: a binary string, the actual response received
                over the wire.

        Returns:
            the binary string of the response payload,
            if validation succeeded.

        Raises:
            subcmd.TpmTestError: in case there are any validation problems,
            the error message describes the problem.
        """
        header_size = struct.calcsize(self.HEADER_FMT)
        tag, size, cmd, sub = struct.unpack(self.HEADER_FMT,
                                            response[:header_size])
        if tag != 0x8001:
            raise subcmd.TpmTestError('Wrong response tag: %4.4x' % tag)
        if cmd:
            raise subcmd.TpmTestError('Unexpected response command'
                                      ' field: %8.8x' % cmd)
        if sub != expected_subcmd:
            raise subcmd.TpmTestError('Unexpected response subcommand'
                                      ' field: %2.2x' % sub)
        if size != len(response):
            raise subcmd.TpmTestError('Size mismatch: header %d, actual %d' %
                                      (size, len(response)))
        return response[header_size:]

    def debug_enabled(self):
        """Return status of debugging"""
        return self._debug_enabled

found_valid_test = False
valid_tests = []

def run_test(requested_test, test_name):
    """Returns True if the test arg is none or it matches the test_name."""
    global found_valid_test

    run_test = (not requested_test) or (requested_test == test_name)
    valid_tests.append(test_name)
    if run_test:
        found_valid_test = True
    return run_test

def check_for_run(requested_test):
    """Raises an ValueError if no tests were run."""
    if not found_valid_test:
        raise ValueError('%r is not a valid test. Use one of %r' %
                         (requested_test, valid_tests))

def usage():
    """Print usage information"""
    print('Syntax: tpmtest.py [-d] | [-t source [-o file] [-s bits] ]| -h | '
          '                           -T test_name ]\n'
          '     -d - prints additional debug information during tests\n'
          '     -t source - only dump raw output from TRNG. source values:\n'
          '        0 - raw TRNG'
          '        [-o file] - set output file, default /tmp/trng_output\n'
          '        [-s bits] - TRNG sample size in bit, default = 1\n'
          '     -l path to output lab result vectors.\n'
          '     -r path for the lab input vector.\n'
          '     -e path for the lab expected results file\n'
          '     -T test to run\n'
          '     -h - this help\n')

def main():
    """Run TPM tests"""
    try:
        opts, _ = getopt.getopt(sys.argv[1:], 'dt:T:hs:o:r:e:l:', 'help')
    except getopt.GetoptError as err:
        print(str(err))
        usage()
        sys.exit(2)
    debug_needed = False
    trng_only = False
    trng_output = '/tmp/trng_output'
    trng_sample_bits = 1
    trng_mode = 0
    lab_output = '/tmp/lab_output'
    request = ''
    expected = ''
    requested_test = ''

    for option, arg in opts:
        if option == '-d':
            debug_needed = True
        elif option == '-t':
            trng_only = True
            trng_mode = int(arg)
        elif option == '-o':
            trng_output = arg
        elif option == '-l':
            lab_output = arg
        elif option == '-r':
            request = arg
        elif option == '-T':
            requested_test = arg
        elif option == '-e':
            expected = arg
        elif option == '-s':
            trng_sample_bits = int(arg)
        elif option in ('-h', '--help'):
            usage()
            sys.exit(0)
    try:
        tpm_object = TPM(debug_mode=debug_needed)
        if trng_only:
            trng_test.trng_test(tpm_object, trng_output,
                                trng_mode, trng_sample_bits)
            sys.exit(0)
        if run_test(requested_test, 'u2f'):
            u2f_test.u2f_test(tpm_object)
        if run_test(requested_test, 'crypto'):
            crypto_test.crypto_tests(tpm_object, os.path.join(ROOT_DIR,
                                                             'crypto_test.xml'))
        if run_test(requested_test, 'drbg'):
            drbg_test.drbg_test(tpm_object, request, expected,
                               lab_output)
        if run_test(requested_test, 'ecc'):
            ecc_test.ecc_test(tpm_object)
#       cr50 don't implement ecies
#       ecies_test.ecies_test(tpm_object)
        if run_test(requested_test, 'hash'):
            hash_test.hash_test(tpm_object)
        if run_test(requested_test, 'hkdf'):
            hkdf_test.hkdf_test(tpm_object)

        if run_test(requested_test, 'rsa'):
            rsa_test.rsa_test(tpm_object)
        if run_test(requested_test, 'upgrade'):
            upgrade_test.upgrade(tpm_object)
        check_for_run(requested_test)
    except subcmd.TpmTestError as tpm_exc:
        _, _, exc_traceback = sys.exc_info()
        exc_file, exc_line = traceback.extract_tb(exc_traceback)[-1][:2]
        print('\nError in %s:%s: ' % (os.path.basename(exc_file),
              exc_line), tpm_exc)
        if debug_needed:
            traceback.print_exc()
        sys.exit(1)

if __name__ == '__main__':
    main()
