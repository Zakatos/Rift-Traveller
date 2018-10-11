//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "Particle.h"
#include "../Components/TransformComponent.h"
#include "../Helpers/MathHelper.h"


Particle::Particle(const ParticleEmitterSettings& emitterSettings) :
	m_EmitterSettings(emitterSettings)
{
}


Particle::~Particle(void)
{
}

void Particle::Update(const GameContext& gameContext)
{
	//See Lab10_2
	if (!m_IsActive) return;

	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();

	if (m_CurrentEnergy < 0.0f)
	{
		m_IsActive = false; 
		return;
	}

	//XMFLOAT3 seconds{ gameContext.pGameTime->GetElapsed(), gameContext.pGameTime->GetElapsed(),gameContext.pGameTime->GetElapsed() };
	m_VertexInfo.Position.x +=  m_EmitterSettings.Velocity.x * gameContext.pGameTime->GetElapsed();
	m_VertexInfo.Position.y += m_EmitterSettings.Velocity.y * gameContext.pGameTime->GetElapsed();
	m_VertexInfo.Position.z += m_EmitterSettings.Velocity.z * gameContext.pGameTime->GetElapsed();

	//Color
	m_VertexInfo.Color = m_EmitterSettings.Color;

	float particleLifePercent = m_CurrentEnergy / m_TotalEnergy;
	particleLifePercent = abs(sin(particleLifePercent * XM_PI));
	m_VertexInfo.Color.w = sqrt(particleLifePercent);
	Clamp(m_VertexInfo.Color.w, 1.0f, 0.0f);

	//Size
	if (m_SizeGrow > 0.0f)
		m_VertexInfo.Size = m_InitSize - m_SizeGrow * particleLifePercent;
	else if (m_SizeGrow < 0.0f)
		m_VertexInfo.Size = m_InitSize + m_SizeGrow * particleLifePercent;

}

void Particle::Init(XMFLOAT3 initPosition)
{

	//See Lab10_2
	m_IsActive = true;

	//Energy Initialization
	m_TotalEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	m_CurrentEnergy = m_TotalEnergy;

	//Position initialization
	XMFLOAT3 randomDirection(1.0f, 0.0f, 0.0f);
	XMVECTOR xmDirection = XMLoadFloat3(&randomDirection);
	XMMATRIX randomMatrix = XMMatrixRotationRollPitchYaw(randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI));
	xmDirection = XMVector3Transform(xmDirection, randomMatrix);
	XMStoreFloat3(&randomDirection, xmDirection);

	float startingPointDistance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);

	m_VertexInfo.Position.x = initPosition.x + startingPointDistance * randomDirection.x;
	m_VertexInfo.Position.y = initPosition.y + startingPointDistance * randomDirection.y;
	m_VertexInfo.Position.z = initPosition.z + startingPointDistance * randomDirection.z;

	//Size initialization
	m_VertexInfo.Size = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);

	//Rotation Initialization
	m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);

}
