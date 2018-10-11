#include "stdafx.h"
#include "MainCharacter.h"
#include "Components/Components.h"
#include "../../Materials/SkinnedDiffuseMaterial.h"
#include "Graphics\ModelAnimator.h"
#include "../Week2/Character.h"
#include "Level.h"
#include "Platformer.h"
#include "Base/SoundManager.h"

MainCharacter::MainCharacter(Character * chara):
	m_pCharacter(chara),
	m_State(MainCharacterState::idle),
	m_StateLastFrame(MainCharacterState::idle),
	m_CanShootProjectile(true),
	m_CanKick(true),
	m_CooldownCurr(0.0f),
	m_CooldownTotal(1.0f),
	m_StunnedTimer(1.0f),
	m_CurrStunnedTime(0.0f)
{
}


MainCharacter::~MainCharacter()
{
}

void MainCharacter::Initialize(const GameContext & gameContext)
{

	m_pModelObject = new GameObject();


	auto model = new ModelComponent(L"Resources/Meshes/Lola.ovm");

	auto pDiffuseMaterial = new SkinnedDiffuseMaterial();
	pDiffuseMaterial->SetDiffuseTexture(L"Resources/Textures/lola_diffuse.png");
	gameContext.pMaterialManager->AddMaterial(pDiffuseMaterial, UINT(3));

	model->SetMaterial(3);

	m_pModelObject->AddComponent(model);

	GetTransform()->Scale(0.04f, 0.04f, 0.04f);
	

	m_pModelObject->GetTransform()->Rotate(0.0f, 180.0f, 0.0f);

	AddChild(m_pModelObject);

	//INPUT
	gameContext.pInput->AddInputAction(InputAction(50, Down, 'E'));
	//gameContext.pInput->AddInputAction(InputAction(50, Down, XINPUT_GAMEPAD_X));
	gameContext.pInput->AddInputAction(InputAction(51, Down, 'K'));

	//DUST PARTICLE
	m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/FX_Dash_Tex_D.png", 20);
	m_pParticleEmitter->SetVelocity(XMFLOAT3(0.0f, 0.3f, 0.0f));
	m_pParticleEmitter->SetMinSize(1.0f);
	m_pParticleEmitter->SetMaxSize(3.0f);
	m_pParticleEmitter->SetMinEnergy(1.0f);
	m_pParticleEmitter->SetMaxEnergy(2.0f);
	m_pParticleEmitter->SetMinSizeGrow(3.5f);
	m_pParticleEmitter->SetMaxSizeGrow(6.5f);
	m_pParticleEmitter->SetMinEmitterRange(0.2f);
	m_pParticleEmitter->SetMaxEmitterRange(0.5f);
	m_pParticleEmitter->SetColor(XMFLOAT4(229.0f/255.0f, 184.0f/255.0f, 136.f/255.0f, 0.6f));
	AddComponent(m_pParticleEmitter);

	
	//Sounds
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD_RESULT fmodres;

	fmodres = fmodSystem->createSound("./Resources/Sounds/Effects/386043__taramg__jumpgroan002.wav", FMOD_DEFAULT, nullptr, &m_pJumpSound);
	SoundManager::ErrorCheck(fmodres);

	fmodres = fmodSystem->createSound("./Resources/Sounds/Effects/Magic Smite.wav", FMOD_DEFAULT, nullptr, &m_pProjectileSound);
	SoundManager::ErrorCheck(fmodres);

	fmodres = fmodSystem->createSound("./Resources/Sounds/Effects/377560__yudena__argh-woman-bymondfisch89.mp3", FMOD_DEFAULT, nullptr, &m_pHitSound);
	SoundManager::ErrorCheck(fmodres);
	
}

void MainCharacter::Update(const GameContext & gameContext)
{
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD_RESULT fmodres;
	m_StateLastFrame = m_State;
	
	m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->Play();

	if (m_State == MainCharacterState::stunned)
	{
		
		m_CurrStunnedTime += gameContext.pGameTime->GetElapsed();

		if (m_CurrStunnedTime >= m_StunnedTimer)
		{
			m_CurrStunnedTime = 0.0f;
			m_State = MainCharacterState::idle;
			return;
		}

		if (m_pCharacter->GetState() == Character::State::running)
		{
			if (m_State != MainCharacterState::magicattack)m_State = MainCharacterState::stunned;

			auto velocity = m_pCharacter->GetVelocity();

			GetTransform()->Translate(m_pCharacter->GetTransform()->GetPosition().x, m_pCharacter->GetTransform()->GetPosition().y - 5.0f, m_pCharacter->GetTransform()->GetPosition().z);


			XMVECTOR tempVel = XMLoadFloat3(&velocity);

			tempVel = XMVector3Normalize(tempVel);

			XMStoreFloat3(&velocity, tempVel);

			float angle = (atan2(velocity.x, velocity.z) * 180 / XM_PI) + 180.f;

			GetTransform()->Rotate(0.0f, angle - 180, 0.0f);

			//m_pParticleEmitter->GetTransform()->Translate(GetTransform()->GetWorldPosition());
		}
		
		else
		{
			if (m_State != MainCharacterState::magicattack)m_State = MainCharacterState::stunned;

		}
	}

	else
	{
		if (m_pCharacter->GetState() == Character::State::running)
		{
			if (m_State != MainCharacterState::magicattack)m_State = MainCharacterState::running;

			auto velocity = m_pCharacter->GetVelocity();

			GetTransform()->Translate(m_pCharacter->GetTransform()->GetPosition().x, m_pCharacter->GetTransform()->GetPosition().y - 5.0f, m_pCharacter->GetTransform()->GetPosition().z);


			XMVECTOR tempVel = XMLoadFloat3(&velocity);

			tempVel = XMVector3Normalize(tempVel);

			XMStoreFloat3(&velocity, tempVel);

			float angle = (atan2(velocity.x, velocity.z) * 180 / XM_PI) + 180.f;

			GetTransform()->Rotate(0.0f, angle - 180, 0.0f);
		}
		else
		{
			if (m_State != MainCharacterState::magicattack)m_State = MainCharacterState::idle;

		}

		if (m_pCharacter->GetState() == Character::State::jumping)
		{
			fmodres = fmodSystem->playSound(m_pJumpSound, 0, false, &m_pJumpChannel);
			GetTransform()->Translate(m_pCharacter->GetTransform()->GetPosition().x, m_pCharacter->GetTransform()->GetPosition().y -5.0f , m_pCharacter->GetTransform()->GetPosition().z);
			m_State = jump;
		}
		
		if(m_State == idle)
		{
			m_pParticleEmitter->SetVelocity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			m_pParticleEmitter->SetMinSize(0.0f);
			m_pParticleEmitter->SetMaxSize(0.0f);
			m_pParticleEmitter->SetMinEnergy(0.0f);
			m_pParticleEmitter->SetMaxEnergy(0.0f);
			m_pParticleEmitter->SetMinSizeGrow(0.0f);
			m_pParticleEmitter->SetMaxSizeGrow(0.0f);
			m_pParticleEmitter->SetMinEmitterRange(0.0f);
			m_pParticleEmitter->SetMaxEmitterRange(0.0f);
			
		}
		else
		{
			m_pParticleEmitter->SetVelocity(XMFLOAT3(0.0f, 0.3f, 0.0f));
			m_pParticleEmitter->SetMinSize(1.0f);
			m_pParticleEmitter->SetMaxSize(3.0f);
			m_pParticleEmitter->SetMinEnergy(1.0f);
			m_pParticleEmitter->SetMaxEnergy(2.0f);
			m_pParticleEmitter->SetMinSizeGrow(3.5f);
			m_pParticleEmitter->SetMaxSizeGrow(6.5f);
			m_pParticleEmitter->SetMinEmitterRange(0.2f);
			m_pParticleEmitter->SetMaxEmitterRange(0.5f);
			m_pParticleEmitter->SetColor(XMFLOAT4(229.0f / 255.0f, 184.0f / 255.0f, 136.f / 255.0f, 0.6f));
		}
		if (gameContext.pInput->IsActionTriggered(50))
		{
			//spawn Projectile

			auto scene = static_cast<Platformer*>(GetScene());

			if (scene)
			{
				auto level = static_cast<Level*>(scene->GetLevel());

				if (level)
				{
					if (m_CanShootProjectile)
					{
						level->SpawnProjectile(GetTransform()->GetPosition(), GetTransform()->GetForward());
						m_CanShootProjectile = false;
						m_State = MainCharacterState::magicattack;
					}
				}
			}
		}

		if (!m_CanShootProjectile)
		{
			m_State = MainCharacterState::magicattack;

			m_CooldownCurr += gameContext.pGameTime->GetElapsed();

			if (m_CooldownCurr >= m_CooldownTotal)
			{
				m_CooldownCurr = 0.0f;
				m_CanShootProjectile = true;
				m_State = MainCharacterState::idle;
			}
		}

	}
	if (gameContext.pInput->IsActionTriggered(51))
	{	
		if (m_CanKick)
		{		
			m_CanKick = false;
			m_State = MainCharacterState::kick;
		}
	}
	if (!m_CanKick)
	{
		m_State = MainCharacterState::kick;

		m_CooldownCurr += gameContext.pGameTime->GetElapsed();

		if (m_CooldownCurr >= m_CooldownTotal)
		{
			m_CooldownCurr = 0.0f;
			m_CanKick = true;
			m_State = MainCharacterState::idle;
		}
	}

	if(m_State == idle)
	{
		GetTransform()->Translate(m_pCharacter->GetTransform()->GetPosition().x, m_pCharacter->GetTransform()->GetPosition().y-5.0f, m_pCharacter->GetTransform()->GetPosition().z);
	}

	if (m_State != m_StateLastFrame)
	{
		switch (m_State)
		{
		case MainCharacter::idle:
			m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(2);
			break;
		case MainCharacter::running:
			m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(8);
			break;
		case MainCharacter::stunned:
			fmodres = fmodSystem->playSound(m_pHitSound, nullptr, false, &m_pHitChannel);
			m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(1);
			break;
		case MainCharacter::magicattack:
			fmodres = fmodSystem->playSound(m_pProjectileSound, nullptr, false, &m_pProjectileChannel);
			m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(5);
			break;
		case MainCharacter::jump:
			m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(8);
			break;
		case MainCharacter::punch:
			m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(6);
			break;
		case MainCharacter::kick:
			m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(4);
			break;
		case MainCharacter::dying:
			m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(5);
			break;
		default:
			break;
		}
	}
	
}

void MainCharacter::PostInit()
{
	m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(2);
	
}

void MainCharacter::RemoveLife()
{
	--m_Lives;

	if (m_Lives <= 0)
	{
		//tell scene that I'm dead
		auto scene = dynamic_cast<Platformer*>(GetScene());

		scene->SetDead();
	}
}

void MainCharacter::SetLife(int life)
{
	m_Lives = life;
}

void MainCharacter::SetStunned()
{
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD_RESULT fmodres;
	fmodres = fmodSystem->playSound(m_pHitSound, nullptr, false, &m_pHitChannel);
	m_State = MainCharacterState::stunned;
	m_pModelObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(1);
}

bool MainCharacter::GetStunned()
{
	if (m_State == MainCharacterState::stunned)
	{
		return true;
	}

	 return false;
}
