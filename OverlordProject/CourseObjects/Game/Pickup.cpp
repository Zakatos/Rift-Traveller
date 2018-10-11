#include "stdafx.h"
#include "Pickup.h"

#include "Components/Components.h"
#include "Physx/PhysxManager.h"
#include "Scenegraph/GameObject.h"
#include "Physx\PhysxProxy.h"

#include "../OverlordProject/Materials/ColorMaterial.h"
#include "Content/ContentManager.h"
#include "../Week2/Character.h"

#include "Scenegraph/GameScene.h"
#include "Platformer.h"
#include "Level.h"
#include "../OverlordEngine/Graphics/ModelAnimator.h"
#include "Base/SoundManager.h"

Pickup::Pickup(XMFLOAT3 pos, PickUpType type):
	m_Pos(pos),
	m_AddForce(false),
	m_State(State::floating),
	m_CurrFlyingTime(0.0f),
	m_TotalFlyingTime(0.5f),
	m_HasBeenTriggered(false),
	m_Speed(5.0f),
	m_Type(type)
{
}

Pickup::~Pickup()
{
}

void Pickup::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto bouncyMaterial = physX->createMaterial(0.5, 0.5, 1.0f);

	if (m_Type == Coin)
	{
		m_pModel = new ModelComponent(L"Resources/Meshes/Platformer/GoldCoin.ovm");
	}
	else
	{
		m_pModel = new ModelComponent(L"Resources/Meshes/Platformer/Key.ovm");
	}
	AddComponent(m_pModel);

	auto rigidBody = new RigidBodyComponent();

	AddComponent(rigidBody);

	
	if (m_Type == Coin)
	{
		m_pModel->SetMaterial(0);
	}
	else
	{
		m_pModel->SetMaterial(13);
	}
	std::shared_ptr<PxGeometry> geometry(new PxBoxGeometry(5.0f, 1.0f, 5.0f));

	ColliderComponent *collider = new ColliderComponent(geometry, *bouncyMaterial);

	collider->EnableTrigger(true);

	AddComponent(collider);

	//IMPACT PARTICLE
	m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/4338.png", 20);
	m_pParticleEmitter->SetVelocity(XMFLOAT3(0.0f, 0.3f, 0.0f));
	m_pParticleEmitter->SetMinSize(1.0f);
	m_pParticleEmitter->SetMaxSize(2.0f);
	m_pParticleEmitter->SetMinEnergy(1.0f);
	m_pParticleEmitter->SetMaxEnergy(2.0f);
	m_pParticleEmitter->SetMinSizeGrow(1.0f);
	m_pParticleEmitter->SetMaxSizeGrow(2.0f);
	m_pParticleEmitter->SetMinEmitterRange(0.2f);
	m_pParticleEmitter->SetMaxEmitterRange(0.5f);
	m_pParticleEmitter->SetColor(XMFLOAT4(229.0f / 255.0f, 184.0f / 255.0f, 136.f / 255.0f, 0.6f));
	AddComponent(m_pParticleEmitter);

	//Sounds
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD_RESULT fmodres;

	fmodres = fmodSystem->createSound("./Resources/Sounds/Effects/Item1A.wav", FMOD_LOOP_OFF, 0, &m_pSound);
	SoundManager::ErrorCheck(fmodres);

	if (m_Type == Key)
	{
		GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	}
}

void Pickup::Update(const GameContext & gameContext)
{
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD_RESULT fmodres;
	
	if (m_State == State::floating)
	{
		GetTransform()->Translate(m_Pos);
		m_pModel->GetTransform()->Rotate(0.0f, gameContext.pGameTime->GetElapsed() * 50.0f, gameContext.pGameTime->GetElapsed() * 50.0f);
	}
	//pickup flies towards you for a certain amount of time then after that time it
	//disappears
	else if (m_State == State::flyingTowardsPlayer)
	{
			
		m_CurrFlyingTime += gameContext.pGameTime->GetElapsed();
		AddForce(gameContext.pGameTime->GetElapsed());
		
		if (m_CurrFlyingTime > m_TotalFlyingTime)
		{
			if (AddForce(gameContext.pGameTime->GetElapsed()))
			{
				SetIsActive(false);
				m_CurrFlyingTime = 0.0f;

				auto scene = dynamic_cast<Platformer*>(GetScene());

				auto level = dynamic_cast<Level*>(scene->GetLevel());
				fmodres = fmodSystem->playSound(m_pSound, 0, false, &m_pChannel);
				level->DeletePickup(this);
			}
		}

	}
}

void Pickup::SetState(State state)
{
	if (!m_HasBeenTriggered)
	{
		m_State = state;
		m_HasBeenTriggered = true;
	}
}

void Pickup::SetGoal(float x, float z)
{
	m_GoalX = (x - GetTransform()->GetPosition().x) * 2.0f;
	m_GoalZ = (z - GetTransform()->GetPosition().z) * 2.0f;
}

void Pickup::SetCharacterRef(Character * character)
{
	m_pCharacter = character;
}


bool Pickup::AddForce(float elapsedSec)
{
	//goal - pos
	//clamp
	//max - min / max
	//* speed
	//elapsedSec

	//add world offset

	auto goal = m_pCharacter->GetTransform()->GetPosition();
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

	GetTransform()->Translate(pos.x + vec.x, pos.y + vec.y , pos.z + vec.z);

	if (GetDistance(vec, pos) >= 60.0f)
	{
		return true;
	}
	 
	return false;
	//GetComponent<RigidBodyComponent>()->AddForce(PxVec3(m_GoalX, 40.5f, m_GoalZ), PxForceMode::eVELOCITY_CHANGE, true);
}

float Pickup::GetDistance(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	XMVECTOR vector1 = XMLoadFloat3(&v1);
	XMVECTOR vector2 = XMLoadFloat3(&v2);
	XMVECTOR vectorSub = XMVectorSubtract(vector1, vector2);
	XMVECTOR length = XMVector3Length(vectorSub);

	float distance = 0.0f;
	XMStoreFloat(&distance, length);
	return distance;
}
