#ifndef _TEXTURE_MODEL_H__
#define _TEXTURE_MODEL_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "S3L_types.h"
#define CITY_TEXTURE_WIDTH 128
#define CITY_TEXTURE_HEIGHT 128
#define CITY_VERTEX_COUNT 155

#define CITY_TRIANGLE_COUNT 197
#define CITY_UV_COUNT 377
#define CITY_UV_INDEX_COUNT 197
#define CAR_VERTEX_COUNT 12
#define CAR_TRIANGLE_COUNT 18
#define CAR_UV_COUNT 24

#define CAR_UV_INDEX_COUNT 18
extern const uint16_t cityPalette[256];
extern const uint8_t cityTexture[16384];
extern const S3L_Unit cityVertices[CITY_VERTEX_COUNT * 3] ;
extern const S3L_Index cityTriangleIndices[CITY_TRIANGLE_COUNT * 3] ;
extern const S3L_Unit cityUVs[CITY_UV_COUNT * 2];
extern const S3L_Index cityUVIndices[CITY_UV_INDEX_COUNT * 3] ;
extern const S3L_Unit carVertices[CAR_VERTEX_COUNT * 3];
extern const S3L_Index carTriangleIndices[CAR_TRIANGLE_COUNT * 3] ;
extern const S3L_Unit carUVs[CAR_UV_COUNT * 2] ;
extern const S3L_Index carUVIndices[CAR_UV_INDEX_COUNT * 3] ;
extern S3L_Model3D cityModel;
extern S3L_Model3D carModel;

extern void cityModelInit(void);
extern void carModelInit(void);
#ifdef __cplusplus
}
#endif
#endif
