//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "GameScene.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "../Diagnostics/Logger.h"
#include "../Prefabs/Prefabs.h"
#include "../Components/Components.h"
#include "../Diagnostics/DebugRenderer.h"
#include "../Physx/PhysxProxy.h"
#include <SoundManager.h>
#include "../Graphics/SpriteRenderer.h"
#include "../Graphics/TextRenderer.h"
#include "../PostProcessingMaterial.h"
#include "../Base/OverlordGame.h"
#include "../Graphics/RenderTarget.h"


GameScene::GameScene(wstring sceneName) :
	m_pChildren(vector<GameObject*>()),
	m_GameContext(GameContext()),
	m_IsInitialized(false),
	m_SceneName(sceneName),
	m_pDefaultCamera(nullptr),
	m_pActiveCamera(nullptr),
	m_pPhysxProxy(nullptr),
	m_PostProcessingMaterialIds(vector<UINT>())
{
}


GameScene::~GameScene(void)
{
	SafeDelete(m_GameContext.pGameTime);
	SafeDelete(m_GameContext.pInput);
	SafeDelete(m_GameContext.pMaterialManager);
	SafeDelete(m_GameContext.pShadowMapper);
	for (GameObject* pChild : m_pChildren)
	{
		SafeDelete(pChild);
	}

	SafeDelete(m_pPhysxProxy);
}

void GameScene::AddChild(GameObject* obj)
{
#if _DEBUG
	if (obj->m_pParentScene)
	{
		if (obj->m_pParentScene == this)
			Logger::LogWarning(L"GameScene::AddChild > GameObject is already attached to this GameScene");
		else
			Logger::LogWarning(L"GameScene::AddChild > GameObject is already attached to another GameScene. Detach it from it's current scene before attaching it to another one.");

		return;
	}

	if (obj->m_pParentObject)
	{
		Logger::LogWarning(L"GameScene::AddChild > GameObject is currently attached to a GameObject. Detach it from it's current parent before attaching it to another one.");
		return;
	}
#endif

	obj->m_pParentScene = this;
	obj->RootInitialize(m_GameContext);
	m_pChildren.push_back(obj);
}

void GameScene::RemoveChild(GameObject* obj, bool deleteObject)
{
	auto it = find(m_pChildren.begin(), m_pChildren.end(), obj);

#if _DEBUG
	if (it == m_pChildren.end())
	{
		Logger::LogWarning(L"GameScene::RemoveChild > GameObject to remove is not attached to this GameScene!");
		return;
	}
#endif

	m_pChildren.erase(it);
	if (deleteObject)
	{
		delete obj;
		obj = nullptr;
	}
	else
		obj->m_pParentScene = nullptr;
}

void GameScene::RootInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{

	if (m_IsInitialized)
		return;

	//Create DefaultCamera
	auto freeCam = new FreeCamera();
	freeCam->SetRotation(30, 0);
	freeCam->GetTransform()->Translate(0, 50, -80);
	AddChild(freeCam);
	m_pDefaultCamera = freeCam->GetComponent<CameraComponent>();
	m_pActiveCamera = m_pDefaultCamera;
	m_GameContext.pCamera = m_pDefaultCamera;

	//Create GameContext
	m_GameContext.pGameTime = new GameTime();
	m_GameContext.pGameTime->Reset();
	m_GameContext.pGameTime->Stop();

	m_GameContext.pInput = new InputManager();
	m_GameContext.pInput->Initialize();

	m_GameContext.pMaterialManager = new MaterialManager();

	m_GameContext.pDevice = pDevice;
	m_GameContext.pDeviceContext = pDeviceContext;

	m_GameContext.pShadowMapper = new ShadowMapRenderer();
	m_GameContext.pShadowMapper->Init(m_GameContext);

	// Initialize Physx
	m_pPhysxProxy = new PhysxProxy();
	m_pPhysxProxy->Initialize(this);

	//User-Scene Initialize
	Initialize(m_GameContext);

	//Root-Scene Initialize
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootInitialize(m_GameContext);
	}

	//Initialize Materials
	m_GameContext.pMaterialManager->Initialize(m_GameContext);

	m_IsInitialized = true;
}

void GameScene::RootUpdate()
{
	m_GameContext.pGameTime->Update();
	m_GameContext.pInput->Update();
	m_GameContext.pCamera = m_pActiveCamera;

	SoundManager::GetInstance()->GetSystem()->update();

	//User-Scene Update
	Update(m_GameContext);

	//Root-Scene Update
	volatile int count = 0;
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootUpdate(m_GameContext);
		++count;
	}
	m_pPhysxProxy->Update(m_GameContext);
}

void GameScene::RootDraw()
{
	//add shadow pass

	m_GameContext.pShadowMapper->Begin(m_GameContext);
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootDraw_ShadowMap(m_GameContext);
	}
	m_GameContext.pShadowMapper->End(m_GameContext);

	//User-Scene Draw
	Draw(m_GameContext);

	//Object-Scene Draw
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootDraw(m_GameContext);
	}

	//Object-Scene Post-Draw
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootPostDraw(m_GameContext);
	}

	//Draw PhysX
	m_pPhysxProxy->Draw(m_GameContext);

	//Draw Debug Stuff
	DebugRenderer::Draw(m_GameContext);
	//SpriteRenderer::GetInstance()->Draw(m_GameContext);
	//TextRenderer::GetInstance()->Draw(m_GameContext);

	SpriteRenderer::GetInstance()->Draw(m_GameContext);
	TextRenderer::GetInstance()->Draw(m_GameContext);
	//SpriteRenderer::GetInstance()->DrawImmediate(m_GameContext, m_GameContext.pShadowMapper->GetShadowMap(), XMFLOAT2(20, 20), XMFLOAT4(Colors::Red), XMFLOAT2(0, 0), XMFLOAT2(0.2, 0.2));
	//Post Processing Implementation START
	//************************************

	//1. Check if our MaterialIds vector isn't empty (if empty, ignore PP)
	if (m_PostProcessingMaterialIds.size() == 0)
		return;
	//2. Get the current (= INIT_RT) rendertarget from the game (OverlordGame::GetRenderTarget...)
	auto game = SceneManager::GetInstance()->GetGame();
	auto INIT_RT = game->GetRenderTarget();//??????
										   //3. Create a new variable to hold our previous rendertarget (= PREV_RT) that holds the content of the previous drawcall 
										   //   and want to use as a ShaderResource (Texture) for the next PP Effect.
	RenderTarget* PREV_RT = INIT_RT;

	//4. For each MaterialID
	for (auto i : m_PostProcessingMaterialIds)
	{
		//4.1 Get the corresponding PostProcessingMaterial (= CURR_MAT) through MaterialManager
		auto CURR_MAT = m_GameContext.pMaterialManager->GetMaterial_PP(i);
		//4.2 Get the RT from CURR_MAT (= TEMP_RT)
		auto TEMP_RT = CURR_MAT->GetRenderTarget();
		//4.3 Use TEMP_RT as current rendertarget (OverlordGame::SetRenderTarget)
		game->SetRenderTarget(TEMP_RT);
		//4.4 Draw CURR_MAT (PREV_MAT provides the Texture used in the PPEffect)
		CURR_MAT->Draw(m_GameContext, PREV_RT);
		//4.5 Change PREV_MAT to TEMP_MAT
		PREV_RT = TEMP_RT;
	}

	//5. Restore the current rendertarget with INIT_RT
	game->SetRenderTarget(INIT_RT);
	//6. Use SpriteRenderer::DrawImmediate to draw the content of the last postprocessed rendertarget > PREV_RT
	SpriteRenderer::GetInstance()->DrawImmediate(m_GameContext, PREV_RT->GetShaderResourceView(), XMFLOAT2(0, 0));

	//i want to exclude this from the pp
	
}

void GameScene::RootSceneActivated()
{
	//Start Timer
	m_GameContext.pGameTime->Start();
	SceneActivated();
}

void GameScene::RootSceneDeactivated()
{
	//Stop Timer
	m_GameContext.pGameTime->Stop();
	SceneDeactivated();
}

void GameScene::RootWindowStateChanged(int state, bool active)
{
	//TIMER
	if (state == 0)
	{
		if (active)m_GameContext.pGameTime->Start();
		else m_GameContext.pGameTime->Stop();
	}
}

void GameScene::AddPostProcessingMaterial(UINT materialId_PP)
{
	//Add the given ID to the MaterialIDs vector
	//Check for duplicates

	if (std::find(m_PostProcessingMaterialIds.begin(), m_PostProcessingMaterialIds.end(), materialId_PP) == m_PostProcessingMaterialIds.end())
	{
		m_PostProcessingMaterialIds.push_back(materialId_PP);
	}

	//Hint: std::find
}

void GameScene::RemovePostProcessingMaterial(UINT materialId_PP)
{
	//Remove the given ID from the MaterialsIDs vector

	auto i = std::find(m_PostProcessingMaterialIds.begin(), m_PostProcessingMaterialIds.end(), materialId_PP);
	if (i != m_PostProcessingMaterialIds.end())
	{
		m_PostProcessingMaterialIds.erase(i);
	}
	//Hint: std::erase
}

void GameScene::SetActiveCamera(CameraComponent* pCameraComponent)
{
	//nullptr for DefaultCamera

	if (m_pActiveCamera != nullptr)
		m_pActiveCamera->m_IsActive = false;

	m_pActiveCamera = (pCameraComponent) ? pCameraComponent : m_pDefaultCamera;
	m_pActiveCamera->m_IsActive = true;
}
