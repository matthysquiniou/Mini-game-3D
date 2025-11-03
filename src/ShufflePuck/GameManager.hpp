#pragma once
#include "GameState.hpp"
#include "Entity.hpp"
#include <Text.h>

class World;

class GameManager {
public:
	GameManager();
	void update(float dt, World* world);
	void addScorePlayer(int scoreToAdd);
	void addScoreIa(int scoreToAdd);
	void respawnPuck(World* world, Entity side);
	void movePlayerInsideBoundries(World* world, float dt);
	void movePuckInsideBoundries(World* world);
	void reset();

private:
	void updateFinish();
	void udpateIa(float dt, World* world);

public:
	gce::Vector2f32 m_mouseDelta;
	GameState m_gameState;
	sr::Text* m_playerScorePlayingText;
	sr::Text* m_iaScorePlayingText;
	sr::Text* m_playerScoreFinishText;
	sr::Text* m_iaScoreFinishText;
	sr::Text* m_resumeText;
	sr::Text* m_resetText;
	sr::Text* m_pauseText;
	sr::Text* m_finishText;

private:
	int m_maxScore;
	int m_playerScore;
	int m_iaScore;
};