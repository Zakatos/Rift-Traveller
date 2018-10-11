//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "Character.h"
#include "Components\Components.h"
#include "Prefabs\Prefabs.h"
#include "Physx/PhysxManager.h"

Character::Character(float radius, float height, float moveSpeed) :
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0),
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	//Running
	m_MaxRunVelocity(50.0f),
	m_TerminalVelocity(100),
	m_Gravity(250.0f),
	m_RunAccelerationTime(0.3f),
	m_JumpAccelerationTime(0.8f),
	m_RunAcceleration(m_MaxRunVelocity / m_RunAccelerationTime),
	m_JumpAcceleration(m_Gravity / m_JumpAccelerationTime),
	m_RunVelocity(0),
	m_JumpVelocity(0),
	m_Velocity(0, 0, 0),
	m_State(State::idle)
{
}


Character::~Character(void)
{
}

void Character::Initialize(const GameContext& gameContext)
{
	//UNREFERENCED_PARAMETER(gameContext);
	// Create controller
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto bouncyMaterial = physX->createMaterial(0, 0, 1);

	m_pController = new ControllerComponent(bouncyMaterial);
	AddComponent(m_pController);
	// Add a fixed camera as child
	auto pFixedCamera = new FixedCamera();
	m_pCamera = new CameraComponent();
	pFixedCamera->AddComponent(m_pCamera);
	AddChild(pFixedCamera);
	
	//AddChild(pFixedCamera);
	//m_pCamera = pFixedCamera->GetComponent<CameraComponent>();

	
	// Register all Input Actions
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::FORWARD, Down, 'W'));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::BACKWARD, Down, 'S'));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::RIGHT, Down, 'D'));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::LEFT, Down, 'A'));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::JUMP, Down, VK_SPACE));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::PUNCH, Down, 'K'));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::KICK, Down, 'L'));
	//gameContext.pInput->AddInputAction(InputAction(CharacterMovement::FORWARD, Pressed, -1,-1, XINPUT_GAMEPAD_LEFT_THUMB));
	//gameContext.pInput->AddInputAction(InputAction(CharacterMovement::BACKWARD, Pressed,-1, -1,XINPUT_GAMEPAD_LEFT_THUMB));
	//gameContext.pInput->AddInputAction(InputAction(CharacterMovement::RIGHT, Pressed, -1, -1, XINPUT_GAMEPAD_LEFT_THUMB));
	//gameContext.pInput->AddInputAction(InputAction(CharacterMovement::LEFT, Pressed, -1, -1, XINPUT_GAMEPAD_LEFT_THUMB));
	//gameContext.pInput->AddInputAction(InputAction(CharacterMovement::JUMP, Down, -1, -1, XINPUT_GAMEPAD_A));
	

	GetTransform()->Translate(-6.0f, 5.0f, -50.0f);
}

void Character::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	// Set the camera as active
	m_pCamera = GetChild<FixedCamera>()->GetComponent<CameraComponent>();
	m_pCamera->SetActive();

	// We need to do this in the PostInitialize because child game objects only get initialized 
	//after the Initialize of the current object finishes

}


void Character::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	
	//std::cout << GetTransform()->GetPosition().x << " " << GetTransform()->GetPosition().y << " " << GetTransform()->GetPosition().z << std::endl;
	if (m_pCamera->IsActive() && m_IsRecivingInput)
	{
		XMFLOAT3 move = XMFLOAT3(0, 0, 0);
		move.z = gameContext.pInput->IsActionTriggered(FORWARD) ? 1.0f : 0.0f;
		if (move.z == 0) move.z = gameContext.pInput->IsActionTriggered(BACKWARD) ? -1.0f : 0.0f;

		//std::cout << gameContext.pInput->GetThumbstickPosition().x << " " << gameContext.pInput->GetThumbstickPosition().y << std::endl;

		move.x = gameContext.pInput->IsActionTriggered(RIGHT) ? 1.0f : 0.0f;
		if (move.x == 0) move.x = gameContext.pInput->IsActionTriggered(LEFT) ? -1.0f : 0.0f;

		if (gameContext.pInput->IsActionTriggered(JUMP))
		{
			m_TerminalVelocity = 100.0f;
			m_Gravity = 250.0f;
			move.y = 1.0f;
			m_State = State::jumping;
		}
		
		XMFLOAT2 look = XMFLOAT2(0, 0);
		auto mouseMove = gameContext.pInput->GetMouseMovement();
		look.x = static_cast<float>(mouseMove.x);
		look.y = static_cast<float>(mouseMove.y);

		//Calculate Transforms
		XMVECTOR currPos = XMLoadFloat3(&GetTransform()->GetPosition());
		XMFLOAT3 forward = GetTransform()->GetForward();
		XMFLOAT3 right = GetTransform()->GetRight();
		auto elapsed = gameContext.pGameTime->GetElapsed();

		if (move.x != 0 || move.z != 0)
		{
			m_RunVelocity += m_RunAcceleration * elapsed;
			if (m_RunVelocity >= m_MaxRunVelocity)
			{
				m_RunVelocity = m_MaxRunVelocity;
			}

			m_Velocity.x = ((forward.x * move.z) + (right.x * move.x)) * m_RunVelocity;
			m_Velocity.z = ((forward.z * move.z) + (right.z * move.x)) * m_RunVelocity;

			m_State = State::running;
		}
		else
		{
			m_Velocity.x = 0.0f;
			m_Velocity.z = 0.0f;

			m_State = State::idle;
		}

		XMFLOAT3 currPosFloat3;
		XMStoreFloat3(&currPosFloat3, currPos);
		if (!m_pController->GetCollisionFlags().isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
		{
			m_JumpVelocity = m_JumpAcceleration * elapsed;
			m_Velocity.y -= m_JumpVelocity;
			
			if (m_Velocity.y <= -m_TerminalVelocity)
			{
				m_Velocity.y = -m_TerminalVelocity;
			}
			
		}
		else if (move.y != 0)
		{
			m_JumpVelocity = 0;
			m_Velocity.y = 100;
			m_State = State::jumping;
			
		}
		else
		{
			m_Velocity.y = 0;
		}

		auto tempVel = XMFLOAT3(m_Velocity.x * elapsed, m_Velocity.y * elapsed, m_Velocity.z * elapsed);
		m_pController->Move(tempVel);

		m_TotalYaw += look.x * m_RotationSpeed * elapsed;
		m_TotalPitch += look.y * m_RotationSpeed * elapsed;
		if (m_TotalPitch > 100.0f) m_TotalPitch = 100.0f;
		if (m_TotalPitch < 5.0f) m_TotalPitch = 5.0f;
		GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);
		
	}

}

void Character::SetPos(XMFLOAT3 pos)
{
	GetTransform()->Translate(XMFLOAT3{ pos });
}