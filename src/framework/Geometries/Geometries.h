#pragma once
#include "Geometry.h"

#include <GeometryFactory.h>

namespace sr
{
    class Cube : public Geometry
    {
    public:
        Cube() : Geometry(gce::GeometryFactory::CreateCubeGeo()) {}
        ~Cube() override = default;
        bool isRound() override { return false; }
    };
    
    class Sphere : public Geometry
    {
    public:
        Sphere() : Geometry(gce::GeometryFactory::CreateSphereGeo(0.5f, 20, 20)) {}
        ~Sphere() override = default;
        bool isRound() override { return true; }
    };

    class HalfSphere : public Geometry
    {
    public:
        HalfSphere() : Geometry(gce::GeometryFactory::CreateHalfSphereGeo(0.5f, 20, 20)) {}
        ~HalfSphere() override = default;
        bool isRound() override { return true; }
    };
    
    class Capsule : public Geometry
    {
    public:
        Capsule() : Geometry(gce::GeometryFactory::CreateCapsuleGeo(0.5f, 1.0f, 20, 20)) {}
        ~Capsule() override = default;
        bool isRound() override { return true; }
    };

    class IcoSphere : public Geometry
    {
    public:
        IcoSphere() : Geometry(gce::GeometryFactory::CreateIcoSphereGeo(0.5f, 1)) {}
        ~IcoSphere() override = default;
        bool isRound() override { return true; }
    };
    
    class Plane : public Geometry
    {
    public:
        Plane() : Geometry(gce::GeometryFactory::CreatePlaneGeo(1, 1)) {}
        ~Plane() override = default;
        bool isRound() override { return false; }
    };
    
    class Donut : public Geometry
    {
    public:
        Donut() : Geometry(gce::GeometryFactory::CreateDonutGeo(0.5f, 0.25f, 20, 20)) {}
        ~Donut() override = default;
        bool isRound() override { return true; }
    };
    
    class Cylinder : public Geometry
    {
    public:
        Cylinder() : Geometry(gce::GeometryFactory::CreateCylinderGeo(0.5f, 0.5f, 1.0f, 20, 20)) {}
        ~Cylinder() override = default;
        bool isRound() override { return true; }
    };

    
}