#include "stdafx.h"
#include "Platformer.h"
#include "Components/Components.h"
#include "Physx/PhysxManager.h"
#include "../OverlordEngine/Scenegraph/GameObject.h"
#include "Physx/PhysxProxy.h"
#include "../OverlordProject/Materials/ColorMaterial.h"
#include "Content/ContentManager.h"

#include "Level.h"

#include "../Week2/Character.h"

#include "../../Materials/DiffuseMaterial.h"

#include "MainCharacter.h"
#include "Hud.h"
#include "../../Materials/SkinnedDiffuseMaterial.h"
#include "Prefabs/Prefabs.h"
#include "../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../../Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../OverlordEngine/Button.h"
#include "Scenegraph/SceneManager.h"
#include "Graphics/SpriteRenderer.h"
#include "../../MenuScene.h"
#include "Diagnostics/DebugRenderer.h"
#include "Base/SoundManager.h"
#include "../../PostChromAb.h"


bool Platformer::m_ChangingFromMenu = false;

Platformer::Platformer():
	GameScene(L"Platformer"),
	m_MainGameState(MainGameState::PLAYING)
{
}


Platformer::~Platformer()
{
}


void Platformer::RemoveLifePlayer()
{
	m_pMainCharacter->RemoveLife();
}

void Platformer::SetDead()
{
	m_MainGameState = MainGameState::GAME_OVER;
	//m_pGameOverScreen->SetVisible(true);
}

void Platformer::Initialize(const GameContext & gameContext)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto bouncyMaterial = physX->createMaterial(0.5, 0.5, 1.0f);
    //GetPhysxProxy()->EnablePhysxDebugRendering(true);
	gameContext.pShadowMapper->SetLight({ -129.6139526f,100.1346436f,-40.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f }); //Area 2
	//gameContext.pShadowMapper->SetLight({ -259.6139526f,180.1346436f,-40.1850471f }, { 0.740129888f, -0.497205281f, 0.109117377f }); //Area 1
	//gameContext.pShadowMapper->SetLight({ -145.6139526f,110.1346436f,-130.1850471f }, { 0.740129888f, -0.497205281f, 0.109117377f }); //Area 3
	//Materials
	//*********

	//MAIN MATERIAL
	auto pDiffuseMaterial =  new DiffuseMaterial_Shadow();
	pDiffuseMaterial->SetDiffuseTexture(L"Resources/Textures/Platformer/High.png");
	gameContext.pMaterialManager->AddMaterial(pDiffuseMaterial, UINT(10));
	pDiffuseMaterial->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	
	//MATERIAL//
	auto pDiffuseMaterialEnemy = new  SkinnedDiffuseMaterial_Shadow();
	pDiffuseMaterialEnemy->SetDiffuseTexture(L"Resources/Textures/zombiegirl_diffuse.png");
	gameContext.pMaterialManager->AddMaterial(pDiffuseMaterialEnemy, UINT(1));
	pDiffuseMaterialEnemy->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());


	//CHARACTER
	m_Character = new Character();
	AddChild(m_Character);


	m_Character->GetTransform()->Translate(10.0f, 10.0f, 20.0f);

	m_pMainCharacter = new MainCharacter(m_Character);
	AddChild(m_pMainCharacter);

	m_pMainCharacter->PostInit();

	m_pMainCharacter->GetTransform()->Translate(0.0f, 2.0f, 20.0f);
	//GROUND
	m_Level = new Level();
	AddChild(m_Level);
	
	m_Level->PostInit();

	gameContext.pGameTime->Stop();


	//Splash Screens
	m_pPauseScreen = ContentManager::Load<TextureData>(L"./Resources/Textures/Game_UI/PauseScreenObjective.png");

	m_pGameOverTex = ContentManager::Load<TextureData>(L"./Resources/Textures/Game_UI/GameOver.png");

	m_pVictoryScreen = ContentManager::Load<TextureData>(L"./Resources/Textures/Game_UI/Victory.png");

	//Buttons
	m_pRestart = new Button(L"./Resources/Textures/Game_UI/B_restart.png", PxVec2(580, 450));
	m_pRestart->Initialize(gameContext);
	m_pRestart->SetPressed(true);
	m_pRestart->SetVisible(false);
	m_pRestart->SetActive(false);
	AddChild(m_pRestart);

	m_pExit = new Button(L"./Resources/Textures/Game_UI/B_Exit.png", PxVec2(580, 500));
	m_pExit->Initialize(gameContext);
	m_pExit->SetPressed(true);
	m_pExit->SetVisible(false);
	m_pExit->SetActive(false);
	AddChild(m_pExit);

	m_pBMainMenu = new Button(L"./Resources/Textures/Game_UI/B_MainMenu.png", PxVec2(580,400));
	m_pBMainMenu->Initialize(gameContext);
	m_pBMainMenu->SetPressed(true);
	m_pBMainMenu->SetVisible(false);
	m_pBMainMenu->SetActive(false);
	AddChild(m_pBMainMenu);

	m_pResume = new Button(L"./Resources/Textures/Game_UI/B_resume.png", PxVec2(580, 350));
	m_pResume->Initialize(gameContext);
	m_pResume->SetPressed(true);
	m_pResume->SetVisible(false);
	m_pResume->SetActive(false);
	AddChild(m_pResume);

	//skybox
	auto skybox = new SkyBoxPrefab();
	AddChild(skybox);

	gameContext.pInput->AddInputAction(InputAction(11, Released, VK_ESCAPE));
	gameContext.pInput->AddInputAction(InputAction(32, Released, VK_RETURN));

	//Post Material
	gameContext.pMaterialManager->AddMaterial_PP(new PostChromAb(), 2);
	//AddPostProcessingMaterial(2);

	//Sounds
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD_RESULT fmodres;

	fmodres = fmodSystem->createSound("./Resources/Sounds/Music/sad_game_over.wav", FMOD_DEFAULT,0, &m_pGameOverMusic);
	SoundManager::ErrorCheck(fmodres);

	fmodres = fmodSystem->createSound("./Resources/Sounds/Effects/button.wav", FMOD_DEFAULT, 0, &m_pBSound);
	SoundManager::ErrorCheck(fmodres);

	fmodres = fmodSystem->createSound("./Resources/Sounds/Music/Main Music.mp3", FMOD_LOOP_NORMAL, 0, &m_pMainMSound);
	SoundManager::ErrorCheck(fmodres);

	fmodres = fmodSystem->createSound("./Resources/Sounds/Music/Heavy_ConceptB.wav", FMOD_DEFAULT, 0, &m_pVictorySound);
	SoundManager::ErrorCheck(fmodres);


	fmodres = fmodSystem->playSound(m_pMainMSound, nullptr, true, &m_pChannelMainMusic);
	SoundManager::ErrorCheck(fmodres);
	//m_pChannelMainMusic->setPaused(true);

	fmodres = fmodSystem->playSound(m_pGameOverMusic, nullptr, true, &m_pGameOverChannel);
	SoundManager::ErrorCheck(fmodres);
	//m_pGameOverChannel->setPaused(true);

	fmodres = fmodSystem->playSound(m_pVictorySound, nullptr, true, &m_pVictoryChannel);
	SoundManager::ErrorCheck(fmodres);
	//m_pVictoryChannel->setPaused(true);

	m_pPortal = new GameObject();
	m_pPortal->GetTransform()->Translate(-100.817f, -3.1f, -41.4f);
	auto rigidBody2 = new RigidBodyComponent(true);

	m_pPortal->AddComponent(rigidBody2);

	std::shared_ptr<PxGeometry> geometry2(new PxBoxGeometry(5.0f, 5.0f, 5.0f));

	ColliderComponent *collider2 = new ColliderComponent(geometry2, *bouncyMaterial);

	collider2->EnableTrigger(true);
	m_pPortal->AddComponent(collider2);

	m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/Portal.png", 40);
	m_pParticleEmitter->SetVelocity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pParticleEmitter->SetMinSize(0.0f);
	m_pParticleEmitter->SetMaxSize(0.0f);
	m_pParticleEmitter->SetMinEnergy(0.0f);
	m_pParticleEmitter->SetMaxEnergy(0.0f);
	m_pParticleEmitter->SetMinSizeGrow(0.0f);
	m_pParticleEmitter->SetMaxSizeGrow(0.0f);
	m_pParticleEmitter->SetMinEmitterRange(0.0f);
	m_pParticleEmitter->SetMaxEmitterRange(0.0f);
	m_pPortal->AddComponent(m_pParticleEmitter);


	AddChild(m_pPortal);

}

void Platformer::Update(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	fmodSystem->update();
	FMOD_RESULT fmodres;
	if(!m_IsPaused && m_MainGameState != GAME_OVER && !m_IsPortalActivated )
	{
		fmodres = m_pChannelMainMusic->setPaused(false);
		SoundManager::ErrorCheck(fmodres);
		
		m_MainGameState = PLAYING;
	}

	if(m_ChangingFromMenu)
	{
		ResetLevel(gameContext);
		m_MainGameState = PLAYING;
		m_ChangingFromMenu = false;
	}
	if(m_Level->GetIsExitActivated())
	{
		m_Level->GetHud()->SetObjectiveActive(true);
		AddPostProcessingMaterial(2);
		m_IsPortalActivated = true;
		m_pParticleEmitter->SetVelocity(XMFLOAT3(0.0f, 0.3f, 0.0f));
		m_pParticleEmitter->SetMinSize(6.0f);
		m_pParticleEmitter->SetMaxSize(6.0f);
		m_pParticleEmitter->SetMinEnergy(2.0f);
		m_pParticleEmitter->SetMaxEnergy(2.0f);
		m_pParticleEmitter->SetMinSizeGrow(10.5f);
		m_pParticleEmitter->SetMaxSizeGrow(10.5f);
		m_pParticleEmitter->SetMinEmitterRange(0.5f);
		m_pParticleEmitter->SetMaxEmitterRange(0.5f);
		m_pParticleEmitter->SetColor(XMFLOAT4(229.0f / 255.0f, 184.0f / 255.0f, 136.f / 255.0f, 0.6f));		
	}
	
	m_pPortal->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(trigger);
		UNREFERENCED_PARAMETER(action);
		auto chara = dynamic_cast<Character*>(other);
		if (chara && m_IsPortalActivated)
		{		
			m_MainGameState = VICTORY;
		}
	});


	switch (m_MainGameState)
	{
	case Platformer::PLAYING:

		m_pChannelMainMusic->setPaused(false);
		ShadowRendererSet(gameContext);
		gameContext.pGameTime->Reset();
		m_pResume->SetActive(false);
		m_pExit->SetActive(false);
		m_pBMainMenu->SetActive(false);
		m_pRestart->SetActive(false);
		m_pExit->SetVisible(false);
		m_pRestart->SetVisible(false);
		m_pBMainMenu->SetVisible(false);
		m_pResume->SetVisible(false);
		if (gameContext.pInput->IsActionTriggered(11))
		{
			//pause everything
			gameContext.pGameTime->Stop();
			m_MainGameState = MainGameState::PAUSED;
			m_IsPaused = true;
		}
		break;
	case Platformer::PAUSED:
		m_IsReseted = false;
		m_pChannelMainMusic->setPaused(true);
		m_pResume->Update(gameContext);
		m_pExit->Update(gameContext);
		m_pBMainMenu->Update(gameContext);
		m_pRestart->Update(gameContext);
		m_pResume->SetActive(true);
		m_pExit->SetActive(true);
		m_pBMainMenu->SetActive(true);
		m_pRestart->SetActive(true);
		if (m_pResume->MouseClicked())
		{
			m_MainGameState = MainGameState::PLAYING;
			m_IsPaused = false;
		}
		if (m_pRestart->MouseClicked())
		{
			fmodres = fmodSystem->playSound(m_pBSound, nullptr, false, &m_pButtonChannel);
			ResetLevel(gameContext);
		}
		if (m_pBMainMenu->MouseClicked())
		{
			//ResetLevel(gameContext);
			fmodres = fmodSystem->playSound(m_pBSound, nullptr, false, &m_pButtonChannel);
			SceneManager::GetInstance()->SetActiveGameScene(L"MenuScene");			
		}
		if (m_pExit->MouseClicked())
		{
			fmodres = fmodSystem->playSound(m_pBSound, nullptr, false, &m_pButtonChannel);
			PostQuitMessage(1);
		}

		break;

	case Platformer::GAME_OVER:
		m_IsReseted = false;
		m_pChannelMainMusic->setPaused(true);
		//fmodres = fmodSystem->playSound(m_pGameOverMusic, nullptr, false, &m_pGameOverChannel);
		
		m_pGameOverChannel->setPaused(false);
		gameContext.pGameTime->Stop();
		m_pExit->Update(gameContext);
		m_pBMainMenu->Update(gameContext);
		m_pRestart->Update(gameContext);
		m_pExit->SetActive(true);
		m_pBMainMenu->SetActive(true);
		m_pRestart->SetActive(true);
		if (m_pRestart->MouseClicked())
		{
			fmodres = fmodSystem->playSound(m_pBSound, nullptr, false, &m_pButtonChannel);
			ResetLevel(gameContext);
		}
		if (m_pBMainMenu->MouseClicked())
		{
			//ResetLevel(gameContext);
			fmodres = fmodSystem->playSound(m_pBSound, nullptr, false, &m_pButtonChannel);
			SceneManager::GetInstance()->SetActiveGameScene(L"MenuScene");
				
		}
		if (m_pExit->MouseClicked())
		{
			fmodres = fmodSystem->playSound(m_pBSound, nullptr, false, &m_pButtonChannel);
			PostQuitMessage(1);
		}

		break;
	case Platformer::VICTORY:
		
		m_IsReseted = false;
		m_pChannelMainMusic->setPaused(true);
		//fmodres = fmodSystem->playSound(m_pVictorySound, nullptr, false, &m_pVictoryChannel);
		
		m_pVictoryChannel->setPaused(false);
		gameContext.pGameTime->Stop();
		m_pExit->Update(gameContext);
		m_pBMainMenu->Update(gameContext);
		m_pRestart->Update(gameContext);
		m_pExit->SetActive(true);
		m_pBMainMenu->SetActive(true);
		m_pRestart->SetActive(true);
		if (m_pRestart->MouseClicked())
		{
			fmodres = fmodSystem->playSound(m_pBSound, nullptr, false, &m_pButtonChannel);
			ResetLevel(gameContext);
		}
		if (m_pBMainMenu->MouseClicked())
		{
			//ResetLevel(gameContext);
			fmodres = fmodSystem->playSound(m_pBSound, nullptr, false, &m_pButtonChannel);
			SceneManager::GetInstance()->SetActiveGameScene(L"MenuScene");
			
		}
		if (m_pExit->MouseClicked())
		{
			fmodres = fmodSystem->playSound(m_pBSound, nullptr, false, &m_pButtonChannel);
			PostQuitMessage(1);
		}
		
		break;
	}
}

void Platformer::Draw(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if(m_IsPaused)
	{
		m_pExit->SetVisible(true);
		m_pRestart->SetVisible(true);
		m_pBMainMenu->SetVisible(true);
		m_pResume->SetVisible(true);
		m_pExit->Draw(gameContext);
		m_pRestart->Draw(gameContext);
		m_pBMainMenu->Draw(gameContext);
		m_pResume->Draw(gameContext);
		SpriteRenderer::GetInstance()->Draw(m_pPauseScreen, XMFLOAT2(-0, -0));
	}

	if(m_MainGameState == GAME_OVER)
	{
		m_pExit->SetVisible(true);
		m_pRestart->SetVisible(true);
		m_pBMainMenu->SetVisible(true);
		m_pExit->Draw(gameContext);
		m_pRestart->Draw(gameContext);
		m_pBMainMenu->Draw(gameContext);
		SpriteRenderer::GetInstance()->Draw(m_pGameOverTex, XMFLOAT2(-0, -0));
	}

	if (m_MainGameState == VICTORY)
	{
		m_pExit->SetVisible(true);
		m_pRestart->SetVisible(true);
		m_pBMainMenu->SetVisible(true);
		m_pExit->Draw(gameContext);
		m_pRestart->Draw(gameContext);
		m_pBMainMenu->Draw(gameContext);
		SpriteRenderer::GetInstance()->Draw(m_pVictoryScreen, XMFLOAT2(-0, -0));
	}

}

void Platformer::ResetLevel(const GameContext& gameContext)
{
	if (!m_IsReseted)
	{
		auto fmodSystem = SoundManager::GetInstance()->GetSystem();
		m_Level->ResetLevel();
		//std::cout << "Reset!" << std::endl;
		m_pGameOverChannel->stop();
		m_pGameOverChannel->setPosition(0, 0);
		fmodSystem->playSound(m_pGameOverMusic, nullptr, true, &m_pGameOverChannel);
		m_pVictoryChannel->stop();
		m_pVictoryChannel->setPosition(0, 0);
		fmodSystem->playSound(m_pVictorySound, nullptr, true, &m_pVictoryChannel);
		m_pChannelMainMusic->stop();
		m_pChannelMainMusic->setPosition(0,0);
		fmodSystem->playSound(m_pMainMSound, nullptr, false, &m_pChannelMainMusic);
		//m_pChannelMainMusic->setPaused(false);
		m_Character->GetTransform()->Translate(0.0f, 10.0f, 20.0f);
		m_pMainCharacter->GetTransform()->Translate(0.0f, 2.0f, 20.0f);
		m_pMainCharacter->SetLife(5);
		m_MainGameState = MainGameState::PLAYING;
		m_IsPaused = false;
		m_pExit->SetVisible(false);
		m_pRestart->SetVisible(false);
		m_pBMainMenu->SetVisible(false);
		m_pResume->SetVisible(false);
		m_pResume->SetActive(false);
		m_pExit->SetActive(false);
		m_pBMainMenu->SetActive(false);
		m_pRestart->SetActive(false);
		gameContext.pGameTime->Start();
		m_IsReseted = true;
		m_IsPortalActivated = false;
		m_pParticleEmitter->SetMinSize(0.0f);
		m_pParticleEmitter->SetMaxSize(0.0f);
		m_pParticleEmitter->SetMinEnergy(0.0f);
		m_pParticleEmitter->SetMaxEnergy(0.0f);
		m_pParticleEmitter->SetMinSizeGrow(0.0f);
		m_pParticleEmitter->SetMaxSizeGrow(0.0f);
		m_pParticleEmitter->SetMinEmitterRange(0.0f);
		m_pParticleEmitter->SetMaxEmitterRange(0.0f);
		RemovePostProcessingMaterial(2);
		m_Level->GetHud()->SetObjectiveActive(false);
	}
}

void Platformer::ShadowRendererSet(const GameContext& gameContext)
{
	if (m_Character->GetTransform()->GetPosition().x < -73.0f && m_Character->GetTransform()->GetPosition().z >2.0f )
	{
		gameContext.pShadowMapper->SetLight({ -129.6139526f,100.1346436f,-40.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });//Area 2
	}
	else if ( m_Character->GetTransform()->GetPosition().x > -100.0f  && m_Character->GetTransform()->GetPosition().z < -67.4f)
	{
		gameContext.pShadowMapper->SetLight({ -145.6139526f,110.1346436f,-130.1850471f }, { 0.740129888f, -0.497205281f, 0.109117377f }); //Area 3
	}
	else
	{
		gameContext.pShadowMapper->SetLight({ -259.6139526f,180.1346436f,-40.1850471f }, { 0.740129888f, -0.497205281f, 0.109117377f }); //Area 1
	}
}

