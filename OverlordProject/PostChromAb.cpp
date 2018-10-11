//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PostChromAb.h"
#include "Graphics/RenderTarget.h"
#include "Base/GeneralStructs.h"
#include "Content/ContentManager.h"
#include "../OverlordEngine/Graphics/TextureData.h"

PostChromAb::PostChromAb()
	: PostProcessingMaterial(L"./Resources/Effects/Post/ChromAb.fx"),
	m_pTextureMapVariabele(nullptr)
{
	m_pTextureData = ContentManager::Load<TextureData>(L"./Resources/Textures/noise.png");
}

PostChromAb::~PostChromAb(void)
{
}

void PostChromAb::LoadEffectVariables()
{
	//Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	auto effectVar = m_pEffect->GetVariableByName("gTexture");
	m_pTextureMapVariabele = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;

	effectVar = m_pEffect->GetVariableByName("gTime");
	m_pTimerVariable = (effectVar->IsValid()) ? effectVar->AsScalar() : nullptr;

	effectVar = m_pEffect->GetVariableByName("gNoiseTexture");
	m_pNoiseTextureMapVariabele = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;

}

void PostChromAb::UpdateEffectVariables(RenderTarget* rendertarget, const GameContext& gameContext)
{
	//Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	m_Timer += gameContext.pGameTime->GetElapsed();

	if (m_pTimerVariable)
		m_pTimerVariable->SetFloat(m_Timer);

	if (m_pTextureMapVariabele)
		m_pTextureMapVariabele->SetResource(rendertarget->GetShaderResourceView());

	if (m_pNoiseTextureMapVariabele)
		m_pNoiseTextureMapVariabele->SetResource(m_pTextureData->GetShaderResourceView());
}