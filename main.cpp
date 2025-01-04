/*********************************************************************
 * \file   main.cpp
 * \brief  メインファイル
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#include "MaruRhythm.h"
 ///--------------------------------------------------------------
 ///						 標準ライブラリ
#include <memory> // std::unique_ptr

///=============================================================================
///						Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	///--------------------------------------------------------------
	///						 MaruRhythmクラス
	std::unique_ptr<MaruRhythm> maruRhythm = std::make_unique<MaruRhythm>();
	//初期化
	maruRhythm->Initialize();

	///=============================================================================
	///						 メインループ
	MSG msg{};
	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//========================================
			// ゲーム更新
			maruRhythm->Update();

			//========================================
			// 終了リクエストがあれば終了
			if(maruRhythm->IsEndRequest()) {
				break;
			}

			//========================================
			// ゲーム描画
			maruRhythm->Draw();
		}
	}
	//========================================
	// ゲームの終了処理
	maruRhythm->Finalize();

	return 0;
}
