/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDefine.hpp"
#include <CubismFramework.hpp>

namespace LAppDefine {

    using namespace Csm;

    // 画面
    const csmFloat32 ViewScale = 1.0f;
    const csmFloat32 ViewMaxScale = 2.0f;
    const csmFloat32 ViewMinScale = 0.8f;

    const csmFloat32 ViewLogicalLeft = -1.0f;
    const csmFloat32 ViewLogicalRight = 1.0f;
    const csmFloat32 ViewLogicalBottom = -1.0f;
    const csmFloat32 ViewLogicalTop = -1.0f;

    const csmFloat32 ViewLogicalMaxLeft = -2.0f;
    const csmFloat32 ViewLogicalMaxRight = 2.0f;
    const csmFloat32 ViewLogicalMaxBottom = -2.0f;
    const csmFloat32 ViewLogicalMaxTop = 2.0f;

    // 相対パス
    const csmChar* ResourcesPath = "/Users/gaoyukun/github/qf/Resources/";

    // モデルの後ろにある背景の画像ファイル
    const csmChar* BackImageName = "back_class_normal.png";
    // 歯車
    const csmChar* GearImageName = "icon_gear.png";
    // 終了ボタン
    const csmChar* PowerImageName = "close.png";

    // 模型定义------------------------------------------
    // モデルを配置したディレクトリ名の配列 放置了模型的目录名称的数组
    // ディレクトリ名とmodel3.jsonの名前を一致させておくこと 请确保目录名称与model3.json文件的名称相匹配。
    //const csmChar* ModelDir[] = {
        //"Haru",
        //"mingshi",
        //"cbjms",
        //"Mark",
        //"Natori",
        //"Rice"
    //};
    //const csmInt32 ModelDirSize = sizeof(ModelDir) / sizeof(const csmChar*);

    // 外部定義ファイル(json)と合わせる 与外部定义文件（json）相匹配
    const csmChar* MotionGroupIdle = "Idle"; // アイドリング 偶像化
//    const csmChar* MotionGroupTapBody = "TapBody"; // 当你轻触身体时
    const csmChar* MotionGroupTapBody = "TapArea1";
    const csmChar* MotionGroupTap = "Tap";


    // 与外部定义文件（json）配合使用
    const csmChar* HitAreaNameHead = "Head";
//    const csmChar* HitAreaNameBody = "Body";
    const csmChar* HitAreaNameBody="Area1";

    // モーションの優先度定数
    const csmInt32 PriorityNone = 0;
    const csmInt32 PriorityIdle = 1;
    const csmInt32 PriorityNormal = 2;
    const csmInt32 PriorityForce = 3;

    // デバッグ用ログの表示オプション
#ifdef QF_DEBUG
    const csmBool DebugLogEnable = true;
#else
    const csmBool DebugLogEnable = false;
#endif
    const csmBool DebugTouchLogEnable = false;

    // Frameworkから出力するログのレベル設定
    const CubismFramework::Option::LogLevel CubismLoggingLevel = CubismFramework::Option::LogLevel_Verbose;

    // デフォルトのレンダーターゲットサイズ
    const csmInt32 RenderTargetWidth = 1900;
    const csmInt32 RenderTargetHeight = 1000;
}
