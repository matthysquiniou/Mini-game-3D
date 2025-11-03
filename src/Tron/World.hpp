#pragma once
#include "Geometry.hpp"
#include "RigidBody.hpp"
#include "Clock.hpp"
#include "Entity.hpp"
#include "GameState.hpp"
#include "GameManager.hpp"
#include "Pool.hpp"
#include <unordered_map>
#include <Camera.h>
#include <Window.h>
#include <Geometries/Geometries.h>
#include "../src/framework/Texture.h"
#include <queue>
#include <includes/Core/Maths/Vector3.h>

class World {
public:
	void loop();
	World();
	Entity CreateEntity();
	Entity GetPlayer1Entity();
	Entity GetPlayer2Entity();
	void DestroyEntity(Entity entity);

private:
	void doCollisions();
	bool shouldCollide(EntityType entityType1, EntityType entityType2);
	gce::Vector3f32 checkCollision(sr::Geometry* geometry1, sr::Geometry* geometry2);
	void doCollision(Geometry& geometry, Entity ownId, gce::Vector3f32 penetration, Geometry& collideWith, Entity collideWithId);
	void updatePhysic(float dt);
	void applyPlayerInput(float dt);
	void reset();
	void draw();
	void init();

public:
	std::unordered_map<Entity,Geometry> m_geometries;
	std::unordered_map<Entity,RigidBody> m_rigidBodies;
	Pool<sr::Cube> m_cubePool;

private:
	Entity m_player1Entity;
	Entity m_player2Entity;
	Entity m_nextEntity = 0;
	std::queue<Entity> m_freeEntities;
	Clock m_clock;
	float m_gravity;
	gce::Vector3f32 m_gravity_direction;
	GameManager* m_gameManager;
	sr::Window* m_window;
	sr::Camera* m_camera;
};