#include "pch.h"

#include "TransformMouvement.hpp"
#include "HelperFunction.hpp"

void rotateXZAround(sr::Geometry& geometry, gce::Vector3f32 point, float dt, float speed, bool clockwise) {
	gce::Vector3f32 geometryPosition = geometry.GetPosition();
	point.y = geometryPosition.y;
	float radius = (geometryPosition - point).Norm();
	float initialAngle = getCircleAngleXZFromCenter(point, geometryPosition);
	int direction = clockwise ? -1 : 1;
	float newAngle = addAngleWithDistance(speed * dt * direction * radius, radius, initialAngle);
	gce::Vector3f32 newPosition = getCirclePointXZFromCenter(point, radius, newAngle);
	geometry.Rotate({ 0.f,  (speed * dt * -direction * radius) / radius, 0.f });
	geometry.SetPosition(newPosition);
};

void rotateXZAround(sr::Camera& camera, gce::Vector3f32 point, float dt, float speed, bool clockwise) {
	gce::Vector3f32 geometryPosition = camera.GetPosition();
	point.y = geometryPosition.y;
	float radius = (geometryPosition - point).Norm();
	float initialAngle = getCircleAngleXZFromCenter(point, geometryPosition);
	int direction = clockwise ? -1 : 1;
	float newAngle = addAngleWithDistance(speed * dt * direction * radius, radius, initialAngle);
	gce::Vector3f32 newPosition = getCirclePointXZFromCenter(point, radius, newAngle);
	camera.Rotate({ 0.f, (speed * dt * -direction * radius) / radius,0.f });
	camera.SetPosition(newPosition);
};