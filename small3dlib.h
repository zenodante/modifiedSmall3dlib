#ifndef SMALL3DLIB_H
#define SMALL3DLIB_H

#ifdef __cplusplus
extern "C" {
#endif
/*
  Simple realtime 3D software rasterization renderer. It is fast, focused on
  resource-limited computers, located in a single C header file, with no
  dependencies, using only 32bit integer arithmetics.

  author: Miloslav Ciz
  license: CC0 1.0 (public domain)
           found at https://creativecommons.org/publicdomain/zero/1.0/
           + additional waiver of all IP
  version: 0.853d

  Before including the library, define S3L_PIXEL_FUNCTION to the name of the
  function you'll be using to draw single pixels (this function will be called
  by the library to render the frames). Also either init S3L_resolutionX and
  S3L_resolutionY or define S3L_RESOLUTION_X and S3L_RESOLUTION_Y.

  You'll also need to decide what rendering strategy and other settings you
  want to use, depending on your specific usecase. You may want to use a
  z-buffer (full or reduced, S3L_Z_BUFFER), sorted-drawing (S3L_SORT), or even
  none of these. See the description of the options in this file.

  The rendering itself is done with S3L_drawScene, usually preceded by
  S3L_newFrame (for clearing zBuffer etc.).

  The library is meant to be used in not so huge programs that use single
  translation unit and so includes both declarations and implementation at once.
  If you for some reason use multiple translation units (which include the
  library), you'll have to handle this yourself (e.g. create a wrapper, manually
  split the library into .c and .h etc.).

  --------------------

  This work's goal is to never be encumbered by any exclusive intellectual
  property rights. The work is therefore provided under CC0 1.0 + additional
  WAIVER OF ALL INTELLECTUAL PROPERTY RIGHTS that waives the rest of
  intellectual property rights not already waived by CC0 1.0. The WAIVER OF ALL
  INTELLECTUAL PROPERTY RGHTS is as follows:

  Each contributor to this work agrees that they waive any exclusive rights,
  including but not limited to copyright, patents, trademark, trade dress,
  industrial design, plant varieties and trade secrets, to any and all ideas,
  concepts, processes, discoveries, improvements and inventions conceived,
  discovered, made, designed, researched or developed by the contributor either
  solely or jointly with others, which relate to this work or result from this
  work. Should any waiver of such right be judged legally invalid or
  ineffective under applicable law, the contributor hereby grants to each
  affected person a royalty-free, non transferable, non sublicensable, non
  exclusive, irrevocable and unconditional license to this right.

  --------------------

  CONVENTIONS:

  This library should never draw pixels outside the specified screen
  boundaries, so you don't have to check this (that would cost CPU time)!

  You can safely assume that triangles are rasterized one by one and from top
  down, left to right (so you can utilize e.g. various caches), and if sorting
  is disabled the order of rasterization will be that specified in the scene
  structure and model arrays (of course, some triangles and models may be
  skipped due to culling etc.).

  Angles are in S3L_Units, a full angle (2 pi) is S3L_FRACTIONS_PER_UNITs.

  We use row vectors.

  In 3D space, a left-handed coord. system is used. One spatial unit is split
  into S3L_FRACTIONS_PER_UNIT fractions (fixed point arithmetic).

     y ^
       |   _ 
       |   /| z
       |  /
       | /
  [0,0,0]-------> x

  Untransformed camera is placed at [0,0,0], looking forward along +z axis. The
  projection plane is centered at [0,0,0], stretrinch from
  -S3L_FRACTIONS_PER_UNIT to S3L_FRACTIONS_PER_UNIT horizontally (x),
  vertical size (y) depends on the aspect ratio (S3L_RESOLUTION_X and
  S3L_RESOLUTION_Y). Camera FOV is defined by focal length in S3L_Units.

           y ^
             |  _
             |  /| z
         ____|_/__
        |    |/   |
     -----[0,0,0]-|-----> x
        |____|____|
             |    
             |

  Rotations use Euler angles and are generally in the extrinsic Euler angles in
  ZXY order (by Z, then by X, then by Y). Positive rotation about an axis
  rotates CW (clock-wise) when looking in the direction of the axis.

  Coordinates of pixels on the screen start at the top left, from [0,0].

  There is NO subpixel accuracy (screen coordinates are only integer).

  Triangle rasterization rules are these (mostly same as OpenGL, D3D etc.):

  - Let's define:
    - left side:
      - not exactly horizontal, and on the left side of triangle
      - exactly horizontal and above the topmost
      (in other words: its normal points at least a little to the left or
       completely up)
    - right side: not left side
  - Pixel centers are at integer coordinates and triangle for drawing are
    specified with integer coordinates of pixel centers.
  - A pixel is rasterized:
    - if its center is inside the triangle OR
    - if its center is exactly on the triangle side which is left and at the
      same time is not on the side that's right (case of a triangle that's on
      a single line) OR
    - if its center is exactly on the triangle corner of sides neither of which
      is right.

  These rules imply among others:

  - Adjacent triangles don't have any overlapping pixels, nor gaps between.
  - Triangles of points that lie on a single line are NOT rasterized.
  - A single "long" triangle CAN be rasterized as isolated islands of pixels.
  - Transforming (e.g. mirroring, rotating by 90 degrees etc.) a result of
    rasterizing triangle A is NOT generally equal to applying the same
    transformation to triangle A first and then rasterizing it. Even the number
    of rasterized pixels is usually different.
  - If specifying a triangle with integer coordinates (which we are), then:
    - The bottom-most corner (or side) of a triangle is never rasterized
      (because it is connected to a right side).
    - The top-most corner can only be rasterized on completely horizontal side
      (otherwise it is connected to a right side).
    - Vertically middle corner is rasterized if and only if it is on the left
      of the triangle and at the same time is also not the bottom-most corner.
*/



#include "S3L_types.h"
#include "S3L_config.h"
#include "S3L_port.h"
extern void S3L_PIXEL_FUNCTION(S3L_PixelInfo *pixel); // forward decl
extern S3L_Unit S3L_abs(S3L_Unit value);
extern S3L_Unit S3L_min(S3L_Unit v1, S3L_Unit v2);
extern S3L_Unit S3L_max(S3L_Unit v1, S3L_Unit v2);
extern S3L_Unit S3L_clamp(S3L_Unit v, S3L_Unit v1, S3L_Unit v2);
extern S3L_Unit S3L_zeroClamp(S3L_Unit value);
extern S3L_Unit S3L_wrap(S3L_Unit value, S3L_Unit mod);
extern S3L_Unit S3L_nonZero(S3L_Unit value);

/** Interpolated between two values, v1 and v2, in the same ratio as t is to
  tMax. Does NOT prevent zero division. */
extern S3L_Unit S3L_interpolate(
  S3L_Unit v1,
  S3L_Unit v2,
  S3L_Unit t,
  S3L_Unit tMax);
extern S3L_Unit S3L_sin(S3L_Unit x);
extern S3L_Unit S3L_asin(S3L_Unit x);
extern S3L_Unit S3L_cos(S3L_Unit x);
extern S3L_Unit S3L_sqrt(S3L_Unit value);
extern void S3L_initVec4(S3L_Vec4 *v);
extern void S3L_setVec4(S3L_Vec4 *v, S3L_Unit x, S3L_Unit y,S3L_Unit z, S3L_Unit w);
extern void S3L_vec3Add(S3L_Vec4 *result, S3L_Vec4 added);
extern void S3L_vec3Sub(S3L_Vec4 *result, S3L_Vec4 substracted);
extern S3L_Unit S3L_vec2Length(S3L_Vec4 v);
extern S3L_Unit S3L_vec3Length(S3L_Vec4 v);
extern void S3L_normalizeVec3Fast(S3L_Vec4 *v);
extern void S3L_normalizeVec3(S3L_Vec4 *v);
#define S3L_logVec4(v)\
  printf("Vec4: %d %d %d %d\n",((v).x),((v).y),((v).z),((v).w))
/** Initializes a 4x4 matrix to identity. */
extern void S3L_initMat4(S3L_Mat4 m); //matrix update
extern void S3L_copyMat4(S3L_Mat4 src, S3L_Mat4 dst); //matrix update
#define S3L_logMat4(m)\
  printf("Mat4:\n  %d %d %d %d\n  %d %d %d %d\n  %d %d %d %d\n  %d %d %d %d\n"\
   ,(m)[0][0],(m)[1][0],(m)[2][0],(m)[3][0],\
    (m)[0][1],(m)[1][1],(m)[2][1],(m)[3][1],\
    (m)[0][2],(m)[1][2],(m)[2][2],(m)[3][2],\
    (m)[0][3],(m)[1][3],(m)[2][3],(m)[3][3])
/** Like S3L_normalizeVec3, but doesn't perform any checks on the input vector,
  which is faster, but can be very innacurate or overflowing. You are supposed
  to provide a "nice" vector (not too big or small). */
extern S3L_Unit S3L_dotProductVec3(S3L_Vec4 a, S3L_Vec4 b);
/** Normalizes Vec3. Note that this function tries to normalize correctly
  rather than quickly! If you need to normalize quickly, do it yourself in a
  way that best fits your case. */
extern void S3L_crossProduct(S3L_Vec4 a, S3L_Vec4 b, S3L_Vec4 *result);
/** Computes a reflection direction (typically used e.g. for specular component
  in Phong illumination). The input vectors must be normalized. The result will
  be normalized as well. */
/** Multiplies a vector by a matrix with normalization by
  S3L_FRACTIONS_PER_UNIT. Result is stored in the input vector. */
extern void S3L_vec4Xmat4(S3L_Vec4 *v, S3L_Mat4 m);//matrix update
/** Same as S3L_vec4Xmat4 but faster, because this version doesn't compute the
  W component of the result, which is usually not needed. */
extern void S3L_vec3Xmat4(S3L_Vec4 *v, S3L_Mat4 m);//matrix update
/** Multiplies two matrices with normalization by S3L_FRACTIONS_PER_UNIT.
  Result is stored in the first matrix. The result represents a transformation
  that has the same effect as applying the transformation represented by m1 and
  then m2 (in that order). */
extern void S3L_mat4Xmat4(S3L_Mat4 m1, S3L_Mat4 m2);//matrix update
extern void S3L_transposeMat4(S3L_Mat4 m);//matrix update
extern void S3L_rotate2DPoint(S3L_Unit *x, S3L_Unit *y, S3L_Unit angle);
extern void S3L_reflect(S3L_Vec4 toLight, S3L_Vec4 normal, S3L_Vec4 *result);
#define S3L_logTransform3D(t)\
  printf("Transform3D: T = [%d %d %d], R = [%d %d %d], S = [%d %d %d]\n",\
    (t).translation.x,(t).translation.y,(t).translation.z,\
    (t).rotation.x,(t).rotation.y,(t).rotation.z,\
    (t).scale.x,(t).scale.y,(t).scale.z)
extern void S3L_lookAt(S3L_Vec4 pointTo, S3L_Transform3D *t);
/** Corrects barycentric coordinates so that they exactly meet the defined
  conditions (each fall into <0,S3L_FRACTIONS_PER_UNIT>, sum =
  S3L_FRACTIONS_PER_UNIT). Note that doing this per-pixel can slow the program
  down significantly. */
extern void S3L_correctBarycentricCoords(S3L_Unit barycentric[3]);
/** Returns a value interpolated between the three triangle vertices based on
  barycentric coordinates. */
extern S3L_Unit S3L_interpolateBarycentric(
  S3L_Unit value0,
  S3L_Unit value1,
  S3L_Unit value2,
  S3L_Unit barycentric[3]);



/** Writes a value (not necessarily depth! depends on the format of z-buffer)
  to z-buffer (if enabled). Does NOT check boundaries! */
extern void S3L_zBufferWrite(S3L_ScreenCoord x, S3L_ScreenCoord y, S3L_Unit value);
/** Reads a value (not necessarily depth! depends on the format of z-buffer)
  from z-buffer (if enabled). Does NOT check boundaries! */
extern S3L_Unit S3L_zBufferRead(S3L_ScreenCoord x, S3L_ScreenCoord y);
extern void S3L_zBufferClear(void);

extern void S3L_initTransform3D(S3L_Transform3D *t);
extern void S3L_setTransform3D(
  S3L_Unit tx,
  S3L_Unit ty,
  S3L_Unit tz,
  S3L_Unit rx,
  S3L_Unit ry,
  S3L_Unit rz,
  S3L_Unit sx,
  S3L_Unit sy,
  S3L_Unit sz,
  S3L_Transform3D *t);
/** Converts rotation transformation to three direction vectors of given length
  (any one can be NULL, in which case it won't be computed). */
extern void S3L_rotationToDirections(
  S3L_Vec4 rotation,
  S3L_Unit length,
  S3L_Vec4 *forw, 
  S3L_Vec4 *right,
  S3L_Vec4 *up);



extern void S3L_makeTranslationMat(
  S3L_Unit offsetX,
  S3L_Unit offsetY,
  S3L_Unit offsetZ,
  S3L_Mat4 m);//matrix update
/** Makes a scaling matrix. DON'T FORGET: scale of 1.0 is set with
  S3L_FRACTIONS_PER_UNIT! */
extern void S3L_makeScaleMatrix(
  S3L_Unit scaleX,
  S3L_Unit scaleY,
  S3L_Unit scaleZ,
  S3L_Mat4 m);//matrix update
/** Makes a matrix for rotation in the ZXY order. */
extern void S3L_makeRotationMatrixZXY(
  S3L_Unit byX,
  S3L_Unit byY,
  S3L_Unit byZ,
  S3L_Mat4 m);//matrix update
extern void S3L_makeWorldMatrix(S3L_Transform3D worldTransform, S3L_Mat4 m);//matrix update
extern void S3L_makeCameraMatrix(S3L_Transform3D cameraTransform, S3L_Mat4 m);//matrix update
extern void S3L_initCamera(S3L_Camera *camera);
extern void S3L_initDrawConfig(S3L_DrawConfig *config);
extern void S3L_initModel3D(
  const S3L_Unit *vertices,
  S3L_Index vertexCount,
  const S3L_Index *triangles,
  S3L_Index triangleCount,
  S3L_Model3D *model);
extern void S3L_initScene(
  S3L_Model3D *models,
  S3L_Index modelCount,
  S3L_Scene *scene);

/** Projects a single point from 3D space to the screen space (pixels), which
  can be useful e.g. for drawing sprites. The w component of input and result
  holds the point size. If this size is 0 in the result, the sprite is outside
  the view. */
extern void project3DPointToScreen(
  S3L_Vec4 point,
  S3L_Camera camera,
  S3L_Vec4 *result);
/** Computes a normalized normal of given triangle. */
extern void S3L_triangleNormal(S3L_Vec4 t0, S3L_Vec4 t1, S3L_Vec4 t2,
  S3L_Vec4 *n);
/** Helper function for retrieving per-vertex indexed values from an array,
  e.g. texturing (UV) coordinates. The 'indices' array contains three indices
  for each triangle, each index pointing into 'values' array, which contains
  the values, each one consisting of 'numComponents' components (e.g. 2 for
  UV coordinates). The three values are retrieved into 'v0', 'v1' and 'v2'
  vectors (into x, y, z and w, depending on 'numComponents'). This function is
  meant to be used per-triangle (typically from a cache), NOT per-pixel, as it
  is not as fast as possible! */
extern void S3L_getIndexedTriangleValues(
  S3L_Index triangleIndex,
  const S3L_Index *indices,
  const S3L_Unit *values,
  uint8_t numComponents,
  S3L_Vec4 *v0,
  S3L_Vec4 *v1,
  S3L_Vec4 *v2);
/** Computes a normalized normal for every vertex of given model (this is
  relatively slow and SHOUDN'T be done each frame). The dst array must have a
  sufficient size preallocated! The size is: number of model vertices * 3 *
  sizeof(S3L_Unit). Note that for advanced allowing sharp edges it is not
  sufficient to have per-vertex normals, but must be per-triangle. This
  function doesn't support this. 

  The function computes a normal for each vertex by averaging normals of
  the triangles containing the vertex. The maximum number of these triangle
  normals that will be averaged is set with
  S3L_NORMAL_COMPUTE_MAXIMUM_AVERAGE. */
extern void S3L_computeModelNormals(S3L_Model3D model, S3L_Unit *dst,
  int8_t transformNormals);
/** Draws a triangle according to given config. The vertices are specified in
  Screen Space space (pixels). If perspective correction is enabled, each
  vertex has to have a depth (Z position in camera space) specified in the Z
  component. */
extern void S3L_drawTriangle(
  S3L_Vec4 point0,
  S3L_Vec4 point1,
  S3L_Vec4 point2,
  S3L_Index modelIndex,
  S3L_Index triangleIndex);
/** This should be called before rendering each frame. The function clears
  buffers and does potentially other things needed for the frame. */
extern void S3L_newFrame(void);

extern void S3L_stencilBufferClear(void);
extern void S3L_drawScene(S3L_Scene scene);
/** Predefined vertices of a cube to simply insert in an array. These come with
    S3L_CUBE_TRIANGLES and S3L_CUBE_TEXCOORDS. */
#define S3L_CUBE_VERTICES(m)\
 /* 0 front, bottom, right */\
 m/2, -m/2, -m/2,\
 /* 1 front, bottom, left */\
-m/2, -m/2, -m/2,\
 /* 2 front, top,    right */\
 m/2,  m/2, -m/2,\
 /* 3 front, top,    left */\
-m/2,  m/2, -m/2,\
 /* 4 back,  bottom, right */\
 m/2, -m/2,  m/2,\
 /* 5 back,  bottom, left */\
-m/2, -m/2,  m/2,\
 /* 6 back,  top,    right */\
 m/2,  m/2,  m/2,\
 /* 7 back,  top,    left */\
-m/2,  m/2,  m/2

#define S3L_CUBE_VERTEX_COUNT 8

/** Predefined triangle indices of a cube, to be used with S3L_CUBE_VERTICES
    and S3L_CUBE_TEXCOORDS. */
#define S3L_CUBE_TRIANGLES\
  3, 0, 2, /* front  */\
  1, 0, 3,\
  0, 4, 2, /* right  */\
  2, 4, 6,\
  4, 5, 6, /* back   */\
  7, 6, 5,\
  3, 7, 1, /* left   */\
  1, 7, 5,\
  6, 3, 2, /* top    */\
  7, 3, 6,\
  1, 4, 0, /* bottom */\
  5, 4, 1

#define S3L_CUBE_TRIANGLE_COUNT 12

/** Predefined texture coordinates of a cube, corresponding to triangles (NOT
    vertices), to be used with S3L_CUBE_VERTICES and S3L_CUBE_TRIANGLES. */
#define S3L_CUBE_TEXCOORDS(m)\
  0,0,  m,m,  m,0,\
  0,m,  m,m,  0,0,\
  m,m,  m,0,  0,m,\
  0,m,  m,0,  0,0,\
  m,0,  0,0,  m,m,\
  0,m,  m,m,  0,0,\
  0,0,  0,m,  m,0,\
  m,0,  0,m,  m,m,\
  0,0,  m,m,  m,0,\
  0,m,  m,m,  0,0,\
  m,0,  0,m,  m,m,\
  0,0,  0,m,  m,0

#ifdef __cplusplus
}
#endif

#endif // guard
