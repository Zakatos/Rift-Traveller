#pragma once
#include "Scenegraph/GameObject.h"

class ControllerComponent;
class CameraComponent;

class Character : public GameObject
{
public:
	enum CharacterMovement : UINT
	{
		LEFT = 0,
		RIGHT,
		FORWARD,
		BACKWARD,
		JUMP,
		PUNCH,
		KICK
	};

	enum State
	{
		running,
		idle,
		jumping
	};

	Character(float radius = 2, float height = 5, float moveSpeed = 100);
	virtual ~Character();
	virtual void Initialize(const GameContext& gameContext);
	virtual void PostInitialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	void SetPos(XMFLOAT3 pos);
	XMFLOAT3 GetVelocity() { return m_Velocity; }
	bool GetIsJumping() { return m_IsJumping; }
	State GetState() { return m_State; }

	void SetInput(bool input) { m_IsRecivingInput = input; }
protected:

	CameraComponent* m_pCamera ;
	ControllerComponent* m_pController;

	float m_TotalPitch, m_TotalYaw;
	float m_MoveSpeed, m_RotationSpeed;
	float m_Radius, m_Height;

	//Running
	float m_MaxRunVelocity, 
		m_TerminalVelocity,
		m_Gravity, 
		m_RunAccelerationTime, 
		m_JumpAccelerationTime, 
		m_RunAcceleration, 
		m_JumpAcceleration, 
		m_RunVelocity, 
		m_JumpVelocity;

	XMFLOAT3 m_Velocity;

	State m_State;
	bool m_IsJumping = false;
	bool m_IsRecivingInput = true;
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	Character(const Character& t);
	Character& operator=(const Character& t);
};


