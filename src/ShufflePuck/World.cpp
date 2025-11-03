#include "pch.h"

#include "World.hpp"
#include "TransformMouvement.hpp"
#include <Inputs/InputSystem.h>
#include <Inputs/InputsMethods.h>
#include "HelperFunction.hpp"

World::World() {
	m_clock = Clock();
	m_gravity = 9.81f;
	m_friction = 0.99f;
	m_gravity_direction = {0.f, 1.f, 0.f};

	m_window = new sr::Window(L"ShufflePuck", 1920, 1080);
	m_camera = new sr::Camera(sr::CameraType::PERSPECTIVE);
	m_camera->SetPosition({ 0.0f, 10.0f, -20.0f });
	m_camera->SetRotation({ 30.0f, 0.0f, 0.0f });
	m_camera->SetFOV(gce::PI / 5.0f);
	m_camera->SetFarPlane(500.0f);
	m_camera->SetNearPlane(0.1f);

	m_gameManager = new GameManager();
	
	init();
}

void World::init() {
	sr::Cylinder* puck = m_cylinderPool.acquire();
	puck->SetPosition({ 0.f, 0.5f, 0.f });
	puck->SetScale({ 0.66f, 0.5f, 0.66f });
	puck->SetColor({ 0.f,1.f,1.f });

	m_puckEntity = CreateEntity();

	m_geometries[m_puckEntity] = { puck, EntityType::PUCK };
	m_rigidBodies[m_puckEntity] = { 0.0001f, { 0.f, 0.f, 0.f } };

	sr::Cylinder* player = m_cylinderPool.acquire();
	player->SetPosition({ 0.f, 0.5f, -5.f });
	player->SetScale({ 1.f, 0.5f, 1.f });
	player->SetColor({ 1.f,1.f,1.f });

	m_playerEntity = CreateEntity();

	m_geometries[m_playerEntity] = { player, EntityType::PADDLE };
	m_rigidBodies[m_playerEntity] = { 0.0001f, { 0.f, 0.f, 0.f } };

	
	sr::Cylinder* ia = m_cylinderPool.acquire();
	ia->SetPosition({ 0.f, 0.5f, 5.f });
	ia->SetScale({ 1.f, 0.5f, 1.f });
	ia->SetColor({ 1.f,1.f,1.f });

	m_iaEntity = CreateEntity();

	m_geometries[m_iaEntity] = { ia, EntityType::PADDLE };
	m_rigidBodies[m_iaEntity] = { 0.0001f, { 0.f, 0.f, 0.f } };
	
	
	Entity floorEntity = CreateEntity();

	sr::Cube* floor = m_cubePool.acquire();
	floor->SetPosition({ 0.f, 0.f, 0.f });
	floor->SetScale({ 10.0f, 1.f, 20.f });
	floor->SetColor({ 1.f,0.f,1.f });

	m_geometries[floorEntity] = { floor, EntityType::FLOOR };
	
	Entity wallEntity1 = CreateEntity();

	sr::Cube* wall1 = m_cubePool.acquire();
	wall1->SetPosition({ 6.0f, 0.f, 0.f });
	wall1->SetScale({ 2.f, 2.f, 24.f });
	wall1->SetColor({ 0.f,0.f,1.f });

	m_geometries[wallEntity1] = { wall1, EntityType::WALL };

	Entity wallEntity2 = CreateEntity();

	sr::Cube* wall2 = m_cubePool.acquire();
	wall2->SetPosition({ -6.0f, 0.f, 0.f });
	wall2->SetScale({ 2.f, 2.f, 24.f });
	wall2->SetColor({ 0.f,0.f,1.f });
	
	m_geometries[wallEntity2] = { wall2, EntityType::WALL };

	Entity wallEntity3 = CreateEntity();

	sr::Cube* wall3 = m_cubePool.acquire();
	wall3->SetPosition({ 3.f, 0.f, 11.f });
	wall3->SetScale({ 4.f, 2.f, 2.f });
	wall3->SetColor({ 0.f,0.f,1.f });

	m_geometries[wallEntity3] = { wall3, EntityType::WALL };

	Entity wallEntity4 = CreateEntity();

	sr::Cube* wall4 = m_cubePool.acquire();
	wall4->SetPosition({ -3.f, 0.f, 11.f });
	wall4->SetScale({ 4.f, 2.f, 2.f });
	wall4->SetColor({ 0.f,0.f,1.f });

	m_geometries[wallEntity4] = { wall4, EntityType::WALL };

	Entity wallEntity5 = CreateEntity();

	sr::Cube* wall5 = m_cubePool.acquire();
	wall5->SetPosition({ 3.f, 0.f, -11.f });
	wall5->SetScale({ 4.f, 2.f, 2.f });
	wall5->SetColor({ 0.f,0.f,1.f });

	m_geometries[wallEntity5] = { wall5, EntityType::WALL };

	Entity wallEntity6 = CreateEntity();

	sr::Cube* wall6 = m_cubePool.acquire();
	wall6->SetPosition({ -3.f, 0.f, -11.f });
	wall6->SetScale({ 4.f, 2.f, 2.f });
	wall6->SetColor({ 0.f,0.f,1.f });

	m_geometries[wallEntity6] = { wall6, EntityType::WALL };

	m_playerGoalEntity = CreateEntity();

	sr::Cube* goalPlayer = m_cubePool.acquire();
	goalPlayer->SetPosition({ 0.f, 0.f, -11.f });
	goalPlayer->SetScale({ 2.f, 2.f, 2.f });
	goalPlayer->SetColor({ 1.f,0.f,0.f });

	m_geometries[m_playerGoalEntity] = { goalPlayer, EntityType::GOAL };

	Entity iaGoalEntity = CreateEntity();

	sr::Cube* goalIa = m_cubePool.acquire();
	goalIa->SetPosition({ 0.f, 0.f, 11.f });
	goalIa->SetScale({ 2.f, 2.f, 2.f });
	goalIa->SetColor({ 1.f,0.f,0.f });

	m_geometries[iaGoalEntity] = { goalIa, EntityType::GOAL };

	Entity separationEntity = CreateEntity();

	sr::Cube* separation = m_cubePool.acquire();
	separation->SetPosition({ 0.f, 0.5f, 0.f });
	separation->SetScale({ 10.f, 0.1f, 0.1f });
	separation->SetColor({ 1.f,0.f,0.f });

	m_geometries[separationEntity] = { separation, EntityType::DRAW };

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
	case PLAYING:
		if (sr::GetKey(sr::Keyboard::ESC)) {
			sr::ShowMouseCursor();
			sr::UnlockMouseCursor();
			m_gameManager->m_gameState = GameState::PAUSED;
			return;
		}
		m_gameManager->movePlayerInsideBoundries(this, dt);

		break;
	case PAUSED:
		if (sr::GetKey(sr::Keyboard::C)) {
			m_gameManager->m_gameState = GameState::PLAYING;
			sr::HideMouseCursor();
			sr::LockMouseCursor();
			sr::SetMousePosition({ 960,540 });
			sr::GetMouseDelta();
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
	m_camera->SetPosition({ 0.0f, 10.0f, -20.0f });
	m_camera->SetRotation({ 30.0f, 0.0f, 0.0f });
	std::queue<int> empty;
	std::swap(m_freeEntities, empty);
	init();
}

void World::updatePhysic(float dt) { 
	for (auto& rigidBody : m_rigidBodies) {
		sr::Geometry* geometry = m_geometries[rigidBody.first].geometry;
		rigidBody.second.velocity -= m_gravity_direction * m_gravity * dt * rigidBody.second.mass;
		float frictionFactor = expf(-m_friction * dt);
		rigidBody.second.velocity *= frictionFactor;
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
	return entityType1 == EntityType::PUCK && entityType2 == EntityType::FLOOR ||
		entityType1 == EntityType::PUCK && entityType2 == EntityType::WALL ||
		entityType1 == EntityType::PUCK && entityType2 == EntityType::PADDLE ||
		entityType1 == EntityType::PUCK && entityType2 == EntityType::GOAL ||
		entityType1 == EntityType::PADDLE && entityType2 == EntityType::FLOOR || 
		entityType1 == EntityType::PADDLE && entityType2 == EntityType::WALL ||
		entityType1 == EntityType::PADDLE && entityType2 == EntityType::GOAL;
}

gce::Vector3f32 World::checkCollision(sr::Geometry* g1, sr::Geometry* g2) {
	auto pos1 = g1->GetPosition();
	auto pos2 = g2->GetPosition();

	if (g1->isRound() && g2->isRound()) {
		float r1 = g1->GetScale().x * 0.5f;
		float r2 = g2->GetScale().x * 0.5f;

		gce::Vector3f32 delta = pos1 - pos2;
		float dist = sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
		float penetrationDepth = (r1 + r2) - dist;

		if (penetrationDepth <= 0.f)
			return { 0.f, 0.f, 0.f };

		const float epsilon = 1e-6f;
		gce::Vector3f32 dir = (dist > epsilon) ? (delta / dist) : gce::Vector3f32{ 1.f,0.f,0.f };
		return dir * penetrationDepth;
	}

	auto handleSphereCube = [](sr::Geometry* sphere, sr::Geometry* cube) -> gce::Vector3f32 {
		gce::Vector3f32 spherePos = sphere->GetPosition();
		gce::Vector3f32 cubePos = cube->GetPosition();
		gce::Vector3f32 half = cube->GetScale() * 0.5f;

		gce::Vector3f32 closest;
		closest.x = gce::Max(cubePos.x - half.x, gce::Min(spherePos.x, cubePos.x + half.x));
		closest.y = gce::Max(cubePos.y - half.y, gce::Min(spherePos.y, cubePos.y + half.y));
		closest.z = gce::Max(cubePos.z - half.z, gce::Min(spherePos.z, cubePos.z + half.z));

		gce::Vector3f32 delta = spherePos - closest;
		float dist2 = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
		float radius = sphere->GetScale().x * 0.5f;

		if (dist2 >= radius * radius)
			return { 0.f,0.f,0.f };

		float dist = sqrtf(dist2);
		const float epsilon = 1e-6f;
		gce::Vector3f32 dir = (dist > epsilon) ? (delta / dist) : gce::Vector3f32{ 1.f,0.f,0.f };
		return dir * (radius - dist);
		};

	if (g1->isRound() && !g2->isRound()) return handleSphereCube(g1, g2);
	if (!g1->isRound() && g2->isRound()) return handleSphereCube(g2, g1);

	gce::Vector3f32 half1 = g1->GetScale() * 0.5f;
	gce::Vector3f32 half2 = g2->GetScale() * 0.5f;

	gce::Vector3f32 penetration = { 0.f,0.f,0.f };

	float dx = pos2.x - pos1.x;
	float px = (half1.x + half2.x) - std::abs(dx);
	if (px <= 0.f) return { 0.f,0.f,0.f };
	penetration.x = (dx < 0.f) ? -px : px;

	float dy = pos2.y - pos1.y;
	float py = (half1.y + half2.y) - std::abs(dy);
	if (py <= 0.f) return { 0.f,0.f,0.f };
	penetration.y = (dy < 0.f) ? -py : py;

	float dz = pos2.z - pos1.z;
	float pz = (half1.z + half2.z) - std::abs(dz);
	if (pz <= 0.f) return { 0.f,0.f,0.f };
	penetration.z = (dz < 0.f) ? -pz : pz;

	return penetration;
}


void World::doCollision(Geometry& geometry, Entity ownId, gce::Vector3f32 penetration, Geometry& collideWith, Entity collideWithId) {
	switch (geometry.entityType)
	{
	case PADDLE:
		switch (collideWith.entityType)
		{
		case FLOOR:
			m_rigidBodies[ownId].velocity.y = 0;
			geometry.geometry->Translate({ 0.f, penetration.y, 0.f });
			break;
		case WALL:
		case GOAL:
			if (std::abs(penetration.x) > std::abs(penetration.z))
			{
				geometry.geometry->Translate({ penetration.x, 0.f, 0.f });
			} else {
				geometry.geometry->Translate({ 0.f, 0.f, penetration.z });
			}

			break;
		default:
			break;
		}

		break;
	case PUCK:
		switch (collideWith.entityType) 
		{
		case FLOOR:
			m_rigidBodies[ownId].velocity.y = 0;
			geometry.geometry->Translate({ 0.f, penetration.y, 0.f });
			break;
		case PADDLE: {
			sr::Geometry* puckGeom = m_geometries[ownId].geometry;
			sr::Geometry* paddleGeom = m_geometries[collideWithId].geometry;

			gce::Vector3f32 penetration = checkCollision(puckGeom, paddleGeom);

			gce::Vector3f32 penetrationXZ = { penetration.x, 0.f, penetration.z };
			float len = sqrtf(penetrationXZ.x * penetrationXZ.x + penetrationXZ.z * penetrationXZ.z);
			if (len < 1e-6f) len = 1e-6f;

			gce::Vector3f32 dir = { penetrationXZ.x / len, 0.f, penetrationXZ.z / len };

			float pushFactor = 0.5f;
			m_rigidBodies[ownId].velocity.x += dir.x * pushFactor * len;
			m_rigidBodies[ownId].velocity.z += dir.z * pushFactor * len;
			m_gameManager->movePuckInsideBoundries(this);
			break;
		}

		case WALL:
			if (std::abs(penetration.x) > std::abs(penetration.z))
			{
				geometry.geometry->Translate({ penetration.x, 0.f, 0.f });
				m_rigidBodies[ownId].velocity.x *= -1;
			}
			else {
				geometry.geometry->Translate({ 0.f, 0.f, penetration.z });
				m_rigidBodies[ownId].velocity.z *= -1;
			}
			break;
		case GOAL:
			if (collideWithId == m_playerGoalEntity)
			{
				m_gameManager->addScoreIa(1);
				m_gameManager->respawnPuck(this, m_playerEntity);
			} else {
				m_gameManager->addScorePlayer(1);
				m_gameManager->respawnPuck(this, m_iaEntity);
			}

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
		m_window->DrawText(*m_gameManager->m_playerScorePlayingText);
		m_window->DrawText(*m_gameManager->m_iaScorePlayingText);
		break;
	case PAUSED:
		m_window->DrawText(*m_gameManager->m_playerScorePlayingText);
		m_window->DrawText(*m_gameManager->m_iaScorePlayingText);
		m_window->DrawText(*m_gameManager->m_pauseText);
		m_window->DrawText(*m_gameManager->m_resumeText);
		m_window->DrawText(*m_gameManager->m_resetText);
		break;
	case FINISHED:
		m_window->DrawText(*m_gameManager->m_finishText);
		m_window->DrawText(*m_gameManager->m_iaScoreFinishText);
		m_window->DrawText(*m_gameManager->m_playerScoreFinishText);
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
		case EntityType::FLOOR:
		case EntityType::WALL:
		case EntityType::GOAL:
			m_cubePool.release(static_cast<sr::Cube*>(it->second.geometry));
			break;
		case EntityType::PADDLE:
		case EntityType::PUCK:
			m_cylinderPool.release(static_cast<sr::Cylinder*>(it->second.geometry));
			break;
		default:
			break;
		}
		m_geometries.erase(it);
	}

	m_rigidBodies.erase(entity);
	m_freeEntities.push(entity);
}


Entity World::getPlayerEntity() {
	return m_playerEntity;
}
Entity World::getIaEntity() {
	return m_iaEntity;
}
Entity World::getPuckEntiy() {
	return m_puckEntity;
}