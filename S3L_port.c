
#include "S3L_types.h"
#include "texture_model.h"
#include "small3dlib.h"
inline uint16_t sampleTexture(int32_t u, int32_t v);
static uint16_t *pFBuff;
static uint32_t previousTriangle = -1;
static S3L_Vec4 uv0, uv1, uv2;

void S3L_SetFBuffAddr(uint16_t *buff){
    pFBuff = buff;
}
void S3L_pixel_function(S3L_PixelInfo *pixel){
 if (pixel->triangleID != previousTriangle)
  {
    const S3L_Index *uvIndices;
    const S3L_Unit *uvs;

    if (pixel->modelIndex == 0)
    {
      uvIndices = cityUVIndices;
      uvs = cityUVs;
    }
    else
    {
      uvIndices = carUVIndices;
      uvs = carUVs;
    }

    S3L_getIndexedTriangleValues(pixel->triangleIndex,uvIndices,uvs,2,&uv0,&uv1,&uv2);

    previousTriangle = pixel->triangleID;
  }

  S3L_Unit uv[2];

  uv[0] = S3L_interpolateBarycentric(uv0.x,uv1.x,uv2.x,pixel->barycentric);
  uv[1] = S3L_interpolateBarycentric(uv0.y,uv1.y,uv2.y,pixel->barycentric);
  uint16_t *buf=pFBuff;

  buf += pixel->y * 120;
  buf += pixel->x;
  *buf = sampleTexture(uv[0] >> 2,uv[1] >> 2);

}

inline uint16_t sampleTexture(int32_t u, int32_t v)
{
  uint32_t index = v * CITY_TEXTURE_WIDTH + u;

  return cityTexture[index];
}
