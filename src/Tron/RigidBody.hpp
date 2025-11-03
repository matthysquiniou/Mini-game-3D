#pragma once
#ifndef RIGID_BODY_STRUCT
#define RIGID_BODY_STRUCT
#include <includes/Core/Maths/Vector3.h>

struct RigidBody {
    float mass;
    gce::Vector3f32 velocity;
};

#endif 