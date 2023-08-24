/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: const.h
 * Description: This file defines the const value used in rpc.
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#ifndef PORT_CONST_H_
#define PORT_CONST_H_

#ifndef ERPC_DEFAULT_BUFFER_SIZE
#define ERPC_DEFAULT_BUFFER_SIZE (256U)
#endif

#ifndef CONFIG_MAX_PTHREAD_NAME_LEN
#define CONFIG_MAX_PTHREAD_NAME_LEN (16U)
#endif

#endif  // PORT_CONST_H_