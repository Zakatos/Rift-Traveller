#pragma once
#include "Scenegraph/GameScene.h"

class BoneObject;

class SoftwareSkinningScene_2 : public GameScene
{
public:
	SoftwareSkinningScene_2(void);
	virtual ~SoftwareSkinningScene_2(void);
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
	SoftwareSkinningScene_2(const SoftwareSkinningScene_2 &obj);
	SoftwareSkinningScene_2& operator=(const SoftwareSkinningScene_2& obj);
};

