#pragma once
#include "Scenegraph/GameScene.h"

class BoneObject;

class SoftwareSkinningScene_1: public GameScene
{
public:
	SoftwareSkinningScene_1(void);
	virtual ~SoftwareSkinningScene_1(void);
protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:
	float m_FpsInterval;
	BoneObject *m_pBone0, *m_pBone1;
	float m_BoneRotation;
	int m_RotationSign;
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SoftwareSkinningScene_1(const SoftwareSkinningScene_1 &obj);
	SoftwareSkinningScene_1& operator=(const SoftwareSkinningScene_1& obj);
};

