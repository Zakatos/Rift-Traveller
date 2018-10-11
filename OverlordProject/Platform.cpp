#include "stdafx.h"
#include "Platform.h"
#include "Components/Components.h"
#include "Physx/PhysxManager.h"
#include "Scenegraph/GameObject.h"
#include "Physx\PhysxProxy.h"
#include "Content/ContentManager.h"
#include "Scenegraph/GameScene.h"
#include "../OverlordEngine/Graphics/ModelAnimator.h"
#include "Base/SoundManager.h"
#include "CourseObjects/Game/Level.h"
#include "CourseObjects/Game/Platformer.h"
#include "CourseObjects/Week2/Character.h"
#include "CourseObjects/Game/MainCharacter.h"
#include "Scenegraph/SceneManager.h"

Platform::Platform(XMFLOAT3 pos) :
	m_Pos(pos),
	m_AddForce(false),
	m_CurrFlyingTime(0.1f),
	m_TotalFlyingTime(0.5f),
	m_Speed(0.2f),
	m_State(idle),
	m_HasReachedGoal(false),
	m_StartingPos(pos)
{
}


Platform::~Platform()
{
}

void Platform::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto HighFrictionMaterial = physX->createMaterial(1.0f, 1.0f, 1.0f);

	//Platform

	auto rigidBody = new RigidBodyComponent();
	
	rigidBody->SetKinematic(true);
	
	AddComponent(rigidBody);

	std::shared_ptr<PxGeometry> geometry(new PxBoxGeometry(5.0f,1.0f, 5.0f));

	ColliderComponent *collider = new ColliderComponent(geometry, *HighFrictionMaterial,PxTransform(PxVec3(0.0f, 2.5f, 0.0f)));
	
	AddComponent(collider);
	//new ColliderComponent(geom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	
	std::shared_ptr<PxGeometry> geometry2(new PxBoxGeometry(5.0f, 2.0f, 5.0f));
	ColliderComponent *triggerCollider = new ColliderComponent(geometry2, *HighFrictionMaterial, PxTransform(PxVec3(0.0f,4.0f,0.0f)));
	triggerCollider->EnableTrigger(true);
	AddComponent(triggerCollider);
	
	auto pPlatformModel = new ModelComponent(L"Resources/Meshes/Platformer/Platform2.ovm");

	pPlatformModel->SetMaterial(11);

	AddComponent(pPlatformModel);

	GetTransform()->Translate(m_Pos);
}

void Platform::Update(const GameContext & gameContext)
{
	
	/*SetOnTriggerCallBack([&](GameObject* triggerObject, GameObject* other, GameObject::TriggerAction action)
	{
	
	auto chara = dynamic_cast<Character*>(other);
	auto mainchara = dynamic_cast<Character*>(other);
	auto scene = dynamic_cast<Platformer*>(GetScene());
	auto trigger = static_cast<GameObject*>(triggerObject);

	if (chara && trigger && mainchara  && action == GameObject::TriggerAction::ENTER)
	{
		std::cout << "ENTER" << std::endl;
		
		scene->RemoveChild(chara,false);
		scene->RemoveChild(mainchara, false);
		
		AddChild(chara);
		AddChild(mainchara);
		AddForce(elapsedSec, chara);
		AddForce(elapsedSec,mainchara);
	}
	else if(chara && trigger && mainchara  && action == GameObject::TriggerAction::LEAVE)
	{
		RemoveChild(chara);
		RemoveChild(mainchara);
		scene->AddChild(chara);
		scene->AddChild(mainchara);	
	}
});*/

	if (m_State == PlatformState::idle)
	{
		GetTransform()->Translate(m_Pos);
	}
	else if (m_State == flyingTowardsGoal)
	{
		m_CurrFlyingTime += gameContext.pGameTime->GetElapsed();
		AddForce(gameContext.pGameTime->GetElapsed());

		//std::cout << m_HasReachedGoal << std::endl;

			if (m_CurrFlyingTime > m_TotalFlyingTime)
			{
				
				if (GetDistance(GetTransform()->GetPosition(), m_SecondaryGoal) <= 3.0f && !m_HasReachedGoal)
				{
					m_HasReachedGoal = true;
					m_CurrFlyingTime = 0.0f;
					
				}
				/*if (!m_HasReachedGoal)
				{
					m_HasReachedGoal = true;
					m_CurrFlyingTime = 0.0f;
				}*/

				if (m_HasReachedGoal)
				{
					SetGoal(m_StartingPos);
					m_CurrFlyingTime = 0.0f;

				}

				if (GetDistance(GetTransform()->GetPosition(), m_StartingPos) <= 3.0f && m_HasReachedGoal)
				{
					SetGoal(m_SecondaryGoal);
					m_HasReachedGoal = false;
					m_CurrFlyingTime = 0.0f;
					
				}
		}
	}
}


void Platform::SetState(PlatformState state)
{
	m_State = state;
}

void Platform::SetGoal(XMFLOAT3 pos)
{
	m_Goal.x = (pos.x - GetTransform()->GetPosition().x) * 2.0f;
	m_Goal.y=  (pos.y - GetTransform()->GetPosition().y) * 2.0f;
	m_Goal.z = (pos.z - GetTransform()->GetPosition().z) * 2.0f;

}

void  Platform::SetSecondaryGoal(XMFLOAT3 pos)
{
	m_SecondaryGoal.x = (pos.x - GetTransform()->GetPosition().x) * 2.0f;
	m_SecondaryGoal.y = (pos.y - GetTransform()->GetPosition().y) * 2.0f;
	m_SecondaryGoal.z = (pos.z - GetTransform()->GetPosition().z) * 2.0f;
}
void Platform::AddForce(float elapsedSec)
{
	//goal - pos
	//clamp
	//max - min / max
	//* speed
	//elapsedSec

	//add world offset

	auto goal = m_Goal;
	auto currPos = GetTransform()->GetPosition();

	XMVECTOR tempGoal = XMLoadFloat3(&goal);
	XMVECTOR tempCurrPos = XMLoadFloat3(&currPos);

	auto tempVec = XMVectorSubtract(tempGoal, tempCurrPos);

	XMFLOAT3 vec{};

	XMStoreFloat3(&vec, tempVec);

	vec.x *= m_Speed;
	vec.y *= m_Speed;
	vec.z *= m_Speed;

	vec.x *= elapsedSec;
	vec.y *= elapsedSec;
	vec.z *= elapsedSec;

	XMFLOAT3 pos = GetTransform()->GetPosition();

	
	
	GetTransform()->Translate(pos.x + vec.x, pos.y + vec.y, pos.z + vec.z);

}

void Platform::AddForce(float elapsedSec,GameObject* object)
{
	auto goal = m_Goal;
	auto currPos = GetTransform()->GetPosition();

	XMVECTOR tempGoal = XMLoadFloat3(&goal);
	XMVECTOR tempCurrPos = XMLoadFloat3(&currPos);

	auto tempVec = XMVectorSubtract(tempGoal, tempCurrPos);

	XMFLOAT3 vec{};

	XMStoreFloat3(&vec, tempVec);

	vec.x *= m_Speed;
	vec.y *= m_Speed;
	vec.z *= m_Speed;

	vec.x *= elapsedSec;
	vec.y *= elapsedSec;
	vec.z *= elapsedSec;

	XMFLOAT3 pos = GetTransform()->GetPosition();
	
	object->GetTransform()->Translate(pos.x + vec.x, pos.y + vec.y, pos.z + vec.z);
	
}

float Platform::GetDistance(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	XMVECTOR vector1 = XMLoadFloat3(&v1);
	XMVECTOR vector2 = XMLoadFloat3(&v2);
	XMVECTOR vectorSub = XMVectorSubtract(vector1, vector2);
	XMVECTOR length = XMVector3Length(vectorSub);

	float distance = 0.0f;
	XMStoreFloat(&distance, length);
	//std::cout << distance << std::endl;
	return distance;
}


