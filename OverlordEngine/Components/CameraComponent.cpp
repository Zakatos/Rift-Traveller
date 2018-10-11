//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "CameraComponent.h"
#include "../Base/OverlordGame.h"
#include "../Components/TransformComponent.h"
#include "../Diagnostics/Logger.h"
#include "../Physx/PhysxManager.h"
#include "../Physx/PhysxProxy.h"
#include "../Scenegraph/GameObject.h"
#include "../Scenegraph/GameScene.h"
#include "../Base/GeneralStructs.h"


CameraComponent::CameraComponent(void):
	m_FOV(XM_PIDIV4),
	m_NearPlane(0.1f),
	m_FarPlane(2500.0f),
	m_Size(25.0f),
	m_PerspectiveProjection(true),
	m_IsActive(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}


CameraComponent::~CameraComponent(void)
{
}

void CameraComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void CameraComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto windowSettings = OverlordGame::GetGameSettings().Window;
	XMMATRIX projection, view, viewInv, viewProjectionInv;

	if(m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio ,m_NearPlane, m_FarPlane);
	}
	else
	{
		float viewWidth = (m_Size>0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		float viewHeight = (m_Size>0) ? m_Size : windowSettings.Height;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	viewInv = XMMatrixInverse(nullptr, view);
	viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void CameraComponent::SetActive()
{
	auto gameObject = GetGameObject();
	if(gameObject == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game object is null");
		return;
	}

	auto gameScene = gameObject->GetScene();
	if(gameScene == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game scene is null");
		return;
	}

	gameScene->SetActiveCamera(this);
}

GameObject* CameraComponent::Pick(const GameContext& gameContext, CollisionGroupFlag ignoreGroups) const
{
	//TODO
	auto mousePos = gameContext.pInput->GetMousePosition();
	XMFLOAT2 halfViewportDimensions;
	halfViewportDimensions.y = 0.5f * OverlordGame::GetGameSettings().Window.Height;
	halfViewportDimensions.x = 0.5f * OverlordGame::GetGameSettings().Window.Width;

	auto Xndc = (mousePos.x - halfViewportDimensions.x) / halfViewportDimensions.x;
	auto Yndc = (halfViewportDimensions.y - mousePos.y) / halfViewportDimensions.y;

	XMMATRIX VPI = XMLoadFloat4x4(&m_ViewProjectionInverse);
	XMFLOAT4 nearPointFloat = { Xndc, Yndc, 0, 0 };
	XMFLOAT4 farPointFloat = { Xndc, Yndc, 1, 0 };
	XMVECTOR nearPointVector = XMLoadFloat4(&nearPointFloat);
	XMVECTOR farPointVector = XMLoadFloat4(&farPointFloat);
	XMVECTOR nearPoint = XMVector3TransformCoord(nearPointVector, VPI);
	XMVECTOR farPoint = XMVector3TransformCoord(farPointVector, VPI);
	XMFLOAT3 nearP;
	XMStoreFloat3(&nearP, nearPoint);
	XMFLOAT3 farP;
	XMStoreFloat3(&farP, farPoint);

	PxQueryFilterData filterData;
	filterData.data.word0 = ~ignoreGroups;

	PxRaycastBuffer hit;
	auto PhysXProxy = m_pGameObject->GetScene()->GetPhysxProxy();

	auto origin = PxVec3(nearP.x, nearP.y, nearP.z);
	auto direction = PxVec3(farP.x - nearP.x, farP.y - nearP.y, farP.z - nearP.z);
	direction.normalize();

	if (PhysXProxy->Raycast(origin, direction, PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
	{
		auto actor = hit.getAnyHit(0).actor;

		if (actor)
		{
			auto ud = actor->userData;
			if (ud != nullptr)
			{
				return reinterpret_cast<BaseComponent*>(ud)->GetGameObject();
			}
		}
	}
	return nullptr;
}