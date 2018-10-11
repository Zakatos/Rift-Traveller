//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PostVignette.h"
#include "Graphics/RenderTarget.h"

PostVignette::PostVignette()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Vignette.fx"),
	m_pTextureMapVariabele(nullptr)
{
}

PostVignette::~PostVignette(void)
{
}

void PostVignette::LoadEffectVariables()
{
	//Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	auto effectVar = m_pEffect->GetVariableByName("gTexture");
	m_pTextureMapVariabele = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;
	//Check if valid!
}

void PostVignette::UpdateEffectVariables(RenderTarget* rendertarget, const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget

	if (m_pTextureMapVariabele)
		m_pTextureMapVariabele->SetResource(rendertarget->GetShaderResourceView());
}