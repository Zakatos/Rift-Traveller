#include "stdafx.h"
#include "ShadowMapMaterial.h"
#include "Graphics/TextureData.h"
#include "../Components/ModelComponent.h"
#include "../Graphics/ModelAnimator.h"

ShadowMapMaterial::ShadowMapMaterial() :
	Material(L"./Resources/Effects/Shadow/ShadowMapGen.fx")
{
}


ShadowMapMaterial::~ShadowMapMaterial()
{
}

void ShadowMapMaterial::SetLight(XMFLOAT4X4 lightVP)
{
	m_light = lightVP;
}

void ShadowMapMaterial::SetWorld(XMFLOAT4X4 world)
{
	m_world = world;
}

void ShadowMapMaterial::LoadEffectVariables()
{
	auto effectVar = m_pEffect->GetVariableByName("gLightViewProj");
	m_pLightVPMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableByName("gWorld");
	m_pWorldMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
}

void ShadowMapMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	if (m_pLightVPMatrixVariable)
		m_pLightVPMatrixVariable->SetMatrix(&m_light._11);
	if (m_pWorldMatrixVariable)
		m_pWorldMatrixVariable->SetMatrix(&m_world._11);

}