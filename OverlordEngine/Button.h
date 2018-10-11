#pragma once
#include "../OverlordEngine/Scenegraph/GameObject.h"
class Button : public GameObject
{
public:
	Button(const wstring& spriteAsset, PxVec2 position);
	virtual ~Button();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

	bool MouseClicked() {
		if (m_Clicked)
		{
			m_Clicked = false;
			return true;
		}
		return false;
	}

	void SetPressed(bool pressed) { m_Pressed = pressed; }
	void SetActive(bool active) { m_IsActive = active; }
	void SetVisible(bool visible) { m_isVisible = visible; }
private:
	bool m_Pressed = false;
	bool m_IsActive = true;
	bool m_isVisible = true;
	PxVec2 m_Position;
	int m_Width;
	int m_Height;
	TextureData* m_pTexture = nullptr;
	bool m_Clicked = false;
	XMFLOAT4 m_Color = static_cast<XMFLOAT4>(Colors::White);
};

