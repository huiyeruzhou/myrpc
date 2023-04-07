# Include the nanopb provided Makefile rules
# This is an include file for Makefiles. It provides rules for building
# .pb.c and .pb.h files out of .proto, as well the path to nanopb core.

# Path to the nanopb root directory
NANOPB_DIR := ../nanopb
# Files for the nanopb core
NANOPB_CORE = $(NANOPB_DIR)/pb_encode.c $(NANOPB_DIR)/pb_decode.c $(NANOPB_DIR)/pb_common.c

# Check if we are running on Windows
ifdef windir
WINDOWS = 1
endif
ifdef WINDIR
WINDOWS = 1
endif

# Check whether to use binary version of nanopb_generator or the
# system-supplied python interpreter.
ifneq "$(wildcard $(NANOPB_DIR)/generator-bin)" ""
	# Binary package
	PROTOC = $(NANOPB_DIR)/generator-bin/protoc
	PROTOC_OPTS =
else
	# Source only or git checkout
	PROTOC_OPTS =
	ifdef WINDOWS
	    PROTOC = python $(NANOPB_DIR)/generator/protoc
	else
	    PROTOC = $(NANOPB_DIR)/generator/protoc
	endif
endif
CXXFLAGS += -g $(INCLUDE) -lpthread
ERPC_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../)
ERPC_LIB=$(ERPC_ROOT)/Debug/Linux/erpc/lib/liberpc.a
# Rule for building .pb.c and .pb.h
%.pb.c %.pb.h: %.proto %.options
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. $<

%.pb.c %.pb.h: %.proto
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. $<

# Compiler flags to enable all warnings & debug info
CFLAGS = -Wall -Werror -g -O0
CXXFLAGS += "-I$(NANOPB_DIR)" -I../include

# C source code files that are required
SRC  = server.cpp                   # The main program
CRC  = client.cpp                   # The main program
SRC  += idl.pb.c                  # The main program
CRC += idl.pb.c                # The compiled protocol definition
SRC += $(NANOPB_DIR)/pb_encode.c  # The nanopb encoder
SRC += $(NANOPB_DIR)/pb_decode.c  # The nanopb decoder
SRC += $(NANOPB_DIR)/pb_common.c  # The nanopb common parts
CRC += $(NANOPB_DIR)/pb_encode.c  # The nanopb encoder
CRC += $(NANOPB_DIR)/pb_decode.c  # The nanopb decoder
CRC += $(NANOPB_DIR)/pb_common.c  # The nanopb common parts

# Build rule for the main program
server: $(SRC) $(ERPC_LIB)
	$(CXX) $(CXXFLAGS) -osimple $^

# Build rule for the main program
client: $(CRC) $(ERPC_LIB) 
	$(CXX) $(CXXFLAGS) -osimpleclient $^

# Build rule for the protocol
idl.pb.c: idl.proto
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. $<