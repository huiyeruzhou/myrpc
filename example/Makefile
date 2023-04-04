include ../mk/common.mk
ERPC_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../)
ERPC_C_ROOT = $(ERPC_ROOT)
INCLUDES += $(ERPC_C_ROOT)/include
INCLUDE := $(foreach includes, $(INCLUDES), -I $(includes))

CXX=g++
CXXFLAGS += -g $(INCLUDE) -lpthread
PROGRAM=erpc_matrix_multiply
ERPC=idl.erpc
ERPC_LIB=$(ERPC_ROOT)/Debug/Linux/erpc/lib/liberpc.a
ERPCGEN=$(ERPC_ROOT)/Debug/Linux/erpcgen/erpcgen
ERPC_SERVER=${PROGRAM}_server.cpp 
ERPC_CLIENT=${PROGRAM}_client.cpp
ERPC_HEADER=${PROGRAM}_server.hpp \
			${PROGRAM}.h
			
.PHONY:all
all: client server

server: ${ERPC_SERVER}  server.cpp ${ERPC_LIB}
	@${CXX} ${CXXFLAGS} $^ -o server
	@$(call printmessage,build,Building, server)

client: ${ERPC_CLIENT}  client.cpp ${ERPC_LIB}
	@${CXX} ${CXXFLAGS} $^ -o client
	@$(call printmessage,build,Building, client)

${ERPC_SERVER} ${ERPC_CLIENT} ${ERPC_HEADER}:
	${ERPCGEN} ${ERPC} -g c

.PHONY:clean
clean:
	rm client server ${ERPC_CLIENT} ${ERPC_SERVER} ${ERPC_HEADER}
