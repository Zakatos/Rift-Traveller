#include "stdafx.h"

#include "CharacterTestScene.h"
#include "Components/Components.h"
#include "Physx/PhysxManager.h"
#include "../OverlordEngine/Scenegraph/GameObject.h"
#include "Physx\PhysxProxy.h"
#include "Character.h"



CharacterTestScene::CharacterTestScene()
	:GameScene(L"CharacterTestScene")
{
}


CharacterTestScene::~CharacterTestScene()
{
}

void CharacterTestScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//GROUND PLANE
	//************
	GetPhysxProxy()->EnablePhysxDebugRendering(true);

	auto physX = PhysxManager::GetInstance()->GetPhysics();
	 
	
	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(ground);

	/*auto gameController = new GameObject();
	auto controller = new ControllerComponent(bouncyMaterial);
	gameController->AddComponent(controller);

	AddChild(gameController);*/
	
	auto character = new Character();

	AddChild(character);

}

void CharacterTestScene::Update(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void CharacterTestScene::Draw(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
