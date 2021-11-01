#ifndef S3L_TYPES_H_
#define S3L_TYPES_H_


#ifdef __cplusplus
extern "C" {
#endif
 #include <stdint.h> 
typedef int16_t S3L_ScreenCoord;
typedef uint16_t S3L_Index;


typedef struct
{
  S3L_Unit x;
  S3L_Unit y;
  S3L_Unit z;
  S3L_Unit w;
} S3L_Vec4;



typedef struct
{
  S3L_Vec4 translation;
  S3L_Vec4 rotation; /**< Euler angles. Rortation is applied in this order:
                          1. z = by z (roll) CW looking along z+
                          2. x = by x (pitch) CW looking along x+
                          3. y = by y (yaw) CW looking along y+ */
  S3L_Vec4 scale;
} S3L_Transform3D;

typedef S3L_Unit S3L_Mat4[4][4]; 

typedef struct
{
  S3L_Unit focalLength;       ///< Defines the field of view (FOV).
  S3L_Transform3D transform;
} S3L_Camera;

typedef struct
{
  uint8_t backfaceCulling;    /**< What backface culling to use. Possible
                                   values:
                                   - 0 none
                                   - 1 clock-wise
                                   - 2 counter clock-wise */
  int8_t visible;             /**< Can be used to easily hide the model. */
} S3L_DrawConfig;

typedef struct
{
  const S3L_Unit *vertices;
  S3L_Index vertexCount;
  const S3L_Index *triangles;
  S3L_Index triangleCount;
  S3L_Transform3D transform;
  S3L_Mat4 *customTransformMatrix; /**< This can be used to override the
                                     transform (if != 0) with a custom
                                     transform matrix, which is more
                                     general. */
  S3L_DrawConfig config;
} S3L_Model3D;                ///< Represents a 3D model.


typedef struct
{
  S3L_Model3D *models;
  S3L_Index modelCount;
  S3L_Camera camera;
} S3L_Scene;                  ///< Represent the 3D scene to be rendered.

typedef struct
{
  S3L_ScreenCoord x;          ///< Screen X coordinate.
  S3L_ScreenCoord y;          ///< Screen Y coordinate.

  S3L_Unit barycentric[3]; /**< Barycentric coords correspond to the three
                              vertices. These serve to locate the pixel on a
                              triangle and interpolate values between it's
                              three points. Each one goes from 0 to
                              S3L_FRACTIONS_PER_UNIT (including), but due to
                              rounding error may fall outside this range (you
                              can use S3L_correctBarycentricCoords to fix this
                              for the price of some performance). The sum of
                              the three coordinates will always be exactly
                              S3L_FRACTIONS_PER_UNIT. */
  S3L_Index modelIndex;    ///< Model index within the scene.
  S3L_Index triangleIndex; ///< Triangle index within the model.
  uint32_t triangleID;     /**< Unique ID of the triangle withing the whole
                               scene. This can be used e.g. by a cache to
                               quickly find out if a triangle has changed. */
  S3L_Unit depth;         ///< Depth (only if depth is turned on).
  S3L_Unit previousZ;     /**< Z-buffer value (not necessarily world depth in
                               S3L_Units!) that was in the z-buffer on the
                               pixels position before this pixel was
                               rasterized. This can be used to set the value
                               back, e.g. for transparency. */
  S3L_ScreenCoord triangleSize[2]; /**< Rasterized triangle width and height,
                              can be used e.g. for MIP mapping. */
} S3L_PixelInfo;         /**< Used to pass the info about a rasterized pixel
                              (fragment) to the user-defined drawing func. */



#ifdef __cplusplus
} //end extern "C"
#endif

#endif
