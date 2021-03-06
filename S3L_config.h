#ifndef S3L_CONFIG_H_
#define S3L_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#define S3L_RESOLUTION_X       120
#define S3L_RESOLUTION_Y       120


/** How many fractions a spatial unit is split into. This is NOT SUPPOSED TO
BE REDEFINED, so rather don't do it (otherwise things may overflow etc.). */

#define S3L_FRACTIONS_PER_UNIT 512

#define S3L_NEAR_CROSS_STRATEGY 2

#define S3L_FLAT 0  
#define S3L_PERSPECTIVE_CORRECTION 0 
#define S3L_PC_APPROX_LENGTH 32

#define S3L_COMPUTE_DEPTH 0

#define S3L_Z_BUFFER 1
#define S3L_REDUCED_Z_BUFFER_GRANULARITY 5

#define S3L_STENCIL_BUFFER 0
#define S3L_SORT 0

#define S3L_MAX_TRIANGES_DRAWN 128

#define S3L_NORMAL_COMPUTE_MAXIMUM_AVERAGE 6
#define S3L_FAST_LERP_QUALITY 11 


  
#ifdef __cplusplus
}
#endif

#endif
