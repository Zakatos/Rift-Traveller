#pragma once
#include "Scenegraph/GameObject.h"
#include "Pickup.h"

class Projectile;
class Character;
class Hud;
class ModelComponent;
class GameObject;

class Level final : public GameObject
{
public:
	Level(void);
	virtual ~Level(void);

	virtual void Initialize(const GameContext & gameContext);
	virtual void Update(const GameContext & gameContext);

	void SpawnProjectile(XMFLOAT3 startPos, XMFLOAT3 forwardVec);
	void DeleteProjectile(Projectile* projectile);
	void DeletePickup(Pickup* pickupToDelete);	
	void SpawnPickup(XMFLOAT3 pos, PickUpType type);
	void ResetLevel();
	void PostInit();
	void RemoveLifePlayer();
	void AddPoints();
	Hud* GetHud() const  { return m_pHud; }
	bool GetIsExitActivated() const { return m_IsExitActivated; }
private:
	friend class Enemy;

	void InitLevel(const GameContext & gameContext);
	void InitItems(const GameContext & gameContext);
	void InitPickUps(const GameContext & gameContext);
	void InitEnemies(const GameContext & gameContext);
	void InitPlatforms(const GameContext & gameContext);
	void InitColliders(const GameContext & gameContext);
	static void SetWaterTrigger(GameObject* triggerobject, GameObject* otherobject, TriggerAction action);
	static void ItemTrigger(GameObject* triggerobject, GameObject* otherobject, TriggerAction action);
	static void EnemyTrigger(GameObject* triggerobject, GameObject* otherobject, TriggerAction action);
	static void ProjectileTrigger(GameObject* triggerobject, GameObject* otherobject, TriggerAction action);
	
	std::vector<Enemy*> m_pEnemyVec;
	std::vector<Pickup*> m_pPickupVec;
	ModelComponent* m_pLevel;
	Hud * m_pHud;
	bool m_IsExitActivated = false;
};

