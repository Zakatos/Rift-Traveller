#include "stdafx.h"

#include "SoftwareSkinningScene_1.h"
#include "./Scenegraph/BoneObject.h"
#include "../OverlordProject/Materials/ColorMaterial.h"
#include "./Components/Components.h"
#define FPS_COUNTER 1



SoftwareSkinningScene_1::SoftwareSkinningScene_1():
	GameScene(L"SoftwareSkinningScene_1"),
	m_FpsInterval(FPS_COUNTER),
	m_pBone0(nullptr),
	m_pBone1(nullptr),
	m_BoneRotation(0.0f),
	m_RotationSign(1)
{
}


SoftwareSkinningScene_1::~SoftwareSkinningScene_1()
{
}

void SoftwareSkinningScene_1::Initialize(const GameContext & gameContext)
{
	auto colourMaterial = new ColorMaterial();
	gameContext.pMaterialManager->AddMaterial(colourMaterial, 3);

	auto root = new GameObject();

	m_pBone0 = new BoneObject(0, 3, 15.0f);
	m_pBone1 = new BoneObject(1, 3, 15.0f);

	m_pBone0->AddBone(m_pBone1);

	root->AddChild(m_pBone0);

	root->GetComponent<TransformComponent>()->Rotate(0.0f, -90.0f, 0.0f);

	AddChild(root);
}

void SoftwareSkinningScene_1::Update(const GameContext & gameContext)
{
	m_BoneRotation += m_RotationSign * 45.0f * gameContext.pGameTime->GetElapsed();

	if (m_RotationSign < 0 && m_BoneRotation <= -45.0f)
	{
		m_RotationSign = 1;
	}
	else if (m_RotationSign > 0 && m_BoneRotation >= 45.0f)
	{
		m_RotationSign = -1;
	}

	m_pBone0->GetComponent<TransformComponent>()->Rotate(m_BoneRotation,0.0f,0.0f);
	m_pBone1->GetComponent<TransformComponent>()->Rotate(-m_BoneRotation * 2.0f, 0.0f, 0.0f);
}

void SoftwareSkinningScene_1::Draw(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
