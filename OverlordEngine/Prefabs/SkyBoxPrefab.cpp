#include "stdafx.h"
#include "SkyBoxPrefab.h"
#include "../EngineMaterials/SkyBoxMaterial.h"
#include "../Components/Components.h"


SkyBoxPrefab::SkyBoxPrefab()
{
}


SkyBoxPrefab::~SkyBoxPrefab()
{
}

void SkyBoxPrefab::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto myMaterial = new SkyBoxMaterial();
	myMaterial->SetSkyBoxTexture(L"./Resources/Textures/SkyBox.dds");
	gameContext.pMaterialManager->AddMaterial(myMaterial, 50);

	auto model = new ModelComponent(L"./Resources/Meshes/Box.ovm");
	model->SetMaterial(50);

	auto teapot = new GameObject();
	teapot->AddComponent(model);
	AddChild(teapot);
	teapot->GetTransform()->Translate(0, 0, 20);
}
