#include "pch.h"

#include "World.hpp"
#include "TransformMouvement.hpp"
#include <Inputs/InputSystem.h>
#include <Inputs/InputsMethods.h>

World::World() {
	m_clock = Clock();
	m_gravity = 9.81f;
	m_gravity_direction = {0.f, 1.f, 0.f};

	m_window = new sr::Window(L"Tron", 1920, 1080);
	m_camera = new sr::Camera(sr::CameraType::PERSPECTIVE);
	m_camera->SetPosition({ 0.f, 100.f, 0.f });
	m_camera->SetRotation({ 90.f, 0.f, 0.f });
	m_camera->SetFOV(gce::PI / 4.f);
	m_camera->SetFarPlane(500.f);
	m_camera->SetNearPlane(0.1f);

	m_gameManager = new GameManager();
	
	init();
}

void World::init() {	

	float minX = -70.f;
	float maxX = 70.f;
	float minZ = -35.f;
	float maxZ = 35.f;

	sr::Cube* player1 = m_cubePool.acquire();
	player1->SetPosition({ 0.f, 1.f, minZ + maxZ * 0.5f });
	player1->SetScale({ 1.f, 1.f, 1.f });
	player1->SetColor({ 0.f,0.f,1.f });

	m_player1Entity = CreateEntity();

	m_geometries[m_player1Entity] = { player1, EntityType::PLAYER };
	m_rigidBodies[m_player1Entity] = { 0.0001f, { 0.f, 0.f, 0.1f } };

	sr::Cube* player2 = m_cubePool.acquire();
	player2->SetPosition({ 0.f, 1.f, maxZ + minZ * 0.5f });
	player2->SetScale({ 1.f, 1.f, 1.f });
	player2->SetColor({ 0.f,1.f,0.f });

	m_player2Entity = CreateEntity();

	m_geometries[m_player2Entity] = { player2, EntityType::PLAYER };
	m_rigidBodies[m_player2Entity] = { 0.0001f, { 0.f, 0.f, -0.1f } };

	Entity floorEntity = CreateEntity();

	sr::Cube* floor = m_cubePool.acquire();
	floor->SetPosition({ 0.f, 0.f, 0.f });
	floor->SetScale({ std::abs(minX) + std::abs(maxX), 1.f, std::abs(minZ) + std::abs(maxZ) });
	floor->SetColor({ 1.f,0.f,1.f });

	m_geometries[floorEntity] = { floor, EntityType::FLOOR };

	Entity wall1Entity = CreateEntity();

	sr::Cube* wall1 = m_cubePool.acquire();
	wall1->SetPosition({ maxX + 1.f, 0.f, 0.f });
	wall1->SetScale({ 2.f, 2.f, std::abs(minZ) + std::abs(maxZ) });
	wall1->SetColor({ 1.f,0.f,0.f });

	m_geometries[wall1Entity] = { wall1, EntityType::WALL };

	Entity wall2Entity = CreateEntity();

	sr::Cube* wall2 = m_cubePool.acquire();
	wall2->SetPosition({ minX - 1.f, 0.f, 0.f });
	wall2->SetScale({ 2.f, 2.f, std::abs(minZ) + std::abs(maxZ) });
	wall2->SetColor({ 1.f,0.f,0.f });

	m_geometries[wall2Entity] = { wall2, EntityType::WALL };

	Entity wall3Entity = CreateEntity();

	sr::Cube* wall3 = m_cubePool.acquire();
	wall3->SetPosition({ 0.f, 0.f, minZ - 1.f });
	wall3->SetScale({ std::abs(minX) + std::abs(maxX) + 4.f, 2.f, 2.f });
	wall3->SetColor({ 1.f,0.f,0.f });

	m_geometries[wall3Entity] = { wall3, EntityType::WALL };

	Entity wall4Entity = CreateEntity();

	sr::Cube* wall4 = m_cubePool.acquire();
	wall4->SetPosition({ 0.f, 0.f, maxZ + 1.f });
	wall4->SetScale({ std::abs(minX) + std::abs(maxX) + 4.f, 2.f, 2.f });
	wall4->SetColor({ 1.f,0.f,0.f });

	m_geometries[wall4Entity] = { wall4, EntityType::WALL };
	
}

void World::loop() {
	while (m_window->IsOpen()) {
		float dt = m_clock.getDtAsSeconds();

		switch (m_gameManager->m_gameState)
		{
		case PLAYING:
			m_gameManager->update(dt, this);
			applyPlayerInput(dt);
			updatePhysic(dt);
			doCollisions();
			draw();
			break;
		case PAUSED:
		case FINISHED:
			applyPlayerInput(dt);
			draw();
			break;
		default:
			break;
		}
	}
}

void World::applyPlayerInput(float dt) {
	switch (m_gameManager->m_gameState)
	{
	case PLAYING: {
		if (sr::GetKey(sr::Keyboard::ESC)) {
			m_gameManager->m_gameState = GameState::PAUSED;
			return;
		}
		sr::Geometry* player1Geometry = m_geometries[m_player1Entity].geometry;
		RigidBody& player1RigidBody = m_rigidBodies[m_player1Entity];

		int zDir = sr::GetKey(sr::Keyboard::Z) - sr::GetKey(sr::Keyboard::S);
		int xDir = sr::GetKey(sr::Keyboard::D) - sr::GetKey(sr::Keyboard::Q);
		if (zDir != 0 && xDir != 0) return;
		if (zDir != 0 && player1RigidBody.velocity.z == 0.f) {
			m_gameManager->changeDirectionWallSpawn(this, m_player1Entity);
			player1RigidBody.velocity.x = 0.f;
			player1RigidBody.velocity.z = 0.1f * zDir;
		} else if (xDir != 0 && player1RigidBody.velocity.x == 0.f) {
			m_gameManager->changeDirectionWallSpawn(this, m_player1Entity);
			player1RigidBody.velocity.z = 0.f;
			player1RigidBody.velocity.x = 0.1f * xDir;
		}
	}

		break;
	case PAUSED:
		if (sr::GetKey(sr::Keyboard::C)) {
			m_gameManager->m_gameState = GameState::PLAYING;
			return;
		}
		if (sr::GetKey(sr::Keyboard::R)) {
			reset();
			return;
		}

		break;
	case FINISHED:
		if (sr::GetKey(sr::Keyboard::R)) {
			reset();
			return;
		}
		break;
	default:
		break;
	}
}

void World::reset() {
	m_gameManager->reset();
	for (Entity entity = 0; entity < m_nextEntity; entity++)
	{
		DestroyEntity(entity);
	}

	m_nextEntity = 0;
	m_camera->SetPosition({ 0.f, 100.f, 0.f });
	m_camera->SetRotation({ 90.f, 0.f, 0.f });
	std::queue<int> empty;
	std::swap(m_freeEntities, empty);
	init();
}

void World::updatePhysic(float dt) {
	for (auto& rigidBody : m_rigidBodies) {
		sr::Geometry* geometry = m_geometries[rigidBody.first].geometry;
		rigidBody.second.velocity -= m_gravity_direction * m_gravity * dt * rigidBody.second.mass;

		geometry->Translate(rigidBody.second.velocity);
	}
}

void World::doCollisions() {
	struct Collision {
		Entity a, b;
		gce::Vector3f32 penetration;
	};

	std::vector<Collision> collisions;

	for (auto g1 = m_geometries.begin(); g1 != m_geometries.end(); ++g1) {
		for (auto g2 = std::next(g1); g2 != m_geometries.end(); ++g2) {
			if (!shouldCollide(g1->second.entityType, g2->second.entityType)) continue;

			auto penetration = checkCollision(g1->second.geometry, g2->second.geometry);
			if (penetration == gce::Vector3f32{ 0.f, 0.f, 0.f }) continue;

			collisions.push_back({ g1->first, g2->first, penetration });
		}
	}
	for (auto& c : collisions) {
		auto it1 = m_geometries.find(c.a);
		auto it2 = m_geometries.find(c.b);
		if (it1 == m_geometries.end() || it2 == m_geometries.end()) continue;

		doCollision(it1->second, c.a, c.penetration, it2->second, c.b);
		doCollision(it2->second, c.b, c.penetration * -1.f, it1->second, c.a);
	}
}

bool World::shouldCollide(EntityType entityType1, EntityType entityType2) {
	if (entityType1 > entityType2) std::swap(entityType1, entityType2);
	return entityType1 == EntityType::FLOOR && entityType2 == EntityType::PLAYER ||
		entityType1 == EntityType::PLAYER && entityType2 == EntityType::WALL ||
		entityType1 == EntityType::PLAYER && entityType2 == EntityType::PLAYER;
}

gce::Vector3f32 World::checkCollision(sr::Geometry* geometry1, sr::Geometry* geometry2) {
	gce::Vector3f32 position1 = geometry1->GetPosition();
	gce::Vector3f32 scale1 = geometry1->GetScale() * 0.5f;
	gce::Vector3f32 position2 = geometry2->GetPosition();
	gce::Vector3f32 scale2 = geometry2->GetScale() * 0.5f;

	gce::Vector3f32 penetration = { 0.f, 0.f, 0.f };

	float dx = position2.x - position1.x;
	float px = (scale1.x + scale2.x) - std::abs(dx);
	if (px <= 0.f) return { 0.f, 0.f, 0.f };
	penetration.x = (dx < 0) ? -px : px;

	float dy = position2.y - position1.y;
	float py = (scale1.y + scale2.y) - std::abs(dy);
	if (py <= 0.f) return { 0.f, 0.f, 0.f };
	penetration.y = (dy < 0) ? -py : py;

	float dz = position2.z - position1.z;
	float pz = (scale1.z + scale2.z) - std::abs(dz);
	if (pz <= 0.f) return { 0.f, 0.f, 0.f };
	penetration.z = (dz < 0) ? -pz : pz;

	return penetration;
}

void World::doCollision(Geometry& geometry, Entity ownId, gce::Vector3f32 penetration, Geometry& collideWith, Entity collideWithId) {
	switch (geometry.entityType)
	{
	case PLAYER:
		switch (collideWith.entityType)
		{
		case FLOOR:
			m_rigidBodies[ownId].velocity.y = 0;
			geometry.geometry->Translate({ 0.f, -penetration.y, 0.f });
			break;
		case PLAYER:
		case WALL:
			m_gameManager->finishGame(this, ownId, collideWithId);
			break;
		default:
			break;
		}

		break;
	default:
		break;
	}
}

void World::draw() {
	m_window->Begin(*m_camera);

	for (auto& geometry : m_geometries) {
		m_window->Draw(*geometry.second.geometry);
	}

	switch (m_gameManager->m_gameState)
	{
	case PAUSED:
		m_window->DrawText(*m_gameManager->m_pauseText);
		m_window->DrawText(*m_gameManager->m_resumeText);
		m_window->DrawText(*m_gameManager->m_resetText);
		break;
	case FINISHED:
		m_window->DrawText(*m_gameManager->m_finishText);
		m_window->DrawText(*m_gameManager->m_resetText);
		break;
	default:
		break;
	}

	m_window->End();
	m_window->Display();
}

Entity World::CreateEntity() {
	Entity id;
	if (!m_freeEntities.empty()) {
		id = m_freeEntities.front();
		m_freeEntities.pop();
	}
	else {
		id = m_nextEntity++;
	}
	return id;
}

void World::DestroyEntity(Entity entity) {
	auto it = m_geometries.find(entity);
	if (it != m_geometries.end()) {
		m_cubePool.release(static_cast<sr::Cube*>(it->second.geometry));
		m_geometries.erase(it);
	}

	m_rigidBodies.erase(entity);
	m_freeEntities.push(entity);
}

Entity World::GetPlayer1Entity() {
	return m_player1Entity;
}

Entity World::GetPlayer2Entity() {
	return m_player2Entity;
}