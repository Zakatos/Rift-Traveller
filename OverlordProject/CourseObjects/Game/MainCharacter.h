#pragma once
#include "Scenegraph/GameObject.h"

class Character;
class ModelComponent;
class ParticleEmitterComponent;

class MainCharacter final : public GameObject
{
public:

	enum MainCharacterState : UINT
	{
		idle,
		running,
		stunned,
		magicattack,
		punch,
		kick,
		jump,
		dying
		
	};

	MainCharacter(Character * chara);
	virtual ~MainCharacter();

	void Initialize(const GameContext & gameContext);
	void Update(const GameContext & gameContext);

	void PostInit();

	void RemoveLife();
	void SetLife(int life);

	void SetStunned();
	bool GetStunned();


private:

	Character * m_pCharacter;
	
	MainCharacterState m_State;
	MainCharacterState m_StateLastFrame;

	bool m_CanShootProjectile;
	bool m_CanKick;
	float m_CooldownTotal;
	float m_CooldownCurr;

	GameObject * m_pModelObject = nullptr;

	ParticleEmitterComponent* m_pParticleEmitter = nullptr;

	int m_Lives = 5;

	float m_StunnedTimer;
	float m_CurrStunnedTime;

	//Sound
	FMOD::Sound *m_pJumpSound = nullptr, *m_pProjectileSound = nullptr, *m_pHitSound = nullptr;
	FMOD::Channel *m_pJumpChannel = nullptr,  *m_pProjectileChannel = nullptr, *m_pHitChannel = nullptr;
};

