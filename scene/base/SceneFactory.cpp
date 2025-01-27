/*********************************************************************
 * \file   SceneFactory.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#include "SceneFactory.h"
#include <memory>
#include "Logger.h"
using namespace Logger;
//========================================
// privateシーン
#include "DebugScene.h"
// publicシーン
#include "GamePlayScene.h"
#include "TitleScene.h"
#include "ClearScene.h"

///=============================================================================
///						
std::unique_ptr<BaseScene> SceneFactory::CreateScene(int SceneNo) {
	//========================================
	// 次のシーンの生成
	std::unique_ptr<BaseScene> nextScene = nullptr;

	//========================================
	// シーン名によって生成するシーンを変更
	if(SceneNo == SCENE::DEBUG) {
		nextScene = std::make_unique<DebugScene>();
	} else if(SceneNo == SCENE::GAMEPLAY) {
		nextScene = std::make_unique<GamePlayScene>();
	} else if(SceneNo == SCENE::TITLE) {
		nextScene = std::make_unique<TitleScene>();
	} else if(SceneNo == SCENE::CLEAR) {
		nextScene = std::make_unique<ClearScene>();
	} else {
		Log("シーン名が不正です", LogLevel::Error);
	}

	//========================================
	// シーンの出力
	return nextScene;
}
