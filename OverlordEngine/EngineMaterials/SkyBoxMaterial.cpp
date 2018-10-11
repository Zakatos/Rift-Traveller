#include "stdafx.h"
#include "SkyBoxMaterial.h"
#include "../Content/ContentManager.h"
#include "../Graphics/TextureData.h"

ID3DX11EffectShaderResourceVariable* SkyBoxMaterial::m_pSkyBoxSRVvariable = nullptr;

SkyBoxMaterial::SkyBoxMaterial()
	:Material(L"./Resources/Effects/SkyBox.fx")
{
}


SkyBoxMaterial::~SkyBoxMaterial()
{
}

void SkyBoxMaterial::SetSkyBoxTexture(const wstring & assetFile)
{
	m_pSkyboxTexture = ContentManager::Load<TextureData>(assetFile);
}

void SkyBoxMaterial::LoadEffectVariables()
{
	if (!m_pSkyBoxSRVvariable)
	{
		m_pSkyBoxSRVvariable = m_pEffect->GetVariableByName("m_CubeMap")->AsShaderResource();
		if (!m_pSkyBoxSRVvariable->IsValid())
		{
			Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'m_TextureDiffuse\' variable not found!");
			m_pSkyBoxSRVvariable = nullptr;
		}
	}
}

void SkyBoxMaterial::UpdateEffectVariables(const GameContext & gameContext, ModelComponent * pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	if (m_pSkyboxTexture && m_pSkyBoxSRVvariable)
	{
		m_pSkyBoxSRVvariable->SetResource(m_pSkyboxTexture->GetShaderResourceView());
	}
}

