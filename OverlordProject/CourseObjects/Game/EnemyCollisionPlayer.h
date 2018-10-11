#pragma once
#include "Scenegraph/GameObject.h"
#include "Enemy.h"

class Character;
class Enemy;

class EnemyCollisionPlayer final : public GameObject
{
public:
	EnemyCollisionPlayer();
	virtual ~EnemyCollisionPlayer();

	void Initialize(const GameContext & gameContext);
	void Update(const GameContext & gameContext);

	const bool GetIsFollowing() const 
	{
		auto parent = static_cast<Enemy*>(GetParent());
		return parent->GetIsFollowing();
	}
	void SetIsFollowing(bool isFollowing) 
	{ 
		auto parent = static_cast<Enemy*>(GetParent());
		parent->SetIsFollowing(isFollowing);
		
	}
	void SetCharacterReference(Character* character);	
};

