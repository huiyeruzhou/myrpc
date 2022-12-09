file(GLOB_RECURSE RPC_SRC
        *.cpp
        )
set(RPC_INCLUDE infra/
                infra/codec 
                infra/client 
                infra/server 
                infra/transport 
                port)
idf_component_register(SRCS "${RPC_SRC}"
                       INCLUDE_DIRS "${RPC_INCLUDE}"
                       REQUIRES lwip)
