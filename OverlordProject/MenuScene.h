#pragma once
#include "Scenegraph/GameScene.h"

class SpriteComponent;
class SpriteFont;
class Button;

class MenuScene : public GameScene
{
public:
	MenuScene();
	virtual ~MenuScene();
	
protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:
	 Button * m_pStartButton = nullptr;
	 Button* m_pExitButton = nullptr;
	 Button* m_pNextButton = nullptr;
	TextureData* m_pBackDrop = nullptr;
	TextureData* m_pControls = nullptr;
	SpriteFont* m_pSpriteFont = nullptr;
	FMOD::Sound *m_pButtonSound = nullptr, *m_pMenuMusic = nullptr;
	FMOD::Channel *m_pBChannel = nullptr, *m_pMChannel = nullptr;
	FMOD::ChannelGroup *m_pChannelGroup = nullptr;
	bool m_ShowControls = false;
	bool m_IsFirstTime = true;
	enum MenuState
	{
		MAINSCREEN,
		CONTROLS	
	};
	MenuState m_State;
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MenuScene(const MenuScene &obj);
	MenuScene& operator=(const MenuScene& obj);
};

