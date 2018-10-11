#pragma once
#include "Scenegraph/GameObject.h"

class Level;
class ParticleEmitterComponent;

class Projectile final : public GameObject
{
public:
	Projectile(XMFLOAT3 startPos, XMFLOAT3 forwardVec, Level* pLevel);
	virtual ~Projectile();

	void Initialize(const GameContext & gameContext);
	void Update(const GameContext & gameContext);

	void SetIsAlive(bool isAlive) { m_IsAlive = isAlive; }
	const bool GetIsAlive() const { return m_IsAlive; }

private:
	XMFLOAT3 m_StartPos;
	XMFLOAT3 m_ForwardVec;

	float m_Speed;

	float m_TimeAlive;
	float m_TotalTimeAlive;

	bool m_IsAlive;

	Level* m_pLevel;

	ParticleEmitterComponent* m_pParticleEmitter = nullptr;
};

