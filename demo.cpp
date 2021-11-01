#include <math.h>
#include <array>
#include <algorithm>

#include "picosystem.hpp"
#include "small3dlib.h"
#include "texture_model.h"

#define MAX_VELOCITY 700
#define ACCELERATION 600
#define TURN_SPEED 200
#define FRICTION 600

using namespace picosystem;

S3L_Model3D models[2];
const uint8_t collisionMap[8 * 10] =
{
  1,1,1,1,1,1,1,1,
  1,1,1,1,0,0,0,1,
  1,1,1,1,0,1,0,1,
  2,2,1,0,0,0,0,3,
  1,2,1,0,1,1,3,1,
  2,0,0,0,1,1,3,3,
  1,0,1,0,0,1,1,1,
  1,0,0,0,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1
};
S3L_Scene scene;

int16_t velocity = 7;

uint32_t previousTime = 0;

S3L_Vec4 carDirection;
uint8_t collision(S3L_Vec4 worldPosition);
void handleCollision(S3L_Vec4 *pos, S3L_Vec4 previousPos);
uint8_t collision(S3L_Vec4 worldPosition)
{
  worldPosition.x /= S3L_FRACTIONS_PER_UNIT;
  worldPosition.z /= -S3L_FRACTIONS_PER_UNIT;    

  uint16_t index = worldPosition.z * 8 + worldPosition.x;

  return collisionMap[index];
}

void handleCollision(S3L_Vec4 *pos, S3L_Vec4 previousPos)
{
  S3L_Vec4 newPos = *pos;
  newPos.x = previousPos.x;
    
  if (collision(newPos))
  {
    newPos = *pos;
    newPos.z = previousPos.z;

    if (collision(newPos))
      newPos = previousPos;
  }

  *pos = newPos;
}
// initialise the world
void init() {
  S3L_SetFBuffAddr(SCREEN->data);
  cityModelInit();
  carModelInit();
  models[0] = cityModel;
  models[1] = carModel;
  S3L_initScene(models,2,&scene);

  S3L_setTransform3D(1909,16,-3317,0,-510,0,512,512,512,&(models[1].transform));


  S3L_initVec4(&carDirection);
  
  scene.camera.transform.translation.y = S3L_FRACTIONS_PER_UNIT / 2;
  scene.camera.transform.rotation.x = -S3L_FRACTIONS_PER_UNIT / 16;


}

// process user input and update the world state
void update(uint32_t tick) {

}

// draw the world
void draw(uint32_t tick) {
  pen(0,0,0);
  clear();
      models[1].transform.rotation.y += models[1].transform.rotation.z; // overturn the car for the rendering
    S3L_newFrame();
  S3L_drawScene(scene);


      models[1].transform.rotation.y -= models[1].transform.rotation.z; // turn the car back for the physics


      uint32_t frameDiffMs = tick*100;
      previousTime = tick;

      int16_t step = (velocity * frameDiffMs) / 1024;                             
      int16_t stepFriction = (FRICTION * frameDiffMs) / 1024;                     
      int16_t stepRotation = TURN_SPEED * frameDiffMs * S3L_max(0,velocity - 200) / (MAX_VELOCITY * 1024);
      int16_t stepVelocity = S3L_nonZero((ACCELERATION * frameDiffMs) / 1024);

      if (stepRotation == 0 && S3L_abs(velocity) >= 200)
        stepRotation = 10;

      if (velocity < 0)
        stepRotation *= -1;

      if (button(LEFT))
      {
        models[1].transform.rotation.y += stepRotation;
        models[1].transform.rotation.z =
          S3L_min(S3L_abs(velocity) / 64, models[1].transform.rotation.z + 1);
      }
      else if (button(RIGHT))
      {
        models[1].transform.rotation.y -= stepRotation;
        models[1].transform.rotation.z =
          S3L_max(-S3L_abs(velocity) / 64, models[1].transform.rotation.z - 1);
      }
      else
        models[1].transform.rotation.z = (models[1].transform.rotation.z * 3) / 4;

      S3L_rotationToDirections(models[1].transform.rotation,S3L_FRACTIONS_PER_UNIT,&carDirection,0,0);

      S3L_Vec4 previousCarPos = models[1].transform.translation;
      S3L_Vec4 previousCamPos = scene.camera.transform.translation;

      int16_t friction = 0;

      if (button(A))
        velocity = S3L_min(MAX_VELOCITY,velocity + (velocity < 0 ? (2 * stepVelocity) : stepVelocity));
      else if (button(B))
        velocity = S3L_max(-MAX_VELOCITY,velocity - (velocity > 0 ? (2 * stepVelocity) : stepVelocity));
      else
        friction = 1;

      models[1].transform.translation.x += (carDirection.x * step) / S3L_FRACTIONS_PER_UNIT;
      models[1].transform.translation.z += (carDirection.z * step) / S3L_FRACTIONS_PER_UNIT;

      uint8_t coll = collision(models[1].transform.translation);

      if (coll != 0)
      {
        if (coll == 1)
        {
          handleCollision(&(models[1].transform.translation),previousCarPos);
          friction = 2;
        }
        else if (coll == 2)
        {
          // teleport the car
          models[1].transform.translation.x += 5 * S3L_FRACTIONS_PER_UNIT;
          models[1].transform.translation.z += 2 * S3L_FRACTIONS_PER_UNIT;
        }
        else
        {
          // teleport the car
          models[1].transform.translation.x -= 5 * S3L_FRACTIONS_PER_UNIT;
          models[1].transform.translation.z -= 2 * S3L_FRACTIONS_PER_UNIT;
        }
      }

      if (velocity > 0)
        velocity = S3L_max(0,velocity - stepFriction * friction);
      else
        velocity = S3L_min(0,velocity + stepFriction * friction);

      S3L_Unit cameraDistance =
        S3L_interpolate(S3L_FRACTIONS_PER_UNIT / 2,(3 * S3L_FRACTIONS_PER_UNIT) / 4,S3L_abs(velocity),MAX_VELOCITY);

      scene.camera.transform.translation.x =
        scene.models[1].transform.translation.x - (carDirection.x * cameraDistance) / S3L_FRACTIONS_PER_UNIT;

      scene.camera.transform.translation.z =
        scene.models[1].transform.translation.z - (carDirection.z * cameraDistance) / S3L_FRACTIONS_PER_UNIT;

      scene.camera.transform.rotation.y = models[1].transform.rotation.y;

}

