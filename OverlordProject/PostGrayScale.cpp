//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PostGrayscale.h"
#include "Graphics/RenderTarget.h"

PostGrayscale::PostGrayscale()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Grayscale.fx"),
	m_pTextureMapVariabele(nullptr)
{
}

PostGrayscale::~PostGrayscale(void)
{
}

void PostGrayscale::LoadEffectVariables()
{
	//Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	auto effectVar = m_pEffect->GetVariableByName("gTexture");
	m_pTextureMapVariabele = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;
	//Check if valid!
}

void PostGrayscale::UpdateEffectVariables(RenderTarget* rendertarget, const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	if (m_pTextureMapVariabele)
		m_pTextureMapVariabele->SetResource(rendertarget->GetShaderResourceView());
}