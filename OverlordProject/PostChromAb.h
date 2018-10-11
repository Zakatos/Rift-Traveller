#pragma once
#include "PostProcessingMaterial.h"

class TextureData;
class ID3D11EffectShaderResourceVariable;

class PostChromAb : public PostProcessingMaterial
{
public:
	PostChromAb();
	~PostChromAb();

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(RenderTarget* rendertarget, const GameContext& gameContext);

	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;


private:

	float m_Timer = 0.0f;
	ID3DX11EffectScalarVariable* m_pTimerVariable = nullptr;
	ID3DX11EffectShaderResourceVariable* m_pNoiseTextureMapVariabele;
	TextureData* m_pTextureData = nullptr;
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PostChromAb(const PostChromAb &obj);
	PostChromAb& operator=(const PostChromAb& obj);
};
