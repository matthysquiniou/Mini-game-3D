#include "pch.h"

#include "GameManager.hpp"
#include "HelperFunction.hpp"
#include "World.hpp"
#include <includes/Core/Maths/Vector2.hpp>

GameManager::GameManager() {
	m_baseTime = 60.f;
	m_remainingTime = m_baseTime;
	m_baseSpawnTime = 2.f;
	m_currentSpawnTime = 0.f;
	m_score = 0;

	m_scorePlayingText = new sr::Text(std::to_wstring(m_score));
	m_scorePlayingText->SetPosition({ 20.f, 20.f });
	m_currentTimeText = new sr::Text(std::to_wstring(static_cast<int>(m_remainingTime)));
	m_currentTimeText->SetPosition({ 1840.f, 20.f });

	m_finishText = new sr::Text(L"Game over");
	m_finishText->SetPosition({ 960.f, 500.f  });
	m_scoreFinishText = new sr::Text(L"Final score : " + std::to_wstring(m_score));
	m_scoreFinishText->SetPosition({ 960.f, 540.f });

	m_pauseText = new sr::Text(L"Pause");
	m_pauseText->SetPosition({ 960.f, 500.f });
	m_resumeText = new sr::Text(L"Press C to resume");
	m_resumeText->SetPosition({ 960.f, 540.f });

	m_resetText = new sr::Text(L"Press R to Reset");
	m_resetText->SetPosition({ 960.f, 580.f });
	
	sr::Text* m_resetText;
	m_gameState = GameState::PLAYING;
}

void GameManager::addScore(int scoreToAdd) {
	m_score += scoreToAdd;
	m_scorePlayingText->SetText(std::to_wstring(m_score));
	m_scoreFinishText->SetText(L"Final score : " + std::to_wstring(m_score));
}

void GameManager::reset() {
	m_remainingTime = m_baseTime;
	m_currentSpawnTime = 0.f;
	m_score = 0;
	m_scorePlayingText->SetText(std::to_wstring(m_score));
	m_scoreFinishText->SetText(std::to_wstring(m_score));
	m_gameState = GameState::PLAYING;
}

void GameManager::update(float dt, World* world) {
	m_currentTimeText->SetText(std::to_wstring(static_cast<int>(m_remainingTime)));
	updateFinish(dt);
	updateSpawn(dt, world);
}

void GameManager::updateFinish(float dt) {
	m_remainingTime -= dt;
	if (m_remainingTime < 0) m_gameState = GameState::FINISHED;
}

void GameManager::updateSpawn(float dt, World* world) {
	m_currentSpawnTime += dt;
	if (m_currentSpawnTime > m_baseSpawnTime)
	{
		m_currentSpawnTime -= m_baseSpawnTime;

		sr::Sphere* collectible = world->m_sherePool.acquire();
		collectible->SetPosition(getRandomPointOnCircleXZ(5.f,15.f,{0.f,0.f,0.f}));
		collectible->SetScale({ 1.f, 1.f, 1.f });
		collectible->SetColor({ 1.f, 0.f, 0.f });

		Entity collectibleEntity = world->CreateEntity();

		world->m_geometries[collectibleEntity] = {collectible, EntityType::COLLECTIBLE};
		world->m_rigidBodies[collectibleEntity] = { 0.001f, { 0.f, 0.f, 0.f } };
	}
}