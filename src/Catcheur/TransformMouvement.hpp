#pragma once
#include <Geometries/Geometry.h>
#include "EntityType.hpp"
#include "RigidBody.hpp"
#include <includes/Core/Maths/Vector3.h>

void rotateXZAround(sr::Geometry& geometry, gce::Vector3f32 point, float dt, float speed, bool clockwise);
void rotateXZAround(sr::Camera& geometry, gce::Vector3f32 point, float dt, float speed, bool clockwise);