#pragma once
#include "Scenegraph/GameObject.h"

class SpriteFont;
class TextureData;
class SpriteComponent;

class Hud final : public GameObject
{
public:
	Hud();
	virtual ~Hud();

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);
	void IncreaseScoreCoins();
	void SetScoreCoins(int newScore);
	void IncreaseScore();
	void SetScore(int newScore);
	void DecreaseLives();
	void SetLives(int newLives);
	void IncreaseKeys();
	void SetKeys(int keysnumber);
	void SetObjectiveTime(float time);
	void SetObjectiveActive(bool completed) {  m_IsCompleted = completed; }
	int GetKeys() const { return m_Keys; }
private:
	SpriteFont * m_pRetroFont = nullptr;
	SpriteComponent* m_pKeysSprite = nullptr;
	float m_CurrentTimeObjShown = 0.0f;
	float m_MaxTimeShown =3.0f;
	float m_CurrentTimeShown = 0.0f;
	int m_NumCoins = 0;
	int m_Score = 0;
	int m_Lives = 5;
	int m_Keys = 0;
	bool m_IsCompleted = false;
};

