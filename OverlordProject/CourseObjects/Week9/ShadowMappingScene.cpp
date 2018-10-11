//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMappingScene.h"
#include "Scenegraph\GameObject.h"

#include "Content\ContentManager.h"
#include "Components\Components.h"
#include "../../Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"

//ShadowMappingScene::ShadowMappingScene(void) :
//	GameScene(L"ShadowMappingScene")
//{
//}
//
//
//ShadowMappingScene::~ShadowMappingScene(void)
//{
//}
//
//void ShadowMappingScene::Initialize(const GameContext& gameContext)
//{
//	UNREFERENCED_PARAMETER(gameContext);
//	gameContext.pShadowMapper->SetLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });
//
//	//Materials
//	//*********
//	auto diffMat = new DiffuseMaterial_Shadow();
//	diffMat->SetDiffuseTexture(L"./Resources/Textures/Knight.jpg");
//	diffMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
//
//	gameContext.pMaterialManager->AddMaterial(diffMat, 0);
//	
//
//	auto diffMat2 = new DiffuseMaterial_Shadow();
//	diffMat2->SetDiffuseTexture(L"./Resources/Textures/GroundBrick.jpg");
//	diffMat2->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
//
//	gameContext.pMaterialManager->AddMaterial(diffMat2, 1);
//
//	//Ground Mesh
//	//***********
//
//	auto pGroundObj = new GameObject();
//	auto pGroundModel = new ModelComponent(L"./Resources/Meshes/UnitPlane.ovm");
//	pGroundModel->SetMaterial(1);
//
//	pGroundObj->AddComponent(pGroundModel);
//	pGroundObj->GetTransform()->Scale(10.0f, 10.0f, 10.0f);
//
//	AddChild(pGroundObj);
//
//	//Character Mesh
//	//**************
//
//	auto pCharacterObj = new GameObject();
//
//	//Level Model Component
//	m_pKnightModel = new ModelComponent(L"./Resources/Meshes/Knight.ovm");
//	m_pKnightModel->SetMaterial(0);
//	pCharacterObj->AddComponent(m_pKnightModel);
//
//	AddChild(pCharacterObj);
//	pCharacterObj->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
//
//	//Input
//	//*****
//	gameContext.pInput->AddInputAction(InputAction(0, Pressed, VK_SPACE));
//}
//
//void ShadowMappingScene::Update(const GameContext& gameContext)
//{
//	UNREFERENCED_PARAMETER(gameContext);
//
//	if (gameContext.pInput->IsActionTriggered(0))
//	{
//		gameContext.pShadowMapper->SetLight(gameContext.pCamera->GetTransform()->GetPosition(), gameContext.pCamera->GetTransform()->GetForward());
//
//		static_cast<SkinnedDiffuseMaterial_Shadow*>(gameContext.pMaterialManager->GetMaterial(0))->SetLightDirection(gameContext.pCamera->GetTransform()->GetForward());
//		static_cast<DiffuseMaterial_Shadow*>(gameContext.pMaterialManager->GetMaterial(1))->SetLightDirection(gameContext.pCamera->GetTransform()->GetForward());
//	}
//}
//
//void ShadowMappingScene::Draw(const GameContext& gameContext)
//{
//	UNREFERENCED_PARAMETER(gameContext);
//}
