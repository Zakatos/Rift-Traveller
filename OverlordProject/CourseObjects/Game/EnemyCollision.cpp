#include "stdafx.h"
#include "EnemyCollision.h"

#include "Components/Components.h"
#include "Physx/PhysxManager.h"
#include "Scenegraph/GameObject.h"
#include "Enemy.h"
#include "Level.h"
#include "Platformer.h"


EnemyCollision::EnemyCollision()
{
}


EnemyCollision::~EnemyCollision()
{
}

void EnemyCollision::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto bouncyMaterial = physX->createMaterial(0.5, 0.5, 1.0f);

	auto rigidBody = new RigidBodyComponent();

	AddComponent(rigidBody);

	rigidBody->SetKinematic(true);

	std::shared_ptr<PxGeometry> geometry(new PxBoxGeometry(2.0f, 4.0f, 2.0f));

	ColliderComponent *collider = new ColliderComponent(geometry, *bouncyMaterial, PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));

	collider->EnableTrigger(true);

	AddComponent(collider);
}

void EnemyCollision::Update(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	GetTransform()->Translate(GetParent()->GetTransform()->GetPosition());
	GetTransform()->Rotate(GetParent()->GetTransform()->GetRotation().x, GetParent()->GetTransform()->GetRotation().y, GetParent()->GetTransform()->GetRotation().z);
}

void EnemyCollision::Kill()
{
	auto parent = dynamic_cast<Enemy*>(GetParent());

	if (parent->SetIsHit())
	{
		//Todo fix physx callback bug
		//parent->SetIsAlive(false);

		//auto scene = dynamic_cast<Platformer*>(GetScene());

		//auto level = dynamic_cast<Level*>(scene->GetLevel());

		//level->SpawnPickup(GetParent()->GetTransform()->GetWorldPosition());
	}
	
}
