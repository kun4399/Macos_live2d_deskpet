/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */
#pragma once

#include <CubismFramework.hpp>

/**
* @brief  Sample Appで使用する定数 在示例应用程序中使用的常量
*
*/
namespace LAppDefine {

    using namespace Csm;

    extern const csmFloat32 ViewScale;              ///< 拡大縮小率
    extern const csmFloat32 ViewMaxScale;           ///< 拡大縮小率の最大値
    extern const csmFloat32 ViewMinScale;           ///< 拡大縮小率の最小値

    extern const csmFloat32 ViewLogicalLeft;        ///< 論理的なビュー座標系の左端の値
    extern const csmFloat32 ViewLogicalRight;       ///< 論理的なビュー座標系の右端の値
    extern const csmFloat32 ViewLogicalBottom;      ///< 論理的なビュー座標系の下端の値
    extern const csmFloat32 ViewLogicalTop;         ///< 論理的なビュー座標系の上端の値

    extern const csmFloat32 ViewLogicalMaxLeft;     ///< 論理的なビュー座標系の左端の最大値
    extern const csmFloat32 ViewLogicalMaxRight;    ///< 論理的なビュー座標系の右端の最大値
    extern const csmFloat32 ViewLogicalMaxBottom;   ///< 論理的なビュー座標系の下端の最大値
    extern const csmFloat32 ViewLogicalMaxTop;      ///< 論理的なビュー座標系の上端の最大値

//    extern const csmChar* ResourcesPath;           ///< 素材パス
    extern const csmChar* BackImageName;         ///< 背景画像ファイル 背景图像文件
    extern const csmChar* GearImageName;         ///< 歯車画像ファイル 齿轮图像文件
    extern const csmChar* PowerImageName;        ///< 終了ボタン画像ファイル 终止按钮图像文件

    // モデル定義--------------------------------------------
    //extern const csmChar* ModelDir[];               ///< モデルを配置したディレクトリ名の配列. ディレクトリ名とmodel3.jsonの名前を一致させておく.
    //extern const csmInt32 ModelDirSize;             ///< モデルディレクトリ配列のサイズ

                                                    // 外部定義ファイル(json)と合わせる 与外部定义文件(json)相匹配
    extern const csmChar* MotionGroupIdle;          ///< アイドリング時に再生するモーションのリスト 播放闲置时的动作列表
    extern const csmChar* MotionGroupTapBody;       ///< 体をタップした時に再生するモーションのリスト 当点击身体时播放的动作列表
    extern const csmChar* MotionGroupTap;
                                                    // 外部定義ファイル(json)と合わせる
    extern const csmChar* HitAreaNameHead;          ///< 当たり判定の[Head]タグ 碰撞判定的[Head]标签
    extern const csmChar* HitAreaNameBody;          ///< 当たり判定の[Body]タグ

                                                    // モーションの優先度定数
    extern const csmInt32 PriorityNone;             ///< モーションの優先度定数: 0
    extern const csmInt32 PriorityIdle;             ///< モーションの優先度定数: 1
    extern const csmInt32 PriorityNormal;           ///< モーションの優先度定数: 2
    extern const csmInt32 PriorityForce;            ///< モーションの優先度定数: 3

                                                    // デバッグ用ログの表示
    extern const csmBool DebugLogEnable;            ///< デバッグ用ログ表示の有効・無効 调试用日志显示的启用/禁用
    extern const csmBool DebugTouchLogEnable;       ///< タッチ処理のデバッグ用ログ表示の有効・無効

    // Frameworkから出力するログのレベル設定
    extern const CubismFramework::Option::LogLevel CubismLoggingLevel;

    // デフォルトのレンダーターゲットサイズ
    extern const csmInt32 RenderTargetWidth;
    extern const csmInt32 RenderTargetHeight;

}
