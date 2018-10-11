#include "stdafx.h"
#include "BoneObject.h"
#include "../Components/Components.h"
#include "../Physx/PhysxProxy.h"
#include "../Scenegraph/GameObject.h"



BoneObject::BoneObject(int boneId, int materialId, float length):
	m_BoneId(boneId),
	m_Length(length),
	m_MaterialId(materialId)
{
}


BoneObject::~BoneObject()
{
}

void BoneObject::AddBone(BoneObject * pBone)
{
	pBone->GetComponent<TransformComponent>()->Translate(0.0f, 0.0f, -m_Length);


	AddChild(pBone);
}

XMFLOAT4X4 BoneObject::GetBindPose()
{
	return m_BindPose;
}

void BoneObject::CalculateBindPose()
{
	XMStoreFloat4x4(&m_BindPose, XMMatrixInverse(nullptr, XMLoadFloat4x4(&GetTransform()->GetWorld())));
}

void BoneObject::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto boneModel = new ModelComponent(L"./Resources/Meshes/Bone.ovm");
	
	boneModel->SetMaterial(3);

	auto emptyGameObj = new GameObject();

	emptyGameObj->AddComponent(boneModel);
	
	emptyGameObj->GetComponent<TransformComponent>()->Scale(m_Length, m_Length, m_Length);

	AddChild(emptyGameObj);
}
