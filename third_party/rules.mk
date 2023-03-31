# -*- makefile -*-
# vim: set filetype=make :
# Copyright 2020 The ChromiumOS Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Embedded Controller firmware build system - third_party rules/targets
#

# Build and link against libcryptoc.
# See https://chromium.googlesource.com/chromiumos/third_party/cryptoc .
ifeq ($(CONFIG_LIBCRYPTOC),y)

# The cryptoc path can be overridden on invocation, as in the following example:
# $ make CRYPTOC_DIR=~/src/cryptoc BOARD=bloonchipper
CRYPTOC_DIR ?= ../../third_party/cryptoc

# The boringssl path can be overridden on invocation, as in the following
# example: $ make BORINGSSL_DIR=~/src/boringssl BOARD=bloonchipper
BORINGSSL_DIR ?= ../../third_party/boringssl

# SUPPORT_UNALIGNED indicates to libcryptoc that provided data buffers
# may be unaligned and please handle them safely.
cmd_libcryptoc = $(MAKE) -C $(CRYPTOC_DIR) \
	obj=$(realpath $(out))/cryptoc \
	SUPPORT_UNALIGNED=1
cmd_libcryptoc_clean = $(cmd_libcryptoc) -q && echo clean

CPPFLAGS += -I$(CRYPTOC_DIR)/include
CRYPTOC_LDFLAGS := -L$(out)/cryptoc -lcryptoc

ifeq ($(CONFIG_BORINGSSL_CRYPTO), y)

ifeq ($(CROSS_COMPILE), armv7m-cros-eabi-)
CMAKE_SYSTEM_PROCESSOR ?= armv7
OPENSSL_NO_ASM ?= 0
else ifeq ($(CROSS_COMPILE), x86_64-pc-linux-gnu-)
CMAKE_SYSTEM_PROCESSOR ?= x86_64
OPENSSL_NO_ASM ?= 0
else
$(error ERROR: Unknown compiler: $(CROSS_COMPILE))
endif

BORINGSSL_OUTDIR := $(out)/third_party/boringssl/crypto
BORINGSSL_TOOLCHAIN := \
	$(shell pwd)/third_party/boringssl/boringssl-toolchain.cmake

$(BORINGSSL_OUTDIR)/libcrypto.a:
	mkdir -p $(out)/third_party/boringssl/
	cmake \
		-DCC_NAME=$(cc-name) \
		-DCXX_NAME=$(cxx-name) \
		-DCROSS_COMPILE=$(CROSS_COMPILE) \
		-DCMAKE_SYSTEM_PROCESSOR=$(CMAKE_SYSTEM_PROCESSOR) \
		-DCMAKE_SYSROOT=$(SYSROOT) \
		-DOPENSSL_NO_ASM=$(OPENSSL_NO_ASM) \
		-DCROS_EC_REPO=$(CURDIR) \
		-DCMAKE_TOOLCHAIN_FILE=$(BORINGSSL_TOOLCHAIN) \
		-B $(out)/third_party/boringssl/ \
		-S $(BORINGSSL_DIR) \
		-GNinja
	ninja -C $(out)/third_party/boringssl/ crypto

# Make sure the EC/FPMCU code can link to the boringssl library.
CPPFLAGS += -I$(BORINGSSL_DIR) -I$(BORINGSSL_DIR)/include
BORINGSSL_LDFLAGS := -L$(BORINGSSL_OUTDIR) -lcrypto

# And the custom helpers.
CPPFLAGS += -I$(shell pwd)/third_party/boringssl/include

# Disable the unsupported features to prevent the usage of pthread & socket
# related types in headers.
# TODO(b/273639386): Remove these workarounds when the upstream supports
# better way to disable the filesystem, threads and locks usages.
CPPFLAGS += -D__TRUSTY__
endif

# Conditionally force the rebuilding of libcryptoc.a only if it would be
# changed.
# Note, we use ifndef to ensure the likelyhood of rebuilding is much higher.
# For example, if variable cmd_libcryptoc_clean is modified or blank,
# we will rebuild.
ifneq ($(shell $(cmd_libcryptoc_clean)),clean)
.PHONY: $(out)/cryptoc/libcryptoc.a
endif
# Rewrite the CFLAGS include paths to be absolute, since cryptoc is built
# using a different working directory. This is only relevant because
# cryptoc makes use of stdlibs, which EC provides from the builtin/ directory.
$(out)/cryptoc/libcryptoc.a: CFLAGS := $(patsubst -I%,-I$(abspath %),$(CFLAGS))
$(out)/cryptoc/libcryptoc.a:
	+$(call quiet,libcryptoc,MAKE   )

# Link RO and RW against cryptoc.
$(out)/RO/ec.RO.elf $(out)/RO/ec.RO_B.elf: LDFLAGS_EXTRA += $(CRYPTOC_LDFLAGS)
$(out)/RO/ec.RO.elf $(out)/RO/ec.RO_B.elf: $(out)/cryptoc/libcryptoc.a
$(out)/RW/ec.RW.elf $(out)/RW/ec.RW_B.elf: LDFLAGS_EXTRA += $(CRYPTOC_LDFLAGS)
$(out)/RW/ec.RW.elf $(out)/RW/ec.RW_B.elf: $(out)/cryptoc/libcryptoc.a
ifeq ($(CONFIG_BORINGSSL_CRYPTO), y)
$(out)/RO/ec.RO.elf $(out)/RO/ec.RO_B.elf: LDFLAGS_EXTRA += $(BORINGSSL_LDFLAGS)
$(out)/RO/ec.RO.elf $(out)/RO/ec.RO_B.elf: $(BORINGSSL_OUTDIR)/libcrypto.a
$(out)/RW/ec.RW.elf $(out)/RW/ec.RW_B.elf: LDFLAGS_EXTRA += $(BORINGSSL_LDFLAGS)
$(out)/RW/ec.RW.elf $(out)/RW/ec.RW_B.elf: $(BORINGSSL_OUTDIR)/libcrypto.a
endif

# Host test executables (including fuzz tests).
$(out)/$(PROJECT).exe: LDFLAGS_EXTRA += $(CRYPTOC_LDFLAGS)
$(out)/$(PROJECT).exe: $(out)/cryptoc/libcryptoc.a
ifeq ($(CONFIG_BORINGSSL_CRYPTO), y)
$(out)/$(PROJECT).exe: LDFLAGS_EXTRA += $(BORINGSSL_LDFLAGS)
$(out)/$(PROJECT).exe: $(BORINGSSL_OUTDIR)/libcrypto.a
endif

# On-device tests.
third-party-test-targets=$(foreach test,$(test-list-y),\
	$(out)/RW/$(test).RW.elf $(out)/RO/$(test).RO.elf)
$(third-party-test-targets): LDFLAGS_EXTRA += $(CRYPTOC_LDFLAGS)
$(third-party-test-targets): $(out)/cryptoc/libcryptoc.a
ifeq ($(CONFIG_BORINGSSL_CRYPTO), y)
$(third-party-test-targets): LDFLAGS_EXTRA += $(BORINGSSL_LDFLAGS)
$(third-party-test-targets): $(BORINGSSL_OUTDIR)/libcrypto.a
endif

endif # CONFIG_LIBCRYPTOC

# Build and link against googletest in *test* builds if configured.
ifeq ($(TEST_BUILD),y)
ifeq ($(CONFIG_GOOGLETEST),y)

# The googletest path can be overridden on invocation. For example:
# $ make GOOGLETEST_DIR=~/src/googletest BOARD=bloonchipper
GOOGLETEST_DIR ?= $(realpath ../../third_party/googletest)
GOOGLETEST_INSTALL_DIR := $(realpath $(out))/googletest/install
CMAKE_TOOLCHAIN_FILE := $(CURDIR)/cmake/toolchain-armv7m.cmake

GOOGLETEST_CFLAGS := -I$(GOOGLETEST_INSTALL_DIR)/include
GOOGLETEST_LDFLAGS := -L$(GOOGLETEST_INSTALL_DIR)/lib -lgtest
GOOGLETEST_LIB := $(GOOGLETEST_INSTALL_DIR)/lib/libgtest.a

$(GOOGLETEST_LIB):
	mkdir -p $(out)/googletest && \
	cd $(out)/googletest && \
	cmake -Dgtest_disable_pthreads=ON \
		-GNinja \
		-DCMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE) \
		-DCMAKE_INSTALL_PREFIX=$(GOOGLETEST_INSTALL_DIR) \
		$(GOOGLETEST_DIR) && \
	cmake --build . && \
	cmake --install .

# On-device tests.
third-party-test-targets=$(foreach test,$(test-list-y),\
	$(out)/RW/$(test).RW.elf $(out)/RO/$(test).RO.elf)
$(third-party-test-targets): LDFLAGS_EXTRA += $(GOOGLETEST_LDFLAGS)
$(third-party-test-targets): CFLAGS += $(GOOGLETEST_CFLAGS)
$(third-party-test-targets): $(GOOGLETEST_LIB)

# Test files can include googletest headers, so the headers need to be
# installed first.
$(ro-objs): $(GOOGLETEST_LIB)
$(rw-objs): $(GOOGLETEST_LIB)

endif # CONFIG_GOOGLETEST
endif # TEST_BUILD
