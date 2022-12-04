CXX=g++
CXXFLAGS+=-g -I../erpc_c/infra/ -I../erpc_c/setup/ -I../erpc_c/transports/ -I../erpc_c/config/ -I../erpc_c/port/ -lpthread
# CXXFLAGS += -g -I. -L${ERPC_LIB_DIR} -lerpc -lpthread
ERPC_LIB=/home/huiyeruzhou/Documents/WorkSpace/embeeded-rpc/myrpc/Debug/Linux/erpc/lib/liberpc.a
ERPC=idl.erpc
ERPC_SERVER=erpc_matrix_multiply_server.cpp 
ERPC_CLIENT=erpc_matrix_multiply_client.cpp
ERPC_HEADER=erpc_matrix_multiply_server.hpp
.PHONY:all
all: client server

server: ${ERPC_SERVER} ${ERPC_HEADER}
	${CXX}  server.cpp erpc_matrix_multiply_server.cpp  ${ERPC_LIB} ${CXXFLAGS} -o server

client: ${ERPC_CLIENT} ${ERPC_HEADER}
	${CXX}  client.cpp erpc_matrix_multiply_client.cpp  ${ERPC_LIB} ${CXXFLAGS} -o client

${ERPC_SERVER} ${ERPC_CLIENT} ${ERPC_HEADER}:
	erpcgen ${ERPC} -g c

.PHONY:clean
clean:
	rm client server ${ERPC_CLIENT} ${ERPC_SERVER}