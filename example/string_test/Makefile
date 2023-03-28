ERPC_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../../)
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
ERPCGEN=$(ERPC_ROOT)/Debug/Linux/erpcgen/erpcgen
CXX=g++
CXXFLAGS += -g $(INCLUDE) -lpthread
PROGRAM=qaq
ERPC=qaq.erpc
ERPC_SERVER=${PROGRAM}_server.cpp 
ERPC_CLIENT=${PROGRAM}_client.cpp
ERPC_HEADER=${PROGRAM}_server.h \
			${PROGRAM}.h
			
.PHONY:all
all: client server

server: ${ERPC_SERVER}  server.cpp ${ERPC_LIB}
	${CXX} ${CXXFLAGS} $^ -o server


client: ${ERPC_CLIENT}  client.cpp ${ERPC_LIB}
	${CXX} ${CXXFLAGS} $^ -o client


${ERPC_SERVER} ${ERPC_CLIENT} ${ERPC_HEADER}:
	${ERPCGEN} ${ERPC} -g c

.PHONY:clean
clean:
	rm client server ${ERPC_CLIENT} ${ERPC_SERVER} ${ERPC_HEADER}