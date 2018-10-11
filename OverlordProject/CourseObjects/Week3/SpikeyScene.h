#pragma once
#include "Scenegraph/GameScene.h"
#include "Helpers\EffectHelper.h"

class Material;
class SpikeyScene: public GameScene
{
public:
	SpikeyScene(void);
	virtual ~SpikeyScene(void);

protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:

	float m_FpsInterval;
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SpikeyScene( const SpikeyScene &obj);
	SpikeyScene& operator=( const SpikeyScene& obj);
};

