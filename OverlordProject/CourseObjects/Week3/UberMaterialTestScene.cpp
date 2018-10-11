#include "stdafx.h"
#include "UberMaterialTestScene.h"
#include "Prefabs\Prefabs.h"
#include "Components\Components.h"
#include "Physx\PhysxProxy.h"
#include "Physx\PhysxManager.h"
#include "Content\ContentManager.h"
#include "Graphics\MeshFilter.h"
#include "../../Materials/UberMaterial.h"
#include "Components\ModelComponent.h"



UberMaterialTestScene::UberMaterialTestScene()
	:GameScene(L"UberMaterialTestScene")
{
}


UberMaterialTestScene::~UberMaterialTestScene()
{
}

void UberMaterialTestScene::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	m_pObj = new GameObject();
	m_pObj->AddComponent(new ModelComponent(L"./Resources/Meshes/Teapot.ovm"));

	//Material
	auto myMaterial = new UberMaterial();
	myMaterial->EnableDiffuseTexture(true);
	myMaterial->EnableEnvironmentMapping(true);
	myMaterial->EnableFresnelFaloff(true);
	myMaterial->EnableNormalMapping(true);
	myMaterial->EnableOpacityMap(true);
	myMaterial->EnableSpecularBlinn(true);
	myMaterial->EnableSpecularLevelTexture(true);
	myMaterial->EnableSpecularPhong(true);

	myMaterial->SetDiffuseTexture(L"./Resources/Textures/CobbleStone_DiffuseMap.dds");
	myMaterial->SetEnvironmentCube(L"./Resources/Textures/Sunol_Cubemap.dds");
	myMaterial->SetFresnelColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	myMaterial->SetFresnelHardness(0.5f);
	myMaterial->SetFresnelMultiplier(0.3f);
	myMaterial->SetFresnelPower(0.2f);

	myMaterial->SetLightDirection(XMFLOAT3(0.577f, 0.577f, 0.577f));

	myMaterial->SetNormalMapTexture(L"./Resources/Textures/CobbleStone_NormalMap.dds");


}

void UberMaterialTestScene::Update(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void UberMaterialTestScene::Draw(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
