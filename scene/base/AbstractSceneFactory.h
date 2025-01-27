/*********************************************************************
 * \file   AbstractSceneFactory.h
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#pragma once
#include <memory>
#include "BaseScene.h"

///=============================================================================
///						抽象シーンファクトリ
class AbstractSceneFactory {
public:
    virtual ~AbstractSceneFactory() = default;

    /// @brief シーンを作成する純粋仮想関数
    /// @param sceneNo シーン番号
    /// @return 作成されたシーン
    virtual std::unique_ptr<BaseScene> CreateScene(int sceneNo) = 0;
};