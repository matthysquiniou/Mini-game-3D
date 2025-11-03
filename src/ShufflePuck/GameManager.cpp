#include "pch.h"

#include "GameManager.hpp"
#include "HelperFunction.hpp"
#include "World.hpp"
#include <includes/Core/Maths/Vector2.hpp>
#include <Inputs/InputSystem.h>
#include <Inputs/InputsMethods.h>

GameManager::GameManager() {
	m_maxScore = 5;
	m_playerScore = 0;
	m_iaScore = 0;

	m_playerScorePlayingText = new sr::Text(std::to_wstring(m_playerScore));
	m_playerScorePlayingText->SetPosition({ 20.f, 20.f });
	m_iaScorePlayingText = new sr::Text(std::to_wstring(m_iaScore));
	m_iaScorePlayingText->SetPosition({ 1840.f, 20.f });

	m_finishText = new sr::Text(L"Game over");
	m_finishText->SetPosition({ 960.f, 500.f  });
	m_iaScoreFinishText = new sr::Text(L"Enemy score : " + std::to_wstring(m_iaScore));
	m_iaScoreFinishText->SetPosition({ 960.f, 540.f });
	m_playerScoreFinishText = new sr::Text(L"Your score : " + std::to_wstring(m_playerScore));
	m_playerScoreFinishText->SetPosition({ 960.f, 580.f });

	m_pauseText = new sr::Text(L"Pause");
	m_pauseText->SetPosition({ 960.f, 500.f });
	m_resumeText = new sr::Text(L"Press C to resume");
	m_resumeText->SetPosition({ 960.f, 540.f });

	m_resetText = new sr::Text(L"Press R to Reset");
	m_resetText->SetPosition({ 960.f, 620.f });
	
	sr::Text* m_resetText;
	m_gameState = GameState::PLAYING;

	sr::HideMouseCursor();
	sr::LockMouseCursor();
	sr::SetMousePosition({ 960,540 });

	sr::GetMouseDelta();
	m_mouseDelta = sr::GetMouseDelta();
}

void GameManager::addScorePlayer(int scoreToAdd) {
	m_playerScore += scoreToAdd;
	m_playerScorePlayingText->SetText(std::to_wstring(m_playerScore));
	m_playerScoreFinishText->SetText(L"Your score : " + std::to_wstring(m_playerScore));
}

void GameManager::addScoreIa(int scoreToAdd) {
	m_iaScore += scoreToAdd;
	m_iaScorePlayingText->SetText(std::to_wstring(m_iaScore));
	m_iaScoreFinishText->SetText(L"Enemy score : " + std::to_wstring(m_iaScore));
}

void GameManager::reset() {
	m_playerScore = 0;
	m_iaScore = 0;
	m_playerScorePlayingText->SetText(std::to_wstring(m_playerScore));
	m_playerScoreFinishText->SetText(L"Your score : " + std::to_wstring(m_playerScore));
	m_iaScorePlayingText->SetText(std::to_wstring(m_iaScore));
	m_iaScoreFinishText->SetText(L"Enemy score : " + std::to_wstring(m_iaScore));
	m_gameState = GameState::PLAYING;
	sr::HideMouseCursor();
	sr::LockMouseCursor();
	sr::SetMousePosition({ 960,540 });
	sr::GetMouseDelta();
}

void GameManager::update(float dt, World* world) {
	udpateIa(dt, world);
	m_mouseDelta = sr::GetMouseDelta();
	sr::SetMousePosition({ 960,540 });
	sr::GetMouseDelta();
	updateFinish();
}

void GameManager::udpateIa(float dt, World* world) {
	Entity puckEntity = world->getPuckEntiy();
	Entity iaEntity = world->getIaEntity();

	gce::Vector3f32 puckPosition = world->m_geometries[puckEntity].geometry->GetPosition();
	sr::Geometry* iaGeometry = world->m_geometries[iaEntity].geometry;
	gce::Vector3f32 iaPosition = iaGeometry->GetPosition();

	gce::Vector3f32 iaTranslation = { 0.f, 0.f, 0.f };

	if (iaPosition.x > puckPosition.x && std::abs(iaPosition.x - puckPosition.x) > 0.4f)
	{
		iaTranslation.x = -dt * 4.f;
	} else if (std::abs(iaPosition.x - puckPosition.x) > 0.4) {
		iaTranslation.x = dt * 4.f;
	}

	float zDistance = iaPosition.z - puckPosition.z;

	if (zDistance <= 8.f && zDistance > 0.f && iaPosition.z > 0.f && puckPosition.z >= 0.f)
	{
		iaTranslation.z = -dt * 4.f;
	} else if (zDistance < 0.f || iaPosition.z < 8.f) {
		iaTranslation.z = dt * 4.f;
	}

	iaGeometry->Translate(iaTranslation);
	
}

void GameManager::updateFinish() {
	if (m_playerScore >= m_maxScore || m_iaScore >= m_maxScore) {
		m_gameState = GameState::FINISHED;
		sr::ShowMouseCursor();
		sr::UnlockMouseCursor();
	}
}

void GameManager::respawnPuck(World* world, Entity side) {
	Entity puckEntity = world->getPuckEntiy();
	Entity playerEntity = world->getPlayerEntity();
	Entity iaEntity = world->getIaEntity();

	if (playerEntity == side) {
		world->m_geometries[puckEntity].geometry->SetPosition({ 0.f, 0.5f, -2.5f });
	} else {
		world->m_geometries[puckEntity].geometry->SetPosition({ 0.f, 0.5f, 2.5f });
	}
	world->m_rigidBodies[puckEntity].velocity = {0.f, 0.f, 0.f};

	world->m_geometries[playerEntity].geometry->SetPosition({ 0.f, 0.5f, -5.f });
	world->m_geometries[iaEntity].geometry->SetPosition({ 0.f, 0.5f, 5.f });
}

void GameManager::movePlayerInsideBoundries(World* world, float dt) {
	sr::Geometry* playerGeometry = world->m_geometries[world->getPlayerEntity()].geometry;
	playerGeometry->Translate({dt * m_mouseDelta.x,0.f,-dt * m_mouseDelta.y}); 

	gce::Vector3f32 pos = playerGeometry->GetPosition();

	const float minX = -4.5f;
	const float maxX = 4.5f;
	const float minZ = -9.5f;
	const float maxZ = -0.5f;

	if (pos.x < minX) pos.x = minX;
	if (pos.x > maxX) pos.x = maxX;
	if (pos.z < minZ) pos.z = minZ;
	if (pos.z > maxZ) pos.z = maxZ;

	playerGeometry->SetPosition(pos);
}

void GameManager::movePuckInsideBoundries(World* world) {
	sr::Geometry* puckGeometry = world->m_geometries[world->getPuckEntiy()].geometry;

	gce::Vector3f32 pos = puckGeometry->GetPosition();

	const float minX = -4.5f;
	const float maxX = 4.5f;
	const float minZ = -9.5f;
	const float maxZ = 9.5f;

	if (pos.x < minX) pos.x = minX;
	if (pos.x > maxX) pos.x = maxX;
	if (pos.z < minZ) pos.z = minZ;
	if (pos.z > maxZ) pos.z = maxZ;

	puckGeometry->SetPosition(pos);
}
