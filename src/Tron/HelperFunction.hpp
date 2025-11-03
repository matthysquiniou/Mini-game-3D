#pragma once
#include <includes/Core/define.h>

float getCircleAngleXZFromCenter(gce::Vector3f32 center, gce::Vector3f32 point);
float addAngleWithDistance(float distance, float radius, float angle);
gce::Vector3f32 getCirclePointXZFromCenter(gce::Vector3f32 center, float radius, float angle);
gce::Vector3f32 getRandomPointOnCircleXZ(float radius, float Ypos, gce::Vector3f32 center);