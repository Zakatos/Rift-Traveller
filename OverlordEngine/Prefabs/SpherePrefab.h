#pragma once
#include "..\Scenegraph\GameObject.h"

class MeshIndexedDrawComponent;

class SpherePrefab: public GameObject
{
public:
	SpherePrefab(float radius = 1.f, int steps = 10, XMFLOAT4 color = (XMFLOAT4)Colors::Blue);
	~SpherePrefab(void);
	void UpdateColour(XMFLOAT4 colour);

protected:

	virtual void Initialize(const GameContext& gameContext);

private:

	float m_Radius;
	int m_Steps;
	XMFLOAT4 m_Color;
	MeshIndexedDrawComponent* m_pMesh;

	void UpdateBuffer(MeshIndexedDrawComponent & mesh);

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SpherePrefab(const SpherePrefab& yRef);									
	SpherePrefab& operator=(const SpherePrefab& yRef);
};
