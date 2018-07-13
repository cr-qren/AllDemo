/**
 * Copyright (c) 2016 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 * 502 Section b, 2305 Zuchongzhi Road, Zhangjiang Hi-Tech Park,
 * Shanghai 201203, China.
 ********************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

extern int   serialOpen      (const char *device, const int baud) ;
extern void  serialClose     (const int fd) ;
extern void  serialFlush     (const int fd) ;
extern void  serialPutchar   (const int fd, const unsigned char c) ;
extern void  serialPuts      (const int fd, const char *s) ;
extern void  serialPrintf    (const int fd, const char *message, ...) ;
extern int   serialDataAvail (const int fd) ;
extern int   serialGetchar   (const int fd) ;
extern void  serialPutString (const int fd, const char *str, const int size);
extern void  serialGetString (const int fd, char *str, const int size);

#ifdef __cplusplus
}
#endif
