#pragma once
#include "..\Scenegraph\GameObject.h"

class SkyBoxPrefab : public GameObject
{
public:
	SkyBoxPrefab();
	virtual ~SkyBoxPrefab(void);

protected:

	virtual void Initialize(const GameContext& gameContext);


private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SkyBoxPrefab(const SkyBoxPrefab& yRef);
	SkyBoxPrefab& operator=(const SkyBoxPrefab& yRef);
};

