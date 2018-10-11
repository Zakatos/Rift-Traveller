#pragma once
#include "stdafx.h"
#include "SkinnedShadowMapMaterial.h"
#include "Graphics/TextureData.h"
#include "../Components/ModelComponent.h"
#include "../Graphics/ModelAnimator.h"

SkinnedShadowMapMaterial::SkinnedShadowMapMaterial() :
	Material(L"./Resources/Effects/Shadow/ShadowMapGenSkinned.fx")
{
}


SkinnedShadowMapMaterial::~SkinnedShadowMapMaterial()
{
}

void SkinnedShadowMapMaterial::SetLight(XMFLOAT4X4 lightVP)
{
	m_light = lightVP;
}

void SkinnedShadowMapMaterial::SetWorld(XMFLOAT4X4 world)
{
	m_world = world;
}

void SkinnedShadowMapMaterial::LoadEffectVariables()
{
	auto effectVar = m_pEffect->GetVariableByName("gLightViewProj");
	m_pLightVPMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableByName("gWorld");
	m_pWorldMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;

	if (!m_pBoneTransformsVariable)
	{
		m_pBoneTransformsVariable = m_pEffect->GetVariableByName("gBones")->AsMatrix();
		if (!m_pBoneTransformsVariable->IsValid())
		{
			Logger::LogWarning(L"SkinnedDiffuseMaterial::LoadEffectVariables() > \'gBones\' variable not found!");
			m_pBoneTransformsVariable = nullptr;
		}
	}
}

void SkinnedShadowMapMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	if (m_pLightVPMatrixVariable)
		m_pLightVPMatrixVariable->SetMatrix(&m_light._11);
	if (m_pWorldMatrixVariable)
		m_pWorldMatrixVariable->SetMatrix(&m_world._11);

	if (pModelComponent->HasAnimator())
	{
		if (m_pBoneTransformsVariable)
		{
			m_pBoneTransformsVariable->SetMatrixArray(&pModelComponent->GetAnimator()->GetBoneTransforms()[0]._11, 0, pModelComponent->GetAnimator()->GetBoneTransforms().size());
		}
	}

}