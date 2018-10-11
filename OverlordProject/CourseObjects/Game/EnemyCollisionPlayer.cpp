#include "stdafx.h"
#include "EnemyCollisionPlayer.h"

#include "Components/Components.h"
#include "Physx/PhysxManager.h"
#include "Scenegraph/GameObject.h"
#include "Level.h"
#include "Platformer.h"
#include "MainCharacter.h"

EnemyCollisionPlayer::EnemyCollisionPlayer()
{
}


EnemyCollisionPlayer::~EnemyCollisionPlayer()
{
}

void EnemyCollisionPlayer::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto bouncyMaterial = physX->createMaterial(0.5, 0.5, 1.0f);

	auto rigidBody = new RigidBodyComponent();

	AddComponent(rigidBody);

	rigidBody->SetKinematic(true);

	std::shared_ptr<PxGeometry> geometry(new PxBoxGeometry(6.0f,4.0f,6.0f));

	ColliderComponent *collider = new ColliderComponent(geometry, *bouncyMaterial, PxTransform(PxVec3(0.0f,0.0f,-4.0f)));

	collider->EnableTrigger(true);

	AddComponent(collider);
}

void EnemyCollisionPlayer::Update(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	GetTransform()->Translate(GetParent()->GetTransform()->GetPosition());
	GetTransform()->Rotate(GetParent()->GetTransform()->GetRotation().x, GetParent()->GetTransform()->GetRotation().y, GetParent()->GetTransform()->GetRotation().z);
}

void EnemyCollisionPlayer::SetCharacterReference(Character * character)
{
	auto scene = dynamic_cast<Platformer*>(GetScene());

	if (!scene->GetMainCharacter()->GetStunned())
	{
		auto parent = static_cast<Enemy*>(GetParent());
		parent->SetCharacterReference(character);

		auto level = dynamic_cast<Level*>(scene->GetLevel());

		scene->RemoveLifePlayer();
		level->RemoveLifePlayer();

		scene->GetMainCharacter()->SetStunned();
	}
	
}
