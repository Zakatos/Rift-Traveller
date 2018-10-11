#pragma once
#include "../OverlordEngine/Scenegraph/GameObject.h"

class Character;
class ModelComponent;
class ParticleEmitterComponent;

enum State
{
	floating = 0,
	flyingTowardsPlayer = 1,
	falling = 2
};

enum PickUpType
{
	Coin,
	Key
};

class Pickup final : public GameObject
{
public:
	Pickup(XMFLOAT3 pos,PickUpType type = Coin);
	virtual ~Pickup(void);

	void Initialize(const GameContext & gameContext);
	void Update(const GameContext & gameContext);
	void SetState(State state);
	void SetGoal(float x, float z);
	void SetCharacterRef(Character* character);
	PickUpType GetType() const { return m_Type; }
private:
	XMFLOAT3 m_Pos;
	bool m_AddForce;
	State m_State;
	PickUpType m_Type;
	float m_CurrFlyingTime;
	float m_TotalFlyingTime;
	bool m_HasBeenTriggered;
	ModelComponent * m_pModel;
	float m_GoalX;
	float m_GoalZ;
	float m_Speed;
	Character *m_pCharacter = nullptr;
	ParticleEmitterComponent* m_pParticleEmitter = nullptr;

	//Sound
	FMOD::Sound *m_pSound = nullptr;
	FMOD::Channel *m_pChannel = nullptr;
	//Methods
	bool AddForce(float elapsedSec);
	float GetDistance(const XMFLOAT3& v1, const XMFLOAT3& v2);
};

