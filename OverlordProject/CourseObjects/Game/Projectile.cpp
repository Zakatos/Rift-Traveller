#include "stdafx.h"
#include "Projectile.h"

#include "Components/Components.h"
#include "Physx/PhysxManager.h"
#include "Level.h"

Projectile::Projectile(XMFLOAT3 startPos, XMFLOAT3 forwardVec, Level* pLevel):
	m_StartPos(startPos),
	m_ForwardVec(forwardVec),
	m_Speed(10.0f),
	m_TimeAlive(0.0f),
	m_TotalTimeAlive(2.0f),
	m_IsAlive(true),
	m_pLevel(pLevel)
{
}


Projectile::~Projectile()
{
}

void Projectile::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto bouncyMaterial = physX->createMaterial(0.5, 0.5, 1.0f);

	auto rigidBody = new RigidBodyComponent();

	AddComponent(rigidBody);


	std::shared_ptr<PxGeometry> geometry(new PxBoxGeometry(2.0f, 2.0f, 2.0f));

	ColliderComponent *collider = new ColliderComponent(geometry, *bouncyMaterial);

	collider->EnableTrigger(true);

	AddComponent(collider);
	//Projectile Starting position
	GetTransform()->Translate(m_StartPos.x, m_StartPos.y +5.0f, m_StartPos.z);

	//PARTICLE
	m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/MAGINVShockSpellArt.png", 20);
	m_pParticleEmitter->SetVelocity(XMFLOAT3(0, 0.01f, 0));
	m_pParticleEmitter->SetMinSize(0.1f);
	m_pParticleEmitter->SetMaxSize(0.8f);
	m_pParticleEmitter->SetMinEnergy(0.2f);
	m_pParticleEmitter->SetMaxEnergy(0.8f);
	m_pParticleEmitter->SetMinSizeGrow(2.0f);
	m_pParticleEmitter->SetMaxSizeGrow(3.0f);
	m_pParticleEmitter->SetMinEmitterRange(0.2f);
	m_pParticleEmitter->SetMaxEmitterRange(0.5f);
	m_pParticleEmitter->SetColor(XMFLOAT4(1.f, 1.f, 1.f, 0.6f));
	AddComponent(m_pParticleEmitter);

}

void Projectile::Update(const GameContext & gameContext)
{
	if (m_IsAlive)
	{
		XMFLOAT3 pos{};
		pos.x = (m_ForwardVec.x + GetTransform()->GetPosition().x);
		pos.y = (m_ForwardVec.y + GetTransform()->GetPosition().y);
		pos.z = (m_ForwardVec.z + GetTransform()->GetPosition().z);

		m_TimeAlive += gameContext.pGameTime->GetElapsed();

		if (m_TimeAlive >= m_TotalTimeAlive)
		{
			m_pLevel->DeleteProjectile(this);
		}
		else
		{
			GetTransform()->Translate(pos);
			
		}
	}
	else
	{
		m_pLevel->DeleteProjectile(this);
	}
}
