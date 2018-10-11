#include "stdafx.h"
#include "Button.h"
#include "../Graphics/SpriteRenderer.h"
#include "../Components/Components.h"
#include "../Components/SpriteComponent.h"
#include "../Content/ContentManager.h"


Button::Button(const wstring& spriteAsset, PxVec2 position) :
	m_Position(position)
{
	m_pTexture = ContentManager::Load<TextureData>(spriteAsset);
	m_Width = int(m_pTexture->GetDimension().x);
	m_Height = int(m_pTexture->GetDimension().y);
}


Button::~Button()
{
}

void Button::Initialize(const GameContext& gameContext)
{

	if (m_Pressed)
		gameContext.pInput->AddInputAction(InputAction(10, Pressed, -1, VK_LBUTTON));
	else
		gameContext.pInput->AddInputAction(InputAction(10, Down, -1, VK_LBUTTON));
}

void Button::Update(const GameContext& gameContext)
{
	if (m_IsActive)
	{
		if (m_Pressed)
			m_Clicked = false;

		auto mousePos = gameContext.pInput->GetMousePosition();
		if (mousePos.x > m_Position.x && mousePos.x < m_Position.x + m_Width
			&& mousePos.y > m_Position.y && mousePos.y < m_Position.y + m_Height)
		{
			m_Color = static_cast<XMFLOAT4>(Colors::Gray);
			if (gameContext.pInput->IsActionTriggered(10))
			{
				m_Clicked = true;
			}
		}
		else
		{
			m_Color = static_cast<XMFLOAT4>(Colors::White);
		}
	}
}

void Button::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (m_isVisible)
	{
		SpriteRenderer::GetInstance()->Draw(m_pTexture, XMFLOAT2(m_Position.x, m_Position.y), m_Color);
	}
}
