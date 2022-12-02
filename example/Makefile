CXX=g++
CXXFLAGS+=-g -I../erpc_c/infra/ -I../erpc_c/setup/ -I../erpc_c/transports/ -I../erpc_c/config/ -I../erpc_c/port/ -L${ERPC_LIB_DIR} -lerpc -lpthread
# CXXFLAGS += -g -I. -L${ERPC_LIB_DIR} -lerpc -lpthread
ERPC_LIB_DIR=-L/home/huiyeruzhou/Documents/WorkSpace/embeeded-rpc/embeeded-rpc/Debug/Linux/erpc/lib
ERPC=idl.erpc
ERPC_SERVER=erpc_matrix_multiply_server.cpp 
ERPC_CLIENT=erpc_matrix_multiply_client.cpp
ERPC_HEADER=erpc_matrix_multiply_server.hpp
.PHONY:all
all: client server

server: ${ERPC_SERVER} ${ERPC_HEADER}
	${CXX}  server.cpp erpc_matrix_multiply_server.cpp ${CXXFLAGS} -o server

client: ${ERPC_CLIENT} ${ERPC_HEADER}
	${CXX}  client.cpp erpc_matrix_multiply_client.cpp ${CXXFLAGS} -o client

${ERPC_SERVER} ${ERPC_CLIENT} ${ERPC_HEADER}:
	erpcgen ${ERPC} -g c

.PHONY:clean
clean:
	rm client server ${ERPC_CLIENT} ${ERPC_SERVER}