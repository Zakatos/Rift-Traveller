#pragma once
#include "../Graphics/Material.h"
#include "../Graphics/TextureData.h"

class ShadowMapMaterial :public Material
{
public:
	ShadowMapMaterial();
	~ShadowMapMaterial();
	void SetLight(XMFLOAT4X4 lightVP);
	void SetWorld(XMFLOAT4X4 world);


protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;
private:

	ID3DX11EffectMatrixVariable * m_pLightVPMatrixVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable = nullptr;
	XMFLOAT4X4 m_world;
	XMFLOAT4X4 m_light;

	ShadowMapMaterial& operator=(const ShadowMapMaterial& yRef);
	ShadowMapMaterial(const ShadowMapMaterial& obj);
};

