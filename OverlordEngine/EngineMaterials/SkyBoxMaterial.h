#pragma once
#include "../Graphics/Material.h"

class TextureData;

class SkyBoxMaterial : public Material
{
public:
	SkyBoxMaterial();
	~SkyBoxMaterial();

	void SetSkyBoxTexture(const wstring& assetFile);

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

	TextureData* m_pSkyboxTexture;
	static ID3DX11EffectShaderResourceVariable* m_pSkyBoxSRVvariable;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SkyBoxMaterial(const SkyBoxMaterial &obj);
	SkyBoxMaterial& operator=(const SkyBoxMaterial& obj);
};

