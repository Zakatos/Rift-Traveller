//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PostBlur.h"
#include "Graphics/RenderTarget.h"

PostBlur::PostBlur()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Blur.fx"),
	m_pTextureMapVariabele(nullptr)
{
}

PostBlur::~PostBlur(void)
{
}

void PostBlur::LoadEffectVariables()
{
	//Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	auto effectVar = m_pEffect->GetVariableByName("gTexture");
	m_pTextureMapVariabele = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;
	//Check if valid!
}

void PostBlur::UpdateEffectVariables(RenderTarget* rendertarget, const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget

	if (m_pTextureMapVariabele)
		m_pTextureMapVariabele->SetResource(rendertarget->GetShaderResourceView());
}