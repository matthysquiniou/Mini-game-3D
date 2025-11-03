#pragma once
#include "GameState.hpp"
#include <Text.h>
#include "Entity.hpp"
#include <random>

class World;

class GameManager {
public:
	GameManager();
	void update(float dt, World* world);
	void finishGame(World* world, Entity playerEntity, Entity otherEntity);
	void reset();
	void changeDirectionWallSpawn(World* world, Entity playerEntity);

private:
	void updateSpawnWall(float dt, World* world);
	void spawnWall(World* world, Entity entityPlayer);
	void mergeSpawnedWall(World* world, Entity newWallEntity);
	void updateIaPlayer(World* world);

public:
	GameState m_gameState;
	sr::Text* m_resumeText;
	sr::Text* m_resetText;
	sr::Text* m_pauseText;
	sr::Text* m_finishText;

private:
	bool m_player1FirstWallPlaced = false;
	bool m_player2FirstWallPlaced = false;
	gce::Vector3f32 m_last_player1WallNearPoint;
	gce::Vector3f32 m_last_player2WallNearPoint;
	float m_baseSpawnTime;
	float m_currentSpawnTime;
	std::uniform_int_distribution<int> m_distribution;
	std::mt19937 m_gen;
};