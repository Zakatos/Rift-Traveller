#pragma once
#include "Graphics/Material.h"
#include "Graphics/TextureData.h"

class SkinnedShadowMapMaterial :public Material
{
public:
	SkinnedShadowMapMaterial();
	~SkinnedShadowMapMaterial();
	void SetLight(XMFLOAT4X4 lightVP);
	void SetWorld(XMFLOAT4X4 world);


protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;
private:
	ID3DX11EffectMatrixVariable * m_pBoneTransformsVariable = nullptr;

	ID3DX11EffectMatrixVariable* m_pLightVPMatrixVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable = nullptr;
	XMFLOAT4X4 m_world;
	XMFLOAT4X4 m_light;

	SkinnedShadowMapMaterial& operator=(const SkinnedShadowMapMaterial& yRef);
	SkinnedShadowMapMaterial(const SkinnedShadowMapMaterial& obj);
};



