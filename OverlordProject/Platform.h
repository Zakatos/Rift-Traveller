#pragma once
#include "Scenegraph/GameObject.h"

class Character;

enum PlatformState
{
	idle = 0,
	flyingTowardsGoal = 1,
	FloatingTowardsStart = 2
};


class Platform : public GameObject
{
public:
	Platform(XMFLOAT3 pos);
	virtual ~Platform(void);
	virtual void Initialize(const GameContext & gameContext);
	virtual void Update(const GameContext & gameContext);
	void SetGoal(XMFLOAT3 pos);
	void SetSecondaryGoal(XMFLOAT3 pos);
	void SetState(PlatformState state);
private:
	XMFLOAT3 m_Pos;
	XMFLOAT3 m_StartingPos;
	XMFLOAT3 m_SecondaryGoal;
	bool m_HasReachedGoal;
	bool m_AddForce;
	//State m_State;
	float m_CurrFlyingTime;
	float m_TotalFlyingTime;
	XMFLOAT3 m_Goal;
	float m_Speed;
	PlatformState m_State;
	void AddForce(float elapsedSec);
	void AddForce(float elapsedSec,GameObject* object);
	float GetDistance(const XMFLOAT3& v1, const XMFLOAT3& v2);
};

