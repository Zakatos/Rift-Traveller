//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ParticleEmitterComponent.h"
#include "../Helpers/EffectHelper.h"
#include "../Content/ContentManager.h"
#include "../Content/TextureDataLoader.h"
#include "../Graphics/Particle.h"
#include "../Components/TransformComponent.h"
#include "../Diagnostics/Logger.h"


ParticleEmitterComponent::ParticleEmitterComponent(const wstring& assetFile, int particleCount) :
	m_ParticleCount(particleCount),
	m_AssetFile(assetFile)
{
	//See Lab10_2
	m_Particles.resize(m_ParticleCount);
	for (int i = 0; i < m_ParticleCount; i++)
		m_Particles[i] = new Particle(m_Settings);
}


ParticleEmitterComponent::~ParticleEmitterComponent(void)
{
	//See Lab10_2
	for (size_t i = 0; i < m_Particles.size(); i++)
		delete m_Particles[i];
	m_Particles.clear();

	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	//See Lab10_2
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	//See Lab10_2
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");
	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);

	m_pWvpVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pWvpVariable->IsValid())
		Logger::LogWarning(L"ParticleEmitterComponent:: Variable 'gWorldViewProj' not valid!");

	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	if (!m_pViewInverseVariable->IsValid())
		Logger::LogWarning(L"ParticleEmitterComponent:: Variable 'gViewInverse' not valid!");

	m_pTextureVariable = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();
	if (!m_pTextureVariable->IsValid())
		Logger::LogWarning(L"ParticleEmitterComponent:: Variable 'gParticleTexture' not valid!");

	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, m_pInputLayoutSize);
}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	//See Lab10_2
	if (m_pVertexBuffer)
		SafeRelease(m_pVertexBuffer);

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = m_ParticleCount * sizeof(ParticleVertex);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	HRESULT hr = gameContext.pDevice->CreateBuffer(&bd, nullptr, &m_pVertexBuffer);
	Logger::LogHResult(hr, L"ParticleEmitterComponent::CreateVertexBuffer...");
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	//See Lab10_2

	//BUFFER MAPPING CODE [PARTIAL :)]
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//ParticleVertex* pBuffer = (ParticleVertex*) mappedResource.pData;

	float particleInterval = ((m_Settings.MaxEnergy + m_Settings.MinEnergy) / 2) / m_ParticleCount;

	m_LastParticleInit += gameContext.pGameTime->GetElapsed();
	m_ActiveParticles = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ParticleVertex* pBuffer = static_cast<ParticleVertex*>(mappedResource.pData);
	for (Particle* p : m_Particles)
	{
		p->Update(gameContext);
		if (p->IsActive())
		{
			pBuffer[m_ActiveParticles] = p->GetVertexInfo();
			++m_ActiveParticles;
		}
		else if (m_LastParticleInit >= particleInterval)
		{
			p->Init(GetTransform()->GetWorldPosition());
			pBuffer[m_ActiveParticles] = p->GetVertexInfo();
			++m_ActiveParticles;
			m_LastParticleInit = 0.0f;
		}
	}
	//memcpy(mappedResource.pData, pBuffer, sizeof(ParticleVertex) * m_ParticleCount);
	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	//See Lab10_2
	XMFLOAT4X4 viewProjection = gameContext.pCamera->GetViewProjection();
	XMFLOAT4X4 viewInverse = gameContext.pCamera->GetViewInverse();
	m_pWvpVariable->SetMatrix(reinterpret_cast<float*>(&viewProjection));
	m_pViewInverseVariable->SetMatrix(reinterpret_cast<float*>(&viewInverse));
	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	UINT strides = sizeof(ParticleVertex);
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &strides, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pDefaultTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pDefaultTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}
