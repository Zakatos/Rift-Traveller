//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ModelTestScene.h"
#include "Scenegraph/GameObject.h"
#include "Components/Components.h"
#include "Content/ContentManager.h"
#include "Physx/PhysxManager.h"

#include "../OverlordProject/Materials/ColorMaterial.h"
#include "../OverlordProject/Materials/DiffuseMaterial.h"


ModelTestScene::ModelTestScene(void) :
	GameScene(L"ModelTestScene"),
	m_pChair(nullptr)
{
}


ModelTestScene::~ModelTestScene(void)
{
}

void ModelTestScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(ground);

	//CHAIR OBJECT
	//************
	m_pChair = new GameObject();

	//1. Attach a modelcomponent (chair.ovm)
	auto pChairMesh = new ModelComponent(L"Resources/Meshes/Chair.ovm");
	m_pChair->AddComponent(pChairMesh);

	//2. Create a ColorMaterial and add it to the material manager
	auto pColorMaterial = new ColorMaterial();
	gameContext.pMaterialManager->AddMaterial(pColorMaterial, UINT(0));

	//3. Assign the material to the previous created modelcomponent
	// Build and Run
	pChairMesh->SetMaterial(UINT(1));
	//AddChild(m_pChair);

	//4. Create a DiffuseMaterial (using PosNormTex3D.fx)
	auto pDiffuseMaterial = new DiffuseMaterial();
	pDiffuseMaterial->SetDiffuseTexture(L"Resources/Textures/Chair_Dark.dds");
	gameContext.pMaterialManager->AddMaterial(pDiffuseMaterial, UINT(1));
	//		Make sure you are able to set a texture (DiffuseMaterial::SetDiffuseTexture(const wstring& assetFile))
	//		Load the correct shadervariable and set it during the material variable update
	//5. Assign the material to the modelcomponent
	// Build and Run
	m_pChair->GetComponent<ModelComponent>()->SetMaterial(1);
	AddChild(m_pChair);
	//6. Attach a rigidbody component (pure-dynamic)
	//auto pRigidBody = new RigidBodyComponent();
	//m_pChair->AddComponent(pRigidBody);
	//7. Attach a collider component (Use a PxConvexMeshGeometry [chair.ovpc])
	
	// Build and Run

}

void ModelTestScene::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ModelTestScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
