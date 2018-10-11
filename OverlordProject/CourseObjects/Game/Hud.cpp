#include "stdafx.h"
#include "Hud.h"
#include <Graphics/TextRenderer.h>
#include <Base/OverlordGame.h>
#include <Content/ContentManager.h>
#include <Components/Components.h>
#include <Graphics/SpriteRenderer.h>
#include <Graphics/SpriteFont.h>
#include <Graphics/TextureData.h>

Hud::Hud()
{
}


Hud::~Hud()
{
}

void Hud::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pRetroFont = ContentManager::Load<SpriteFont>(L"./Resources/Font/Retro_32.fnt");

	auto pSprite = new SpriteComponent(L"./Resources/Textures/Game_UI/Coin.png");
	AddComponent(pSprite);

	pSprite->SetPosition({ 50.0f,0.0f });
	pSprite->SetVisible(true);

	auto pSprite2 = new SpriteComponent(L"./Resources/Textures/Game_UI/LolaHead.png");
	AddComponent(pSprite2);

	pSprite2->SetPosition({ 1160.0f,-35.0f });
	pSprite2->SetVisible(true);

	m_pKeysSprite = new SpriteComponent(L"./Resources/Textures/Game_UI/key.png");
	AddComponent(m_pKeysSprite);

	m_pKeysSprite->SetPosition({ 1160.0f,600.0f });
	m_pKeysSprite->SetVisible(false);


}

void Hud::Update(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_CurrentTimeShown += gameContext.pGameTime->GetElapsed();
	m_CurrentTimeObjShown += gameContext.pGameTime->GetElapsed();
	if (m_CurrentTimeShown < m_MaxTimeShown)
	{
		m_pKeysSprite->SetVisible(true);
	}
	else
	{
		m_pKeysSprite->SetVisible(false);
	}
	if(m_IsCompleted)
	{
		m_CurrentTimeObjShown = 0.0f;
	}
}

void Hud::Draw(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	wstringstream scoreCoins;
	scoreCoins <<   m_NumCoins << "X" ;

	TextRenderer::GetInstance()->DrawText(m_pRetroFont, scoreCoins.str(), { 10.0f,10.0f }, (XMFLOAT4)Colors::White);

	wstringstream score;
	score << "Score:"<< m_Score;
	TextRenderer::GetInstance()->DrawText(m_pRetroFont, score.str(), { 580.0f,0.0f }, (XMFLOAT4)Colors::White);

	wstringstream lives;
	lives << m_Lives << "X";
	TextRenderer::GetInstance()->DrawText(m_pRetroFont, lives.str(), { 1150.0f,10.0f }, (XMFLOAT4)Colors::White);

	if (m_CurrentTimeShown < m_MaxTimeShown)
	{
		wstringstream keys;
		keys << m_Keys << "X";
		TextRenderer::GetInstance()->DrawText(m_pRetroFont, keys.str(), { 1110.0f,645.0f }, (XMFLOAT4)Colors::White);
	}
	if (m_CurrentTimeObjShown < m_MaxTimeShown && m_Keys >= 3)
	{
		wstringstream objective;
		objective << "The portal is activated!Find it to finish the level!";
		TextRenderer::GetInstance()->DrawText(m_pRetroFont, objective.str(), { 0.0f,50.0f }, (XMFLOAT4)Colors::White);
	}
}

void Hud::IncreaseScoreCoins()
{
	++m_NumCoins;
}

void Hud::SetScoreCoins(int newScore)
{
	m_NumCoins = newScore;
}

void Hud::IncreaseScore()
{
	m_Score += 100;
}

void Hud::SetScore(int newScore)
{
	m_Score = newScore;
}

void Hud::DecreaseLives()
{
	--m_Lives;
}

void Hud::SetLives(int newLives)
{
	m_Lives = newLives;
}

void Hud::IncreaseKeys()
{
	m_CurrentTimeShown = 0.0f;
	++m_Keys;
}

void Hud::SetKeys(int keysnumber)
{
	m_Keys = keysnumber;
}

void  Hud::SetObjectiveTime(float time)
{
	m_CurrentTimeObjShown = time;
}