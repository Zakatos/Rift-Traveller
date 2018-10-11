#pragma once
#include "Scenegraph/GameObject.h"
#include "Scenegraph/GameScene.h"


class Character;
class Level;
class MainCharacter;
class Score;
class SpriteComponent;
class Button;
class ParticleEmitterComponent;


class Platformer : public GameScene
{
public:

	Platformer(void);
	virtual ~Platformer(void);

	Level* GetLevel() { return m_Level; }

	void RemoveLifePlayer();

	MainCharacter* GetMainCharacter() { return m_pMainCharacter; }
	bool GetIsPortalActivated() const { return m_IsPortalActivated; }
	void SetDead();
	void ShadowRendererSet(const GameContext & gameContext);
	static bool m_ChangingFromMenu;
protected:
	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

	
private:
	Level * m_Level;
	Character * m_Character;
	MainCharacter * m_pMainCharacter;
	SpriteComponent * m_pControls;
	Button * m_pRestart = nullptr;
	Button * m_pExit = nullptr;
	Button * m_pBMainMenu = nullptr;
	Button * m_pResume = nullptr;
	TextureData* m_pPauseScreen = nullptr;
	TextureData* m_pGameOverTex = nullptr;
	TextureData* m_pVictoryScreen = nullptr;
	Score * m_pScore;
	GameObject* m_pPortal;
	ParticleEmitterComponent* m_pParticleEmitter = nullptr;
	bool m_IsPaused = false;
	bool m_IsReseted = false;
	
	// sounds
	FMOD::Sound *m_pGameOverMusic = nullptr, *m_pBSound = nullptr, *m_pMainMSound = nullptr, *m_pVictorySound = nullptr;
	FMOD::Channel *m_pGameOverChannel = nullptr, *m_pButtonChannel = nullptr, *m_pChannelMainMusic = nullptr, *m_pVictoryChannel = nullptr;
	bool m_Started = false;
	bool m_Paused = false;
	bool m_IsPortalActivated = false;
	enum MainGameState
	{
		PLAYING,
		PAUSED,
		GAME_OVER,
		MAIN_MENU,
		CONTROLS,
		VICTORY
	};

	MainGameState m_MainGameState;

	void ResetLevel(const GameContext& gameContext);

	float m_TotalControlTime = 0.5f;
	float m_CurrControlTime = 0.0f;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	Platformer(const Platformer &obj);
	Platformer& operator=(const Platformer& obj);
};

