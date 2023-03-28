#-------------------------------------------------------------------------------
# Copyright (C) 2016 Freescale Semiconductor, Inc.
# Copyright 2016-2020 NXP
# All rights reserved.
#
# THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
# SHALL FREESCALE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
# OF SUCH DAMAGE.
#-------------------------------------------------------------------------------

.NOTPARALLEL:

this_makefile := $(firstword $(MAKEFILE_LIST))
ERPC_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

include $(ERPC_ROOT)/mk/erpc_common.mk

ERPC_C_ROOT = $(ERPC_ROOT)

# TARGET_OUTPUT_ROOT = $(OUTPUT_ROOT)/$(DEBUG_OR_RELEASE)/$(os_name)/$(APP_NAME)

#-----------------------------------------------
# setup variables
# ----------------------------------------------

LIB_NAME = erpc

TARGET_OUTPUT_ROOT = $(OUTPUT_ROOT)/$(DEBUG_OR_RELEASE)/$(os_name)/$(LIB_NAME)

TARGET_LIB = $(LIBS_ROOT)/lib$(LIB_NAME).a

OBJS_ROOT = $(TARGET_OUTPUT_ROOT)/obj

LIBS_ROOT = $(TARGET_OUTPUT_ROOT)/lib

#-----------------------------------------------
# Include path. Add the include paths like this:
# INCLUDES += ./include/
#-----------------------------------------------
INCLUDES += $(ERPC_C_ROOT)/infra \
			$(ERPC_C_ROOT)/infra/codec \
			$(ERPC_C_ROOT)/infra/client \
			$(ERPC_C_ROOT)/infra/server \
			$(ERPC_C_ROOT)/infra/transport \
			$(ERPC_C_ROOT)/infra/port \
			$(ERPC_C_ROOT)/infra/port/threading\
			$(ERPC_C_ROOT)/infra/port/malloc_free\
			$(ERPC_C_ROOT)/infra/port/log 

			

SOURCES += 	$(foreach dir,$(subst ' ', ,$(INCLUDES)),$(wildcard $(dir)/*.cpp))
SOURCES +=  $(foreach dir,$(subst ' ', ,$(INCLUDES)),$(wildcard $(dir)/*.c))
CXXFLAGS += -g
CFLAGS += -g

MAKE_TARGET = $(TARGET_LIB)($(OBJECTS_ALL))

include $(ERPC_ROOT)/mk/targets.mk

$(TARGET_LIB)(%): %
	@$(call printmessage,ar,Archiving, $(?F) in $(@F))
	$(at)mkdir -p $(dir $(@))
	$(at)$(AR) $(ARFLAGS) $@ $?


clean::
	$(at)rm -rf $(OBJS_ROOT)/*.cpp $(OBJS_ROOT)/*.hpp $(OBJS_ROOT)/*.c
