/*********************************************************************
 * \file   SceneFactory.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
#include "AbstractSceneFactory.h"

///=============================================================================
///						シーン工場
class SceneFactory : public AbstractSceneFactory {
	///--------------------------------------------------------------
	///							メンバ関数
	/**----------------------------------------------------------------------------
  * \brief  CreateScene シーンの生成
  * \param  sceneNo シーン番号
  * \return シーン
  */
	std::unique_ptr<BaseScene> CreateScene(int sceneNo) override;
};
