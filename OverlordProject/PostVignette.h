#pragma once
#include "PostProcessingMaterial.h"


class ID3D11EffectShaderResourceVariable;

class PostVignette : public PostProcessingMaterial
{
public:
	PostVignette();
	~PostVignette();

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(RenderTarget* rendertarget, const GameContext& gameContext);

	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;


private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PostVignette(const PostVignette &obj);
	PostVignette& operator=(const PostVignette& obj);
};
