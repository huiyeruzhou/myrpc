include ../mk/common.mk
ERPC_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../)
ERPC_C_ROOT = $(ERPC_ROOT)
ERPC_LIB=$(ERPC_ROOT)/Debug/Linux/erpc/lib/liberpc.a
INCLUDES += $(ERPC_C_ROOT)/infra \
			$(ERPC_C_ROOT)/infra/codec \
			$(ERPC_C_ROOT)/infra/client \
			$(ERPC_C_ROOT)/infra/server \
			$(ERPC_C_ROOT)/infra/transport \
			$(ERPC_C_ROOT)/infra/port \
			$(ERPC_C_ROOT)/infra/port/threading\
			$(ERPC_C_ROOT)/infra/port/malloc_free\
			$(ERPC_C_ROOT)/infra/port/log 
INCLUDE := $(foreach includes, $(INCLUDES), -I $(includes))

CXX=g++
CXXFLAGS += -g $(INCLUDE) -lpthread

ERPC=idl.erpc
ERPC_SERVER=erpc_matrix_multiply_server.cpp 
ERPC_CLIENT=erpc_matrix_multiply_client.cpp
ERPC_HEADER=erpc_matrix_multiply_server.hpp \
			erpc_matrix_multiply.h
			
.PHONY:all
all: client server

server: ${ERPC_SERVER}  server.cpp ${ERPC_LIB}
	@${CXX} ${CXXFLAGS} $^ -o server
	@$(call printmessage,build,Building, server)

client: ${ERPC_CLIENT}  client.cpp ${ERPC_LIB}
	@${CXX} ${CXXFLAGS} $^ -o client
	@$(call printmessage,build,Building, client)

${ERPC_SERVER} ${ERPC_CLIENT} ${ERPC_HEADER}:
	erpcgen ${ERPC} -g c

.PHONY:clean
clean:
	rm client server ${ERPC_CLIENT} ${ERPC_SERVER} ${ERPC_HEADER}
