#include "stdafx.h"

#include "UberMaterial.h"
#include "Content/ContentManager.h"
#include "Graphics/TextureData.h"

ID3DX11EffectShaderResourceVariable* UberMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pNormalMappingSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pEnvironmentSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pSpecularLevelSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pOpacitySRVvariable = nullptr;

UberMaterial::UberMaterial()
	:Material(L" ./Resources/Effects/UberShader_Start.fx", L"WithAlphaBlending")
{
	
}

UberMaterial::~UberMaterial()
{
	
}

void UberMaterial::SetLightDirection(XMFLOAT3 direction)
{
	m_LightDirection = direction;
}

void UberMaterial::EnableDiffuseTexture(bool enable)
{
	m_bDiffuseTexture = enable;
}

void UberMaterial::SetDiffuseTexture(const wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetDiffuseColor(XMFLOAT4 color)
{
	m_ColorDiffuse = color;
}

void UberMaterial::SetSpecularColor(XMFLOAT4 color)
{
	m_ColorSpecular = color;
}

void UberMaterial::EnableSpecularLevelTexture(bool enable)
{
	m_bSpecularLevelTexture = enable;
}

void UberMaterial::SetSpecularLevelTexture(const wstring& assetFile)
{
	m_pSpecularLevelTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetShininess(int shininess)
{
	m_Shininess = shininess;
}

void UberMaterial::SetAmbientColor(XMFLOAT4 color)
{
	m_ColorAmbient = color;
}

void UberMaterial::SetAmbientIntensity(float intensity)
{
	m_AmbientIntensity = intensity;
}

void UberMaterial::FlipNormalGreenCHannel(bool flip)
{
	m_bFlipGreenChannel = flip;
}

void UberMaterial::EnableNormalMapping(bool enable)
{
	m_bNormalMapping = enable;
}

void UberMaterial::SetNormalMapTexture(const wstring& assetFile)
{
	m_pNormalMappingTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::EnableEnvironmentMapping(bool enable)
{
	m_bEnvironmentMapping = enable;
}

void UberMaterial::SetEnvironmentCube(const wstring& assetFile)
{
	m_pEnvironmentCube = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetReflectionStrength(float strength)
{
	m_ReflectionStrength = strength;
}

void UberMaterial::SetRefractionStrength(float strength)
{
	m_RefractionStrength = strength;
}

void UberMaterial::SetRefractionIndex(float index)
{
	m_RefractionIndex = index;
}

void UberMaterial::SetOpacity(float opacity)
{
	m_Opacity = opacity;
}

void UberMaterial::EnableOpacityMap(bool enable)
{
	m_bOpacityMap = enable;
}

void UberMaterial::SetOpacityTexture(const wstring& assetFile)
{
	m_pOpacityMap = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::EnableSpecularBlinn(bool enable)
{
	m_bSpecularBlinn = enable;
}

void UberMaterial::EnableSpecularPhong(bool enable)
{
	m_bSpecularBlinn = enable;
}

void UberMaterial::EnableFresnelFaloff(bool enable)
{
	m_bFresnelFaloff = enable;
}

void UberMaterial::SetFresnelColor(XMFLOAT4 color)
{
	m_ColorFresnel = color;
}

void UberMaterial::SetFresnelPower(float power)
{
	m_FresnelPower = power;
}

void UberMaterial::SetFresnelMultiplier(float multiplier)
{
	m_FresnelMultiplier = multiplier;
}

void UberMaterial::SetFresnelHardness(float hardness)
{
	m_FresnelHardness = hardness;
}

void UberMaterial::LoadEffectVariables()
{
	m_pDiffuseSRVvariable = m_pEffect->GetVariableByName("gTextureDiffuse")->AsShaderResource();
	m_pNormalMappingSRVvariable = m_pEffect->GetVariableByName("gTextureNormal")->AsShaderResource();
	m_pEnvironmentSRVvariable = m_pEffect->GetVariableByName("gCubeEnvironment")->AsShaderResource();
	m_pOpacitySRVvariable = m_pEffect->GetVariableByName("gTextureOpacity")->AsShaderResource();
	m_pSpecularLevelSRVvariable = m_pEffect->GetVariableByName("gTextureSpecularIntensity")->AsShaderResource();
}

void UberMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	m_pNormalMappingSRVvariable->SetResource(m_pNormalMappingTexture->GetShaderResourceView());
	m_pEnvironmentSRVvariable->SetResource(m_pEnvironmentCube->GetShaderResourceView());
	m_pOpacitySRVvariable->SetResource(m_pOpacityMap->GetShaderResourceView());
	m_pSpecularLevelSRVvariable->SetResource(m_pSpecularLevelTexture->GetShaderResourceView());
}


