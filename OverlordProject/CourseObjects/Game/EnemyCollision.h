#pragma once
#include "Scenegraph/GameObject.h"

class Enemy;

class EnemyCollision final : public GameObject
{
public:
	EnemyCollision();
	virtual ~EnemyCollision();

	void Initialize(const GameContext & gameContext);
	void Update(const GameContext & gameContext);

	void Kill();
};

