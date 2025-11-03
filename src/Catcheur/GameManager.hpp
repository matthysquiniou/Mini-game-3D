#pragma once
#include "GameState.hpp"
#include <Text.h>

class World;

class GameManager {
public:
	GameManager();
	void update(float dt, World* world);
	void addScore(int scoreToAdd);
	void reset();

private:
	void updateSpawn(float dt, World* world);
	void updateFinish(float dt);

public:
	GameState m_gameState;
	int m_score;
	float m_remainingTime;
	sr::Text* m_currentTimeText;
	sr::Text* m_scorePlayingText;
	sr::Text* m_scoreFinishText;
	sr::Text* m_resumeText;
	sr::Text* m_resetText;
	sr::Text* m_pauseText;
	sr::Text* m_finishText;

private:
	float m_baseTime;
	float m_baseSpawnTime;
	float m_currentSpawnTime;

};