#pragma once
#ifndef GEOMETRY_STRUCT
#define GEOMETRY_STRUCT
#include <Geometries/Geometry.h>
#include "EntityType.hpp"

struct Geometry {
	sr::Geometry* geometry;
	EntityType entityType;
};

#endif 
