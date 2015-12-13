# Copyright (c) 2013 The Native Client Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

#
# GNU Make based build file.  For details on GNU Make see:
# http://www.gnu.org/software/make/manual/make.html
#

#
# Get pepper directory for toolchain and includes.
#
# If NACL_SDK_ROOT is not set, then assume it can be found three directories up.
#
THIS_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT ?= $(abspath $(dir $(THIS_MAKEFILE))../nacl_sdk/pepper_46)

# Project Build flags
WARNINGS := -Wno-c++11-extensions -Wno-long-long -Wall -Wswitch-enum -pedantic -Werror
CXXFLAGS := -pthread -std=gnu++11 $(WARNINGS)

#
# Compute tool paths
#
GETOS := python $(NACL_SDK_ROOT)/tools/getos.py
OSHELPERS = python $(NACL_SDK_ROOT)/tools/oshelpers.py
OSNAME := $(shell $(GETOS))
RM := $(OSHELPERS) rm
CP := $(OSHELPERS) cp

PNACL_TC_PATH := $(abspath $(NACL_SDK_ROOT)/toolchain/$(OSNAME)_pnacl)
PNACL_TOOLS_PATH := $(abspath $(NACL_SDK_ROOT)/tools)
PNACL_CXX := $(PNACL_TC_PATH)/bin/pnacl-clang++
PNACL_FINALIZE := $(PNACL_TC_PATH)/bin/pnacl-finalize
PNACL_TRANSLATE := $(PNACL_TC_PATH)/bin/pnacl-translate
PNACL_SEL_LDR := $(PNACL_TC_PATH)/bin/pnacl-translate
CXXFLAGS := -I$(NACL_SDK_ROOT)/include -Isrc
LDFLAGS := -lppapi_cpp -lppapi
TEST_LDFLAGS := -lppapi_simple_cpp $(LDFLAGS) -lnacl_io -lgtest

#
# Disable DOS PATH warning when using Cygwin based tools Windows
#
CYGWIN ?= nodosfilewarning
export CYGWIN


# Declare the ALL target first, to make the 'all' target the default build
all: dist/capture.pexe dist/capture.nmf
test: all build/capture_suite.pexe

test_run: test
	$(PNACL_TRANSLATE) -arch x86-64 \
		build/capture_suite.pexe \
		-o build/capture_suite.nexe
	$(PNACL_TOOLS_PATH)/sel_ldr_x86_64 \
		-B $(PNACL_TOOLS_PATH)/irt_core_x86_64.nexe \
		build/capture_suite.nexe

clean_test:
	$(RM) build/capture_suite.nexe
	$(RM) build/capture_suite.pexe
	$(RM) build/capture_suite.bc

clean: clean_test
	$(RM) dist/capture.pexe
	$(RM) dist/capture.nmf
	$(RM) build/capture.bc

build/capture.bc: src/capture.cc
	$(PNACL_CXX) -o $@ \
		src/ivf_writer.cc \
		src/remote_control.cc \
		src/frame_advancer.cc \
		src/configurer.cc \
		$< -O2 $(CXXFLAGS) -L$(NACL_SDK_ROOT)/lib/pnacl/Release $(LDFLAGS)

build/capture_suite.bc: src/test/capture_suite.cc
	$(PNACL_CXX) -o $@ \
		src/ivf_writer.cc \
		src/remote_control.cc \
		src/frame_advancer.cc \
		src/configurer.cc \
		src/test/timecop_test.cc \
		src/test/ivf_writer_test.cc \
    src/test/remote_control_test.cc \
		src/test/frame_advancer_test.cc \
		$< -Os $(CXXFLAGS) -L$(NACL_SDK_ROOT)/lib/pnacl/Release $(TEST_LDFLAGS)

dist/capture.pexe: build/capture.bc
	$(PNACL_FINALIZE) -o $@ $<

build/capture_suite.pexe: build/capture_suite.bc
	$(PNACL_FINALIZE) -o $@ $<

dist/capture.nmf:
	$(CP) src/capture.nmf $@
