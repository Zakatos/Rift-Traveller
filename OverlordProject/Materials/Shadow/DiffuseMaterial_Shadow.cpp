#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"
#include "Content/ContentManager.h"


DiffuseMaterial_Shadow::DiffuseMaterial_Shadow() :
	Material(L"./Resources/Effects/Shadow/PosNormTex3D_Shadow.fx")
{
}


DiffuseMaterial_Shadow::~DiffuseMaterial_Shadow()
{
}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const wstring& assetFile)
{
	if (assetFile != L"")
	{
		m_pTextureData = ContentManager::Load<TextureData>(assetFile);
	}
}

void DiffuseMaterial_Shadow::SetLightDirection(XMFLOAT3 lightDir)
{
	m_light = lightDir;
}

void DiffuseMaterial_Shadow::SetWorld(XMFLOAT4X4 world)
{
	m_world = world;
}

void DiffuseMaterial_Shadow::LoadEffectVariables()
{
	auto effectVar = m_pEffect->GetVariableByName("gDiffuseMap");
	m_pDiffuseMapVariabele = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;
	effectVar = m_pEffect->GetVariableByName("shadowMap");
	m_ShadowMapVariabele = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;
	effectVar = m_pEffect->GetVariableByName("gLightViewProj");
	m_pLightVPMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableByName("gWorld");
	m_pWorldMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
}

void DiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
	//gameContext.pShadowMapper->SetLight(XMFLOAT3(-10, 0, 0), m_light);
	auto lightVP = gameContext.pShadowMapper->GetLightVP();

	if (m_pDiffuseMapVariabele)
		m_pDiffuseMapVariabele->SetResource(m_pTextureData->GetShaderResourceView());

	if (m_pLightVPMatrixVariable)
		m_pLightVPMatrixVariable->SetMatrix(&lightVP._11);

	if (m_ShadowMapVariabele)
		m_ShadowMapVariabele->SetResource(gameContext.pShadowMapper->GetShadowMap());


}