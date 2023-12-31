﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <Type/csmRectF.hpp>
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>
#include "QByteArray"
#include "LAppWavFileHandler.hpp"

/**
 * @brief ユーザーが実際に使用するモデルの実装クラス<br>
 *         モデル生成、機能コンポーネント生成、更新処理とレンダリングの呼び出しを行う。
 *用户实际使用的模型实现类，执行模型生成、功能组件生成、更新处理和渲染调用。
 */
class LAppModel : public Csm::CubismUserModel
{
public:
    /**
     * @brief コンストラクタ
     */
    LAppModel();

    /**
     * @brief デストラクタ
     *
     */
    ~LAppModel() override;

    /**
     * @brief model3.jsonが置かれたディレクトリとファイルパスからモデルを生成する
     * 从包含model3.json的目录和文件路径生成模型。
     */
    bool LoadAssets(const Csm::csmChar* dir, const  Csm::csmChar* fileName);

    /**
     * @brief レンダラを再構築する
     *
     */
    void ReloadRenderer();

    /**
     * @brief   モデルの更新処理。モデルのパラメータから描画状態を決定する。
     *
     */
    void Update();

    /**
     * @brief   モデルを描画する処理。モデルを描画する空間のView-Projection行列を渡す。
     *
     * @param[in]  matrix  View-Projection行列
     */
    void Draw(Csm::CubismMatrix44& matrix);

    /**
     * @brief   引数で指定したモーションの再生を開始する。开始播放通过参数指定的动作
     *
     * @param[in]   group                       モーショングループ名 动作组名称
     * @param[in]   no                          グループ内の番号 小组内的编号
     * @param[in]   priority                    優先度
     * @param[in]   onFinishedMotionHandler     在动作播放结束时调用的回调函数。如果为NULL，则不会被调用。
     * @return                                  返回开始的动作的识别号码。这个号码将被用作判断个别动作是否已经结束的IsFinished()函数的参数。如果无法开始，则返回"-1"。
     */
    Csm::CubismMotionQueueEntryHandle StartMotion(const Csm::csmChar *group, Csm::csmInt32 no, Csm::csmInt32 priority,
                                                  Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = nullptr,
                                                  std::shared_ptr<QByteArray> sound= nullptr);

    /**
     * @brief   开始播放随机选择的动作。
     *
     * @param[in]   group                       モーショングループ名
     * @param[in]   priority                    優先度
     * @param[in]   onFinishedMotionHandler     モーション再生終了時に呼び出されるコールバック関数。NULLの場合、呼び出されない。
     * @return                                  開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」
     */
    Csm::CubismMotionQueueEntryHandle StartRandomMotion(const Csm::csmChar* group, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = nullptr);

    /**
     * @brief   引数で指定した表情モーションをセットする 设置由参数指定的表情动作。
     *
     * @param   expressionID    表情モーションのID
     */
    void SetExpression(const Csm::csmChar* expressionID);

    /**
     * @brief   ランダムに選ばれた表情モーションをセットする
     *          设置随机选择的表情动作。
     */
    void SetRandomExpression();

    /**
    * @brief   イベントの発火を受け取る 接收事件的触发
    *
    */
    void MotionEventFired(const Live2D::Cubism::Framework::csmString& eventValue) override;

    /**
     * @brief    当たり判定テスト。<br>
     *            指定IDの頂点リストから矩形を計算し、座標が矩形範囲内か判定する。
     *
     * @param[in]   hitAreaName     当たり判定をテストする対象のID
     * @param[in]   x               判定を行うX座標
     * @param[in]   y               判定を行うY座標
     */
    Csm::csmBool HitTest(const Csm::csmChar* hitAreaName, Csm::csmFloat32 x, Csm::csmFloat32 y);

    /**
     * @brief   別ターゲットに描画する際に使用するバッファの取得
     */
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2& GetRenderBuffer();

    /**
     * @brief  判断是否碰撞到了HitArea
     * @return 返回碰撞到的HitArea的index。如果没有碰撞到，则返回-1。
     */
    Csm::csmInt32 HitTest(Csm::csmFloat32 x, Csm::csmFloat32 y);

    Csm::csmBool StartRandomMotionOrExpression(Csm::csmInt32 hit_area_index, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = nullptr);

    Csm::csmBool ExpressionExists(const Csm::csmChar* expressionID) const;

    Csm::csmBool MotionGroupExists(const Csm::csmChar* motion_group) const;

protected:
    /**
     *  @brief  モデルを描画する処理。モデルを描画する空間のView-Projection行列を渡す。
     *
     */
    void DoDraw();

private:
    /**
     * @brief model3.jsonからモデルを生成する。<br>
     *         model3.jsonの記述に従ってモデル生成、モーション、物理演算などのコンポーネント生成を行う。
     *
     * @param[in]   setting     ICubismModelSettingのインスタンス
     *
     */
    bool SetupModel(Csm::ICubismModelSetting* setting);

    /**
     * @brief OpenGLのテクスチャユニットにテクスチャをロードする
     *
     */
    void SetupTextures();

    /**
     * @brief   モーションデータをグループ名から一括でロードする。<br>
     *           モーションデータの名前は内部でModelSettingから取得する。
     *
     * @param[in]   group  モーションデータのグループ名
     */
    void PreloadMotionGroup(const Csm::csmChar* group);

    /**
     * @brief   モーションデータをグループ名から一括で解放する。<br>
     *           モーションデータの名前は内部でModelSettingから取得する。
     *
     * @param[in]   group  モーションデータのグループ名
     */
    void ReleaseMotionGroup(const Csm::csmChar* group) const;

    /**
    * @brief すべてのモーションデータの解放
    *
    * すべてのモーションデータを解放する。
    */
    void ReleaseMotions();

    /**
    * @brief すべての表情データの解放
    *
    * すべての表情データを解放する。
    */
    void ReleaseExpressions();

    Csm::ICubismModelSetting* _modelSetting; ///< 模型设置信息
    Csm::csmString _modelHomeDir; ///< 模型设置所在的目录
    Csm::csmFloat32 _userTimeSeconds; ///< デルタ時間の積算値[秒]
    Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds; ///< モデルに設定されたまばたき機能用パラメータID
    Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds; ///< モデルに設定されたリップシンク機能用パラメータID
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions; ///< 読み込まれているモーションのリスト
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions; ///< 読み込まれている表情のリスト
    Csm::csmVector<Csm::csmRectF> _hitArea;
    Csm::csmVector<Csm::csmRectF> _userArea;
    const Csm::CubismId* _idParamAngleX; ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamAngleY; ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamAngleZ; ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamBodyAngleX; ///< パラメータID: ParamBodyAngleX
    const Csm::CubismId* _idParamEyeBallX; ///< パラメータID: ParamEyeBallX
    const Csm::CubismId* _idParamEyeBallY; ///< パラメータID: ParamEyeBallXY

    LAppWavFileHandler _wavFileHandler; ///< wavファイルハンドラ

    Csm::Rendering::CubismOffscreenFrame_OpenGLES2  _renderBuffer;   ///< フレームバッファ以外の描画先
};



