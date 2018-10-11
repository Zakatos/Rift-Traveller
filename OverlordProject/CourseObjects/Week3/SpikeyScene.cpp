//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SpikeyScene.h"
#include "Scenegraph\GameObject.h"
#include "Diagnostics\Logger.h"
#include "Diagnostics\DebugRenderer.h"


#include "Prefabs\Prefabs.h"
#include "Components\Components.h"
#include "Physx\PhysxProxy.h"
#include "Physx\PhysxManager.h"
#include "Content\ContentManager.h"
#include "Graphics\MeshFilter.h"
#include "../../Materials/SpikeyMaterial.h"
#include "Components\ModelComponent.h"
#include "Components\TerrainComponent.h"

#define FPS_COUNTER 1

SpikeyScene::SpikeyScene(void):
	GameScene(L"SpikeyScene"),
	m_FpsInterval(FPS_COUNTER)
{
}


SpikeyScene::~SpikeyScene(void)
{
}

void SpikeyScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	gameContext.pGameTime->ForceElapsedUpperbound(true);

	// Create PhysX ground plane
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	
	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));
	
	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom,*bouncyMaterial,PxTransform(PxQuat(XM_PIDIV2,PxVec3(0,0,1)))));
	AddChild(ground);


	//Material Test
	auto myMaterial = new SpikeyMaterial();
	myMaterial->SetDiffuseTexture(L"./Resources/Textures/Chair_Dark.dds");
	gameContext.pMaterialManager->AddMaterial(myMaterial, 60);
	
	auto model = new ModelComponent(L"./Resources/Meshes/Teapot.ovm");
	model->SetMaterial(60);

	auto teapot = new GameObject();
	teapot->AddComponent(model);
	AddChild(teapot);
	teapot->GetTransform()->Translate(0,0,20);

	auto skybox = new SkyBoxPrefab();
	AddChild(skybox);

	//terrain
	auto terrainGameObject = new GameObject();
	terrainGameObject->AddComponent(new TerrainComponent(L"./Resources/Terrain/Hawai_HeightMap_64x64x16.raw", L"./Resources/Terrain/Hawai_Yellow.dds", 64, 64, 64.0f, 64.0f, 10.0f));
	
	AddChild(terrainGameObject);

}

void SpikeyScene::Update(const GameContext& gameContext)
{

	m_FpsInterval += gameContext.pGameTime->GetElapsed();
	if (m_FpsInterval >= FPS_COUNTER)
	{
		m_FpsInterval -= FPS_COUNTER;
		Logger::LogFormat(LogLevel::Info, L"FPS: %i", gameContext.pGameTime->GetFPS());
	}
}

void SpikeyScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
