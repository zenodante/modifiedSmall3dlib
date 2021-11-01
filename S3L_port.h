#ifndef __S3L_PORT_H__
#define __S3L_PORT_H__


#ifdef __cplusplus
extern "C" {
#endif
#include "S3L_types.h"


#define S3L_PIXEL_FUNCTION S3L_pixel_function

extern void S3L_pixel_function(S3L_PixelInfo *pixel); 
extern void S3L_SetFBuffAddr(uint16_t *buff);

  
#ifdef __cplusplus
}
#endif

#endif
