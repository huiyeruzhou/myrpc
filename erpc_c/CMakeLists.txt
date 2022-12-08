file(GLOB_RECURSE RPC_SRC
        *.cpp
        )
set(RPC_INCLUDE INCLUDES 
            infra 
			infra/codec 
			infra/client 
			infra/server 
			infra/transport 
			setup)
idf_component_register(SRCS "${RPC_SRC}"
                       INCLUDE_DIRS "${INCLUDES}"
                       )
