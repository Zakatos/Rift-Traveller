#pragma once
#include "../OverlordEngine/PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostBlur : public PostProcessingMaterial
{
public:
	PostBlur();
	~PostBlur();

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(RenderTarget* rendertarget, const GameContext& gameContext);

	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;


private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PostBlur(const PostBlur &obj);
	PostBlur& operator=(const PostBlur& obj);
};
