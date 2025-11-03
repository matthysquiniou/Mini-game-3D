#include "pch.h"

#include "GameManager.hpp"
#include "HelperFunction.hpp"
#include "World.hpp"
#include <includes/Core/Maths/Vector2.hpp>

GameManager::GameManager() {
	m_baseSpawnTime = 0.0166f;
	m_currentSpawnTime = 0.f;

	m_finishText = new sr::Text(L"");
	m_finishText->SetPosition({ 960.f, 500.f  });

	m_pauseText = new sr::Text(L"Pause");
	m_pauseText->SetPosition({ 960.f, 500.f });
	m_resumeText = new sr::Text(L"Press C to resume");
	m_resumeText->SetPosition({ 960.f, 580.f });

	m_resetText = new sr::Text(L"Press R to Reset");
	m_resetText->SetPosition({ 960.f, 540.f });
	
	m_gameState = GameState::PLAYING;
	std::random_device rd;
	m_gen = std::mt19937(rd());
	m_distribution = std::uniform_int_distribution<int>(0, 1);
}

void GameManager::reset() {
	m_currentSpawnTime = 0.f;
	m_player1FirstWallPlaced = false;
	m_player2FirstWallPlaced = false;
	m_gameState = GameState::PLAYING;
}

void GameManager::update(float dt, World* world) {
	updateSpawnWall(dt, world);
	updateIaPlayer(world);
}

void GameManager::updateSpawnWall(float dt, World* world) {
	m_currentSpawnTime += dt;
	if (m_currentSpawnTime > m_baseSpawnTime)
	{
		m_currentSpawnTime -= m_baseSpawnTime;
		Entity Player1Entity = world->GetPlayer1Entity();
		Entity Player2Entity = world->GetPlayer2Entity();

		spawnWall(world, Player1Entity);
		spawnWall(world, Player2Entity);
	}
}

void GameManager::changeDirectionWallSpawn(World* world, Entity playerEntity) {

	spawnWall(world, playerEntity);
	Entity Player1Entity = world->GetPlayer1Entity();
	Player1Entity == playerEntity ? m_player1FirstWallPlaced = false : m_player2FirstWallPlaced = false;
}

void GameManager::finishGame(World* world, Entity playerEntity, Entity otherEntity) {
	Entity player1 = world->GetPlayer1Entity();
	Entity player2 = world->GetPlayer2Entity();

	if (otherEntity == player1 || otherEntity == player2) {
		m_finishText->SetText(L"Both player died");
	} else if (playerEntity == player1) {
		m_finishText->SetText(L"Player 2 won");
	} else {
		m_finishText->SetText(L"Player 1 won");
	}

	m_gameState = GameState::FINISHED;
}

void GameManager::spawnWall(World* world, Entity entityPlayer) {
	RigidBody playerRigidBody = world->m_rigidBodies[entityPlayer];
	gce::Vector3f32 playerPosition = world->m_geometries[entityPlayer].geometry->GetPosition();
	gce::Vector3f32 newNearPointPlayer = { -playerRigidBody.velocity.x * 6.f + playerPosition.x, 0.f, -playerRigidBody.velocity.z * 6.f + playerPosition.z };
	Entity wallEntity = world->CreateEntity();
	sr::Cube* wall = world->m_cubePool.acquire();

	if (world->GetPlayer1Entity() == entityPlayer)
	{
		if (m_player1FirstWallPlaced == false) {
			m_last_player1WallNearPoint = newNearPointPlayer;
			m_player1FirstWallPlaced = true;
		}
		wall->SetPosition({
				(newNearPointPlayer.x + m_last_player1WallNearPoint.x) * 0.5f,
				0.f,
				(newNearPointPlayer.z + m_last_player1WallNearPoint.z) * 0.5f
			});
		float scaleX = std::abs(newNearPointPlayer.x - m_last_player1WallNearPoint.x);
		float scaleZ = std::abs(newNearPointPlayer.z - m_last_player1WallNearPoint.z);
		wall->SetScale({
			scaleX == 0.f ? 0.1f : scaleX,
			2.f,
			scaleZ == 0.f ? 0.1f : scaleZ
			});
		wall->SetColor({ 0.f,0.f,1.f });
		world->m_geometries[wallEntity] = { wall, EntityType::WALL };

		m_last_player1WallNearPoint = newNearPointPlayer;
	} else {
		if (m_player2FirstWallPlaced == false) {
			m_last_player2WallNearPoint = newNearPointPlayer;
			m_player2FirstWallPlaced = true;
		}
		wall->SetPosition({
				(newNearPointPlayer.x + m_last_player2WallNearPoint.x) * 0.5f,
				0.f,
				(newNearPointPlayer.z + m_last_player2WallNearPoint.z) * 0.5f
			});
		float scaleX = std::abs(newNearPointPlayer.x - m_last_player2WallNearPoint.x);
		float scaleZ = std::abs(newNearPointPlayer.z - m_last_player2WallNearPoint.z);
		wall->SetScale({
			scaleX == 0.f ? 0.1f : scaleX,
			2.f,
			scaleZ == 0.f ? 0.1f : scaleZ
			});
		wall->SetColor({ 0.f,1.f,0.f });
		world->m_geometries[wallEntity] = { wall, EntityType::WALL };

		m_last_player2WallNearPoint = newNearPointPlayer;
	}

	mergeSpawnedWall(world, wallEntity);
}




void GameManager::mergeSpawnedWall(World* world, Entity newWallEntity) {
	sr::Geometry* wall2 = world->m_geometries[newWallEntity].geometry;
	for (auto g1 = world->m_geometries.begin(); g1 != world->m_geometries.end(); ++g1) {
		if (g1->second.entityType != EntityType::WALL || g1->first == newWallEntity) continue;
		sr::Geometry* wall1 = g1->second.geometry;

		gce::Vector3f32 position1 = wall1->GetPosition();
		gce::Vector3f32 position2 = wall2->GetPosition();
		if (position1.x == position2.x) {
			float scaleZ1 = wall1->GetScale().z * 0.5f;
			float scaleZ2 = wall2->GetScale().z * 0.5f;
			float dz = position2.z - position1.z;
			float sz = scaleZ1 + scaleZ2;
			float pz = sz - std::abs(dz);
			if (pz < -0.2f) continue;
			position1.z = (position1.z * scaleZ1 + position2.z * scaleZ2) / sz;
			gce::Vector3f32 newScale = wall1->GetScale();
			newScale.z = gce::Max(position1.z + scaleZ1, position2.z + scaleZ2) - gce::Min(position1.z - scaleZ1, position2.z - scaleZ2);
			if (pz < 0.f) newScale.z -= pz;

			sr::Cube* mergedWall = world->m_cubePool.acquire();
			mergedWall->SetPosition(position1);
			mergedWall->SetScale(newScale);
			mergedWall->SetColor(wall1->GetColor());
			Entity mergedWallEntity = world->CreateEntity();
			world->m_geometries[mergedWallEntity] = { mergedWall, EntityType::WALL };
			world->DestroyEntity(newWallEntity);
			world->DestroyEntity(g1->first);
			return;
		} else if (position1.z == position2.z) {
			float scaleX1 = wall1->GetScale().x * 0.5f;
			float scaleX2 = wall2->GetScale().x * 0.5f;
			float dx = position2.x - position1.x;
			float sx = scaleX1 + scaleX2;
			float px = sx - std::abs(dx);
			if (px < -0.2f) continue;
			position1.x = (position1.x * scaleX1 + position2.x * scaleX2) / sx;
			gce::Vector3f32 newScale = wall1->GetScale();
			newScale.x = gce::Max(position1.x + scaleX1, position2.x + scaleX2) - gce::Min(position1.x - scaleX1, position2.x - scaleX2);
			if (px < 0.f) newScale.x -= px;
			sr::Cube* mergedWall = world->m_cubePool.acquire();
			mergedWall->SetPosition(position1);
			mergedWall->SetScale(newScale);
			mergedWall->SetColor(wall1->GetColor());
			Entity mergedWallEntity = world->CreateEntity();
			world->m_geometries[mergedWallEntity] = {mergedWall, EntityType::WALL};
			world->DestroyEntity(newWallEntity);
			world->DestroyEntity(g1->first);
			return;
		} else continue;
	}
}

void GameManager::updateIaPlayer(World* world) {
	float maxDistance = 5.f;
	Entity iaEntity = world->GetPlayer2Entity();
	sr::Geometry* ia = world->m_geometries[iaEntity].geometry;
	gce::Vector3f32 iaPosition = ia->GetPosition();
	gce::Vector3f32 iaScale = ia->GetScale();
	RigidBody& iaRigidBody = world->m_rigidBodies[iaEntity];
	gce::Vector3f32 newVelocity = iaRigidBody.velocity;

	bool movingX = std::abs(iaRigidBody.velocity.x) > 0.f;

	for (auto& [entity, data] : world->m_geometries) {
		if (data.entityType != EntityType::WALL) continue;
		sr::Geometry* wall = data.geometry;
		gce::Vector3f32 wallPos = wall->GetPosition();
		gce::Vector3f32 wallScale = wall->GetScale();

		if (movingX) {
			float dx = wallPos.x - iaPosition.x;
			if ((iaRigidBody.velocity.x > 0.f && dx <= 0.f) ||
				(iaRigidBody.velocity.x < 0.f && dx >= 0.f) ||
				std::abs(dx) > maxDistance)
				continue;

			float zMinPlayer = iaPosition.z - iaScale.z * 0.5f;
			float zMaxPlayer = iaPosition.z + iaScale.z * 0.5f;
			float zMinWall = wallPos.z - wallScale.z * 0.5f;
			float zMaxWall = wallPos.z + wallScale.z * 0.5f;
			if (zMaxPlayer < zMinWall || zMinPlayer > zMaxWall)
				continue; 

			newVelocity.x = 0.f;
			newVelocity.z = m_distribution(m_gen) ? -0.1f : 0.1f;
			changeDirectionWallSpawn(world, iaEntity);
			break;

		}
		else { 
			float dz = wallPos.z - iaPosition.z;
			if ((iaRigidBody.velocity.z > 0.f && dz <= 0.f) ||
				(iaRigidBody.velocity.z < 0.f && dz >= 0.f) ||
				std::abs(dz) > maxDistance)
				continue;

			float xMinPlayer = iaPosition.x - iaScale.x * 0.5f;
			float xMaxPlayer = iaPosition.x + iaScale.x * 0.5f;
			float xMinWall = wallPos.x - wallScale.x * 0.5f;
			float xMaxWall = wallPos.x + wallScale.x * 0.5f;
			if (xMaxPlayer < xMinWall || xMinPlayer > xMaxWall)
				continue;

			newVelocity.z = 0.f;
			newVelocity.x = m_distribution(m_gen) == 1 ? -0.1f : 0.1f;
			changeDirectionWallSpawn(world, iaEntity);
			break;
		}
	}

	iaRigidBody.velocity = newVelocity;

}