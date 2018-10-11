#include "stdafx.h"
#include "MenuScene.h"
#include <Physx/PhysxProxy.h>
#include "Prefabs/FixedCamera.h"
#include "Components/Components.h"
#include "Graphics/SpriteRenderer.h"
#include "Content/ContentManager.h"
#include "Graphics/TextRenderer.h"
#include "./PostVignette.h"
#include "../OverlordEngine/Button.h"
#include "Scenegraph/SceneManager.h"
#include "Base/SoundManager.h"
#include "Graphics/SpriteFont.h"
#include "CourseObjects/Game/Platformer.h"

MenuScene::MenuScene() :
	GameScene(L"MenuScene"),
	m_State(MAINSCREEN)
{
}

MenuScene::~MenuScene()
{
	 /*SafeDelete( m_pStartButton);
	 m_pStartButton = nullptr;
	 SafeDelete(m_pExitButton);
	 m_pExitButton = nullptr;
	 SafeDelete(m_pNextButton);
	 m_pNextButton = nullptr;	*/
}
void MenuScene::Initialize(const GameContext& gameContext)
{
	//GetPhysxProxy()->EnablePhysxDebugRendering(true);

	UNREFERENCED_PARAMETER(gameContext);
	m_pSpriteFont = ContentManager::Load<SpriteFont>(L"Resources/Font/Retro_32.fnt");

	m_pBackDrop = ContentManager::Load<TextureData>(L"./Resources/Textures/Game_UI/MainMenu.png");

	m_pControls = ContentManager::Load<TextureData>(L"./Resources/Textures/Game_UI/ControlsObjective.png");
	
	//cam
	auto pfixedCamera = new FixedCamera();
	AddChild(pfixedCamera);
	pfixedCamera->GetComponent<TransformComponent>()->Translate(0, 25, -1);
	pfixedCamera->GetComponent<TransformComponent>()->Rotate(0, 0, 0);
	pfixedCamera->GetComponent<CameraComponent>()->UseOrthographicProjection();
	pfixedCamera->GetComponent<CameraComponent>()->SetOrthoSize(50);
	auto gameScene = pfixedCamera->GetScene();
	gameScene->SetActiveCamera(pfixedCamera->GetComponent<CameraComponent>());


	//MENU BUTTONS
	m_pStartButton = new Button(L"./Resources/Textures/Game_UI/B_Start.png", PxVec2(580, 230));
	m_pStartButton->Initialize(gameContext);
	m_pStartButton->SetPressed(true);
	AddChild(m_pStartButton);

	m_pExitButton = new Button(L"./Resources/Textures/Game_UI/B_Exit.png", PxVec2(580, 450));
	m_pExitButton->Initialize(gameContext);
	m_pExitButton->SetPressed(true);
	AddChild(m_pExitButton);

	m_pNextButton = new Button(L"./Resources/Textures/Game_UI/B_Next.png", PxVec2(900, 600));
	m_pNextButton->Initialize(gameContext);
	m_pNextButton->SetPressed(true);
	AddChild(m_pNextButton);

	//SOUND

	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD_RESULT fmodres;
	
	fmodres = fmodSystem->createSound("./Resources/Sounds/Music/Menu Music.mp3", FMOD_LOOP_NORMAL, 0, &m_pMenuMusic);
	SoundManager::ErrorCheck(fmodres);

	fmodres = fmodSystem->createSound("./Resources/Sounds/Effects/button-04.wav", FMOD_LOOP_OFF, 0, &m_pButtonSound);
	SoundManager::ErrorCheck(fmodres);

	fmodres = fmodSystem->playSound(m_pMenuMusic, 0, false, &m_pMChannel);

}

void MenuScene::Update(const GameContext& gameContext)
{
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	fmodSystem->update();
	FMOD_RESULT fmodres{};
	m_pMChannel->setPaused(false);
	switch (m_State)
	{
	case MenuScene::MAINSCREEN:
		m_pStartButton->Update(gameContext);
		m_pExitButton->Update(gameContext);
		if (m_pStartButton->MouseClicked())
		{
			fmodres = fmodSystem->playSound(m_pButtonSound, m_pChannelGroup, false, &m_pBChannel);
			m_State = CONTROLS;
		}
		if (m_pExitButton->MouseClicked())
		{

			fmodres = fmodSystem->playSound(m_pButtonSound, m_pChannelGroup, false, &m_pBChannel);
			PostQuitMessage(1);
		}
		break;
	case MenuScene::CONTROLS:
		
		if (m_pNextButton->MouseClicked())
		{
			m_pMChannel->setPaused(true);
			fmodres = fmodSystem->playSound(m_pButtonSound, m_pChannelGroup, false, &m_pBChannel);
			if (!m_IsFirstTime)
			{
				Platformer::m_ChangingFromMenu = true;
			}
			SceneManager::GetInstance()->SetActiveGameScene(L"Platformer");
			m_IsFirstTime = false;
			m_State = MAINSCREEN;
		}
		
		break;
	/*default:
		m_State = MAINSCREEN;
		break;*/
	}
}

void MenuScene::Draw(const GameContext& gameContext)
{
	if (m_State == MAINSCREEN)
	{
		m_pStartButton->Draw(gameContext);
		m_pExitButton->Draw(gameContext);
		SpriteRenderer::GetInstance()->Draw(m_pBackDrop, XMFLOAT2(0, 0));
	}
	else if (m_State == CONTROLS)
	{
		m_pNextButton->Draw(gameContext);
		SpriteRenderer::GetInstance()->Draw(m_pControls, XMFLOAT2(0, 0));
	}
}

