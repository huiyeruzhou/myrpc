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

# Rule for building .pb.c and .pb.h
%.pb.cpp %.pb.hpp: %.proto %.options
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. $<

%.pb.cpp %.pb.hpp: %.proto
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. $<

# Compiler flags to enable all warnings & debug info
CFLAGS = -Wall -Werror -g -O0
CXXFLAGS += "-I$(NANOPB_DIR)" -I../include

# Path to erpc root directory
ERPC_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../)
ERPC_LIB=$(ERPC_ROOT)/Debug/Linux/erpc/lib/liberpc.a
CXXFLAGS += -g $(INCLUDE) -lpthread


# C source code files that are required
SRC  = server.cpp idl.pb.cpp ${NANOPB_CORE}            # The main program
CRC  = client.cpp idl.pb.cpp ${NANOPB_CORE}            # The main program

.PHONY: all clean
all: server client
clean:
	rm -f server client *.pb.cpp *.pb.hpp
# Build rule for the main program
server: $(SRC) $(ERPC_LIB)
	$(CXX) $(CXXFLAGS) -oserver $^

# Build rule for the main program
client: $(CRC) $(ERPC_LIB) 
	$(CXX) $(CXXFLAGS) -oclient $^
