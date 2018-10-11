#pragma once
#include "Scenegraph/GameScene.h"
#include "Helpers\EffectHelper.h"
class TextureData;
class Material;
class UberMaterialTestScene: GameScene
{
public:
	UberMaterialTestScene(void);
	virtual ~UberMaterialTestScene(void);


protected:
	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);


private:

	float m_FpsInterval;
	GameObject* m_pObj;
};

