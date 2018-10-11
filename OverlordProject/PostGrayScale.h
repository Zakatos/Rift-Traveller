#pragma once
#include "PostProcessingMaterial.h"


class RenderTarget;
class ID3D11EffectShaderResourceVariable;

class PostGrayscale : public PostProcessingMaterial
{
public:
	PostGrayscale();
	~PostGrayscale();

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* rendertarget, const GameContext& gameContext) override;

	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PostGrayscale(const PostGrayscale &obj);
	PostGrayscale& operator=(const PostGrayscale& obj);
};
