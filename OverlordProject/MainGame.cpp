//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MainGame.h"
#include "Base\GeneralStructs.h"
#include "Scenegraph\SceneManager.h"
#include "Physx\PhysxProxy.h"
#include "Diagnostics\DebugRenderer.h"

#define W1
#define W2
#define W3
#define W6
#define W7


#define GAME

#ifdef W1
//#include "CourseObjects/Week 1/TestScene.h"
#include "CourseObjects/Week1/TestScene.h"
#include "CourseObjects/Week1/ComponentTestScene.h"
#endif

#ifdef W2
#include "./CourseObjects/Week2/ModelTestScene.h"
#include "CourseObjects/Week2/CharacterTestScene.h"
#endif

#ifdef W3
#include "CourseObjects/Week3/SpikeyScene.h"
#include "CourseObjects/Week3/SpriteTestScene.h"
#endif

#ifdef W6
#include "CourseObjects/Week6/SoftwareSkinningScene_1.h"
#endif

#ifdef W7
#include "CourseObjects/Week7/HardwareSkinningScene.h"
#endif


#ifdef GAME
#include "./MenuScene.h"
#include "CourseObjects/Game/Platformer.h"
#endif


MainGame::MainGame(void)
{
}


MainGame::~MainGame(void)
{
}

//Game is preparing
void MainGame::OnGamePreparing(GameSettings& gameSettings)
{
	UNREFERENCED_PARAMETER(gameSettings);
	//Nothing to do atm.
}

void MainGame::Initialize()
{

#ifdef W1
	//SceneManager::GetInstance()->AddGameScene(new TestScene());
	//SceneManager::GetInstance()->AddGameScene(new ComponentTestScene());

	//SceneManager::GetInstance()->SetActiveGameScene(L"TestScene");
#endif

#ifdef W2
	//SceneManager::GetInstance()->AddGameScene(new ModelTestScene());

	//SceneManager::GetInstance()->SetActiveGameScene(L"ModelTestScene");

	/*SceneManager::GetInstance()->AddGameScene(new CharacterTestScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"CharacterTestScene");*/

#endif

#ifdef W3
	//SceneManager::GetInstance()->AddGameScene(new SpikeyScene);
	//SceneManager::GetInstance()->AddGameScene(new SpriteTestScene);
#endif

#ifdef W6
	//SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_1);
#endif

#ifdef W7
	//SceneManager::GetInstance()->AddGameScene(new HardwareSkinningScene);
#endif

#ifdef GEOMSHADER
	//SceneManager::GetInstance()->AddGameScene(new BezierScene);
#endif

#ifdef GAME
	SceneManager::GetInstance()->AddGameScene(new MenuScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"MenuScene");
	SceneManager::GetInstance()->AddGameScene(new Platformer());
	//SceneManager::GetInstance()->SetActiveGameScene(L"Platformer");
#endif

}

LRESULT MainGame::WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
		case WM_KEYUP:
		{
			if ((lParam & 0x80000000) != 0x80000000)
				return -1;

			//NextScene
			//if (wParam == VK_F3)
			//{
			//	SceneManager::GetInstance()->NextScene();
			//	return 0;
			//}
			////PreviousScene
			//else if (wParam == VK_F2)
			//{
			//	SceneManager::GetInstance()->PreviousScene();
			//	return 0;
			//}
			if (wParam == VK_F4)
			{
				DebugRenderer::ToggleDebugRenderer();
				return 0;
			}
			/*else if (wParam == VK_F6)
			{
				auto activeScene = SceneManager::GetInstance()->GetActiveScene();
				activeScene->GetPhysxProxy()->NextPhysXFrame();
			}*/
		}
	}

	return -1;
}
