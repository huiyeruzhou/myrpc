file(GLOB_RECURSE RPC_SRC
        infra/*.cpp
        infra/nanopb/pb_*.c
        )
set(RPC_INCLUDE include/ include/nanopb)

idf_component_register(SRCS "${RPC_SRC}"
                       INCLUDE_DIRS "${RPC_INCLUDE}"
                       REQUIRES lwip)
