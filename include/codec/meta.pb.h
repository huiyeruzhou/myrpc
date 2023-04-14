/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.8-dev */

#ifndef PB_MYRPC_META_PB_H_INCLUDED
#define PB_MYRPC_META_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _myrpc_Meta {
    /* enum msgType{
     REQUEST = 0;
     RESPONSE = 1;
     ONEWAY = 2;
 } */
    uint32_t version;
    char *;
    uint32_t seq;
    uint64_t timeout;
} myrpc_Meta;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define myrpc_Meta_init_default                  {0, NULL, 0, 0}
#define myrpc_Meta_init_zero                     {0, NULL, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define myrpc_Meta_version_tag                   1
#define myrpc_Meta_path_tag                      2
#define myrpc_Meta_seq_tag                       3
#define myrpc_Meta_timeout_tag                   4

/* Struct field encoding specification for nanopb */
#define myrpc_Meta_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, UINT32,   version,           1) \
X(a, POINTER,  REQUIRED, STRING,   path,              2) \
X(a, STATIC,   REQUIRED, UINT32,   seq,               3) \
X(a, STATIC,   REQUIRED, UINT64,   timeout,           4)
#define myrpc_Meta_CALLBACK NULL
#define myrpc_Meta_DEFAULT NULL

extern const pb_msgdesc_t myrpc_Meta_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define myrpc_Meta_fields &myrpc_Meta_msg

/* Maximum encoded size of messages (where known) */
/* myrpc_Meta_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
