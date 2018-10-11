#include "stdafx.h"

#include "DiffuseMaterial.h"
#include "Graphics/TextureData.h"
#include "Content/ContentManager.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial::m_pDiffuseSRVvariable = nullptr;


DiffuseMaterial::DiffuseMaterial()
	:Material(L"./Resources/Effects/PosNormTex3D.fx", L"Default")
{

}

DiffuseMaterial::~DiffuseMaterial()
{

}

void DiffuseMaterial::LoadEffectVariables()
{
	LPCSTR varName{ "gDiffuseMap" };
	m_pDiffuseSRVvariable = m_pEffect->GetVariableByName(varName)->AsShaderResource();
	//m_pDiffuseSRVvariable->SetResource(variable);
}

void DiffuseMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

}


void DiffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	//m_pDiffuseTexture = new TextureData();
	//UNREFERENCED_PARAMETER(assetFile);

	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}
