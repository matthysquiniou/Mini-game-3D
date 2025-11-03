#include "pch.h"

#include "World.hpp"
#include "TransformMouvement.hpp"
#include <Inputs/InputSystem.h>
#include <Inputs/InputsMethods.h>

World::World() {
	m_clock = Clock();
	m_gravity = 9.81f;
	m_gravity_direction = {0.f, 1.f, 0.f};

	m_window = new sr::Window(L"Catcheur", 1920, 1080);
	m_camera = new sr::Camera(sr::CameraType::PERSPECTIVE);
	m_camera->SetPosition({ 0.0f, 20.0f, -20.0f });
	m_camera->SetRotation({ 45.0f, 0.0f, 0.0f });
	m_camera->SetFOV(gce::PI / 4.0f);
	m_camera->SetFarPlane(500.0f);
	m_camera->SetNearPlane(0.1f);

	m_gameManager = new GameManager();

	
	init();
}

void World::init() {

	m_floorTexture = new sr::Texture("res/Textures/grass.png");
	m_railTexture = new sr::Texture("res/Textures/steel.png");
	
	sr::Cube* player = m_cubePool.acquire();
	player->SetPosition({ 0.f, 1.f, -5.f });
	player->SetScale({ 1.f, 1.f, 1.f });
	player->SetColor({ 1.f,1.f,1.f });

	m_playerEntity = CreateEntity();

	m_geometries[m_playerEntity] = { player, EntityType::PLAYER };
	m_rigidBodies[m_playerEntity] = { 0.0001f, { 0.f, 0.f, 0.f } };
	
	Entity floorEntity = CreateEntity();

	sr::Cube* floor = m_cubePool.acquire();
	floor->SetPosition({ 0.0f, 0.0f, 0.0f });
	floor->SetScale({ 20.0f, 1.f, 20.0f });
	floor->SetTexture(*m_floorTexture);

	m_geometries[floorEntity] = { floor, EntityType::FLOOR };
	
	Entity railEntity = CreateEntity();
	
	sr::Geometry* rail = m_customDonutPool.acquireCustomDonut(1.f, 0.1f, 100, 100);
	rail->SetPosition({ 0.f, 0.6f, 0.f });
	rail->SetScale({ 5.f, 1.f, 5.f });
	rail->SetTexture(*m_railTexture);
	m_geometries[railEntity] = { rail, EntityType::DRAW };
	
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
	bool Q = sr::GetKey(sr::Keyboard::Q);
	bool D = sr::GetKey(sr::Keyboard::D);

	sr::Geometry* playerGeometry = m_geometries[m_playerEntity].geometry;
	gce::Vector3f32 rotationPoint = { 0.f,0.1f,0.f };

	switch (m_gameManager->m_gameState)
	{
	case PLAYING:
		if (sr::GetKey(sr::Keyboard::ESC)) {
			m_gameManager->m_gameState = GameState::PAUSED;
			return;
		}

		if (!(Q ^ D)) return;

		if (Q) {
			rotateXZAround(*playerGeometry, rotationPoint, dt, 2.f, true);
			rotateXZAround(*m_camera, rotationPoint, dt, 2.f, true);
		}
		if (D) {
			rotateXZAround(*playerGeometry, rotationPoint, dt, 2.f, false);
			rotateXZAround(*m_camera, rotationPoint, dt, 2.f, false);
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
	m_camera->SetPosition({ 0.0f, 20.0f, -20.0f });
	m_camera->SetRotation({ 45.0f, 0.0f, 0.0f });
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
	return entityType1 == EntityType::FLOOR && entityType2 == EntityType::COLLECTIBLE ||
		entityType1 == EntityType::FLOOR && entityType2 == EntityType::PLAYER ||
		entityType1 == EntityType::PLAYER && entityType2 == EntityType::COLLECTIBLE;
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
		if (collideWith.entityType != EntityType::FLOOR) break;
		m_rigidBodies[ownId].velocity.y = 0;
		geometry.geometry->Translate({0.f, -penetration.y, 0.f});
		break;
	case COLLECTIBLE:
		switch (collideWith.entityType) 
		{
			case PLAYER:
				DestroyEntity(ownId);
				m_gameManager->addScore(1);
				break;
			case FLOOR:
				DestroyEntity(ownId);
				m_gameManager->m_remainingTime--;
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
	case PLAYING:
		m_window->DrawText(*m_gameManager->m_scorePlayingText);
		m_window->DrawText(*m_gameManager->m_currentTimeText);
		break;
	case PAUSED:
		m_window->DrawText(*m_gameManager->m_scorePlayingText);
		m_window->DrawText(*m_gameManager->m_currentTimeText);
		m_window->DrawText(*m_gameManager->m_pauseText);
		m_window->DrawText(*m_gameManager->m_resumeText);
		m_window->DrawText(*m_gameManager->m_resetText);
		break;
	case FINISHED:
		m_window->DrawText(*m_gameManager->m_finishText);
		m_window->DrawText(*m_gameManager->m_scoreFinishText);
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
		switch (it->second.entityType) {
		case EntityType::PLAYER:
		case EntityType::FLOOR:
			m_cubePool.release(static_cast<sr::Cube*>(it->second.geometry));
			break;
		case EntityType::DRAW:
			m_customDonutPool.release(it->second.geometry);
			break;
		case EntityType::COLLECTIBLE:
			m_sherePool.release(static_cast<sr::Sphere*>(it->second.geometry));
			break;
		default:
			break;
		}
		m_geometries.erase(it);
	}

	m_rigidBodies.erase(entity);
	m_freeEntities.push(entity);
}
