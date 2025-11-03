#include "pch.h"

#include "HelperFunction.hpp"
#include <includes/Core/Maths/Vector3.h>
#include <includes/Core/Maths/Vector2.hpp>
#include <includes/Core/Maths/MathsFunctions.hpp>
#include <random>
#include <iostream>

float getCircleAngleXZFromCenter(gce::Vector3f32 center, gce::Vector3f32 point) {
	float angle = atan2(point.z - center.z, point.x - center.x);
	if (angle < 0) angle += 2.0f * gce::PI;
	return angle;
}

float addAngleWithDistance(float distance, float radius, float angle) {
    if (radius == 0.0f) return angle;
    float delta = distance / radius;
    float newAngle = angle + delta;
    newAngle = fmod(newAngle, 2.0f * gce::PI);
    if (newAngle < 0) newAngle += 2.0f * gce::PI;
    return newAngle;
}

gce::Vector3f32 getCirclePointXZFromCenter(gce::Vector3f32 center, float radius, float angle) {
    gce::Vector3f32 point = { cos(angle) * radius, center.y, sin(angle) * radius };
    return point;
}

gce::Vector3f32 getRandomPointOnCircleXZ(float radius, float Ypos, gce::Vector3f32 center) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.f, 2.f * gce::PI);    

    float angle = dist(gen);
    float x = center.x + radius * cos(angle);
    float z = center.z + radius * sin(angle);

    return { x, Ypos, z };
}