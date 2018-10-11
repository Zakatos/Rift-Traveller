//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "PostProcessingMaterial.h"
#include "../Graphics/RenderTarget.h"
#include "../Base/OverlordGame.h"
#include "../Content/ContentManager.h"

ID3D11Buffer* PostProcessingMaterial::m_pVertexBuffer = nullptr;
unsigned int PostProcessingMaterial::m_VertexBufferStride = sizeof(VertexPosTex);

ID3D11InputLayout *PostProcessingMaterial::m_pInputLayout = nullptr;
vector<ILDescription> PostProcessingMaterial::m_pInputLayoutDescriptions = vector<ILDescription>();
UINT PostProcessingMaterial::m_pInputLayoutSize = 0;
UINT PostProcessingMaterial::m_InputLayoutID = 0;

PostProcessingMaterial::PostProcessingMaterial(wstring effectFile, const wstring& technique)
	: m_IsInitialized(false),
	m_pEffect(nullptr),
	m_effectFile(effectFile),
	m_pRenderTarget(nullptr),
	m_pTechnique(nullptr),
	m_TechniqueName(technique)
{
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	//Delete/Release NON-STATIC objects
	if (m_pRenderTarget)
	{
		SafeDelete(m_pRenderTarget);
	}
}

void PostProcessingMaterial::Destroy()
{
	//Delete/Release/Clear STATIC objects/lists

	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pInputLayout);
	m_pInputLayoutDescriptions.clear();
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		//1. LoadEffect (LoadEffect(...))
		LoadEffect(gameContext, m_effectFile);
		//2. CreateInputLaytout (CreateInputLayout(...))
		CreateInputLayout(gameContext, m_pTechnique);
		//   CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad) 
		//no need for an indexbuffer :D
		CreateVertexBuffer(gameContext);
		//3. Create RenderTarget (m_pRenderTarget)
		RENDERTARGET_DESC target_desc;
		target_desc.Height = OverlordGame::GetGameSettings().Window.Height;
		target_desc.Width = OverlordGame::GetGameSettings().Window.Width;
		target_desc.EnableColorSRV = true;
		m_pRenderTarget = new RenderTarget(gameContext.pDevice);
		m_pRenderTarget->Create(target_desc);
		//		Take a look at the class, figure out how to initialize/create a RenderTarget Object
		//		GameSettings > OverlordGame::GetGameSettings()
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const wstring& effectFile)
{
	UNREFERENCED_PARAMETER(gameContext);

	//Load Effect through ContentManager
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);

	if (m_TechniqueName.size() > 0)
	{
		auto techString = string(m_TechniqueName.begin(), m_TechniqueName.end());
		m_pTechnique = m_pEffect->GetTechniqueByName(techString.c_str());
	}
	else
	{
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	}

	//if (!EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutSize))
	//	Logger::LogError(L"PostProcessingMaterial::CreateInputLayout >> BuildInputLayout failed!");


	//Check if m_TechniqueName (default constructor parameter) is set

	// If SET > Use this Technique (+ check if valid)
	// If !SET > Use Technique with index 0

	//Call LoadEffectVariables
	LoadEffectVariables();

	return true;
}

//Static Function
void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	//1. Clear the object's RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
	float rgb[4] = { 0,0,0,1 };
	m_pRenderTarget->Clear(gameContext, rgb);
	//2. Call UpdateEffectVariables(...)
	UpdateEffectVariables(previousRendertarget, gameContext);
	//3. Set InputLayout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	//4. Set VertexBuffer
	UINT stride = sizeof(VertexPosTex);
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	//5. Set PrimitiveTopology (TRIANGLESTRIP!!)
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//6. Draw (NOT Indexed!)
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(NUM_VERTS, 0);
	}

	//7. Unbind the PreviousRendertarget's ShaderResource view from the pipeline. Because we'll need to use it as RenderTarget later on. 
	//   A resource can't be bound as RenderTarget & ShaderResource at the same time. See what happens when you comment these last lines.

	// Unbind SRV from PS-Stage > So it can be used as a RenderTarget later on
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
}

//Static Function
void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	//Static Buffer Object (Created once, shared between other PPMaterials)
	if (m_pVertexBuffer)
		return;

	//Create a vertex buffer for a full screen quad. Use the VertexPosTex struct (m_pVertexBuffer)
	std::vector<VertexPosTex> vertices;
	vertices.push_back(VertexPosTex({ -1,-1,0 }, { 0,1 }));
	vertices.push_back(VertexPosTex({ -1,1,0 }, { 0,0 }));
	vertices.push_back(VertexPosTex({ 1,-1,0 }, { 1,1 }));
	vertices.push_back(VertexPosTex({ 1,1,0 }, { 1,0 }));

	D3D11_BUFFER_DESC buffer_desc = {};
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.ByteWidth = sizeof(VertexPosTex) * NUM_VERTS;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	initData.pSysMem = vertices.data();
	HRESULT hr = gameContext.pDevice->CreateBuffer(&buffer_desc, &initData, &m_pVertexBuffer);
	Logger::LogHResult(hr, L"Failed to Create Vertexbuffer");
	//Primitive Topology: TriangleStrip
	//Vertex Amount: NUM_VERTS (4)
	//Vertex Struct: VertexPosTex
	//Use NDC to define your vertex positions

	//+ Check for errors (HRESULT)
	//Set 'm_VertexBufferStride' (Size in bytes of VertexPosTex)
	m_VertexBufferStride = sizeof(VertexPosTex);
	// Create vertex array containing three elements in system memory

}

void PostProcessingMaterial::CreateInputLayout(const GameContext& gameContext, ID3DX11EffectTechnique* pTechnique)
{
	//Static InputLayout Object (Created once, shared between other PPMaterials)
	if (m_pInputLayout)
		return;

	//Create a simple InputLayout for these effects
	//2 Elements, POSITION & TEXCOORD (Check PP-Shaders)
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = sizeof(vertexDesc) / sizeof(vertexDesc[0]);

	D3DX11_PASS_DESC passDesc;
	pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	auto hr = gameContext.pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);
	Logger::LogHResult(hr, L"Failed to Create InputLayout");
	//+ Check for errors (HRESULT)

}

RenderTarget* PostProcessingMaterial::GetRenderTarget() const
{
	return m_pRenderTarget;
}