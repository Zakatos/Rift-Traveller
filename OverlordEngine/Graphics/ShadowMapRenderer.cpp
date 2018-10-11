#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "../Base/OverlordGame.h"
#include "../Graphics/MeshFilter.h"
#include "../Scenegraph/SceneManager.h"


ShadowMapRenderer::ShadowMapRenderer()
{
	m_pShadowMat = new ShadowMapMaterial();
	m_pSkinnedShadowMat = new SkinnedShadowMapMaterial();
}


ShadowMapRenderer::~ShadowMapRenderer()
{
	SafeDelete(m_pShadowRT);
	SafeDelete(m_pShadowMat);
	SafeDelete(m_pSkinnedShadowMat);
}

void ShadowMapRenderer::Init(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		UNREFERENCED_PARAMETER(gameContext);
		m_pShadowMat->Initialize(gameContext);
		m_pSkinnedShadowMat->Initialize(gameContext);

		RENDERTARGET_DESC target_desc;
		target_desc.Height = OverlordGame::GetGameSettings().Window.Height;
		target_desc.Width = OverlordGame::GetGameSettings().Window.Width;
		target_desc.EnableDepthSRV = true;
		target_desc.EnableColorSRV = true;
		m_pShadowRT = new RenderTarget(gameContext.pDevice);

		m_pShadowRT->Create(target_desc);
	}
}


void ShadowMapRenderer::Begin(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);

	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	//set rendertarget shadow map light
	m_pShadowRT->Clear(gameContext, Colors::Yellow);
	m_pShadowMat->SetLight(m_LightVP);
	m_pSkinnedShadowMat->SetLight(m_LightVP);
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, XMFLOAT4X4 world)
{
	UNREFERENCED_PARAMETER(world);
	//use meshfilter to fill vertex buffer
	//m_pShadowMat->SetWorld(world);
	VertexBufferData vbdata;
	if (pMeshFilter->m_HasAnimations)
		vbdata = pMeshFilter->GetVertexBufferData(gameContext, m_pSkinnedShadowMat);
	else
		vbdata = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat);

	//set topo
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//set input layout
	if (pMeshFilter->m_HasAnimations)
		gameContext.pDeviceContext->IASetInputLayout(m_pSkinnedShadowMat->GetInputLayout());
	else
		gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->GetInputLayout());

	//seindexbuffer
	gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//set vert buffer
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &vbdata.pVertexBuffer, &vbdata.VertexStride, &offset);

	if (pMeshFilter->m_HasAnimations)
	{
		D3DX11_TECHNIQUE_DESC techDesc;
		m_pSkinnedShadowMat->GetTechnique()->GetDesc(&techDesc);
		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			m_pSkinnedShadowMat->GetTechnique()->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
			gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
		}
	}
	else
	{
		D3DX11_TECHNIQUE_DESC techDesc;
		m_pShadowMat->GetTechnique()->GetDesc(&techDesc);
		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			m_pShadowMat->GetTechnique()->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
			gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
		}
	}

}

void ShadowMapRenderer::End(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
}

XMFLOAT3 ShadowMapRenderer::GetLightDirection()
{
	return m_LightDirection;
}

XMFLOAT4X4 ShadowMapRenderer::GetLightVP()
{
	return m_LightVP;
}

ShadowMapMaterial* ShadowMapRenderer::GetMaterial()
{
	return m_pShadowMat;
}

SkinnedShadowMapMaterial* ShadowMapRenderer::GetSkinnedMaterial()
{
	return m_pSkinnedShadowMat;
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap()
{
	return m_pShadowRT->GetDepthShaderResourceView();
}

void ShadowMapRenderer::SetLight(XMFLOAT3 position, XMFLOAT3 direction)
{
	m_LightDirection = direction;
	m_LightPosition = position;

	XMMATRIX projection, view;

	float viewWidth = (m_Size>0) ? m_Size * OverlordGame::GetGameSettings().Window.AspectRatio*2.0f : OverlordGame::GetGameSettings().Window.Width;
	float viewHeight = (m_Size>0) ? m_Size *2.0f: OverlordGame::GetGameSettings().Window.Height;
	projection = XMMatrixOrthographicLH(viewWidth, viewHeight,0.1f, 1500.0f);
	//projection = XMMatrixPerspectiveLH(viewWidth, viewHeight, 250.f, 2500.0f);
	XMVECTOR worldPosition = XMLoadFloat3(&position);
	XMVECTOR lookAt = XMLoadFloat3(&direction);
	XMFLOAT3 up = { 0,1,0 };
	XMVECTOR upVec = XMLoadFloat3(&up);
	//calculate right with up then recalculate up

	view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	auto viewproj = view * projection;
	XMStoreFloat4x4(&m_LightVP, viewproj);
}
