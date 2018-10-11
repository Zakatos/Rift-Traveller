#include "stdafx.h"
#include "SkinnedDiffuseMaterial_Shadow.h"
#include "Base/GeneralStructs.h"
#include "Diagnostics/Logger.h"
#include "Content/ContentManager.h"
#include "Graphics/TextureData.h"
#include "Components/ModelComponent.h"
#include "Graphics/ModelAnimator.h"

ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pBoneTransformsVariable = nullptr;

SkinnedDiffuseMaterial_Shadow::SkinnedDiffuseMaterial_Shadow() :
	Material(L"./Resources/Effects/Shadow/PosNormTex3D_Skinned_Shadow.fx")
{
}


SkinnedDiffuseMaterial_Shadow::~SkinnedDiffuseMaterial_Shadow()
{
	//m_pBoneTransformsVariable
}

void SkinnedDiffuseMaterial_Shadow::SetDiffuseTexture(const wstring& assetFile)
{
	if (assetFile != L"")
	{
		m_pTextureData = ContentManager::Load<TextureData>(assetFile);
	}
}

void SkinnedDiffuseMaterial_Shadow::SetLightDirection(XMFLOAT3 lightDir)
{
	m_light = lightDir;

}

void SkinnedDiffuseMaterial_Shadow::SetWorld(XMFLOAT4X4 world)
{
	m_world = world;
}

void SkinnedDiffuseMaterial_Shadow::LoadEffectVariables()
{
	auto effectVar = m_pEffect->GetVariableByName("gDiffuseMap");
	m_pDiffuseMapVariabele = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;
	effectVar = m_pEffect->GetVariableByName("shadowMap");
	m_ShadowMapVariabele = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;
	effectVar = m_pEffect->GetVariableByName("gLightViewProj");
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

void SkinnedDiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	auto lightVP = gameContext.pShadowMapper->GetLightVP();

	if (m_pDiffuseMapVariabele)
		m_pDiffuseMapVariabele->SetResource(m_pTextureData->GetShaderResourceView());
	if (m_pLightVPMatrixVariable)
		m_pLightVPMatrixVariable->SetMatrix(&lightVP._11);
	if (m_ShadowMapVariabele)
		m_ShadowMapVariabele->SetResource(gameContext.pShadowMapper->GetShadowMap());

	if (m_pBoneTransformsVariable)
	{
		m_pBoneTransformsVariable->SetMatrixArray(&pModelComponent->GetAnimator()->GetBoneTransforms()[0]._11, 0, pModelComponent->GetAnimator()->GetBoneTransforms().size());
	}
}