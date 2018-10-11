#pragma once
#include <Scenegraph/GameScene.h>

class ControllerComponent;

class CharacterTestScene final : public GameScene
{
public:
	CharacterTestScene();
	~CharacterTestScene();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void SceneActivated() {}
	void SceneDeactivated() {}

private:



private:
	CharacterTestScene(const CharacterTestScene &obj) = delete;
	CharacterTestScene& operator=(const CharacterTestScene& obj) = delete;
};

