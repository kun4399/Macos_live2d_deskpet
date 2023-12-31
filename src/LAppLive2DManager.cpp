﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppLive2DManager.hpp"
#include <string>
#include <GL/glew.h>
#include <Rendering/CubismRenderer.hpp>
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppDelegate.hpp"
#include "LAppModel.hpp"
#include "LAppView.hpp"
#include "resource_loader.hpp"
#include "event_handler.hpp"

using namespace Csm;
using namespace LAppDefine;
using namespace std;

namespace {
    LAppLive2DManager *s_instance = NULL;

    void FinishedMotion(ACubismMotion *self) {
        CF_LOG_INFO("Motion Finished: %s", (char *) self);
    }
}

LAppLive2DManager *LAppLive2DManager::GetInstance() {
    if (s_instance == NULL) {
        s_instance = new LAppLive2DManager();
    }

    return s_instance;
}

void LAppLive2DManager::ReleaseInstance() {
    if (s_instance != NULL) {
        delete s_instance;
    }

    s_instance = NULL;
}

LAppLive2DManager::LAppLive2DManager()
        : _viewMatrix(NULL) {
    _viewMatrix = new CubismMatrix44();

    //ChangeScene(_sceneIndex);
    auto m = resource_loader::get_instance().get_current_model();
    QByteArray model_name = m->name.toUtf8();
    if (!ChangeScene((Csm::csmChar *) model_name.data())) {
        CF_LOG_ERROR("current module load fail");
        event_handler::get_instance().report<QString>(msg_queue::message_type::app_current_model_load_fail, nullptr);
    }
}

LAppLive2DManager::~LAppLive2DManager() {
    ReleaseAllModel();
}

void LAppLive2DManager::ReleaseAllModel() {
    for (csmUint32 i = 0; i < _models.GetSize(); i++) {
        delete _models[(int) i];
    }

    _models.Clear();
}

LAppModel *LAppLive2DManager::GetModel(csmUint32 no) const {
    if (no < _models.GetSize()) {
        return _models[(int) no];
    }

    return nullptr;
}

void LAppLive2DManager::OnDrag(csmFloat32 x, csmFloat32 y) const {
    for (csmUint32 i = 0; i < _models.GetSize(); i++) {
        LAppModel *model = GetModel(i);

        model->SetDragging(x, y);
    }
}

void LAppLive2DManager::OnTap(csmFloat32 x, csmFloat32 y) {
    CF_LOG_DEBUG("tap point: {x:%.2f y:%.2f}", x, y);
    if (_models.GetSize() != 1) {
        CF_LOG_ERROR("model size is not 1: %d", _models.GetSize());
        return;
    }
    Csm::csmInt32 hit_area = _models[0]->HitTest(x, y);
    if (hit_area == -1) {
        return;
    }
    _models[0]->StartRandomMotionOrExpression(hit_area, FinishedMotion);

}

void LAppLive2DManager::OnUpdate() const {
    //int width, height;
    int width = LAppDelegate::GetInstance()->GetWindow()->width();
    int height = LAppDelegate::GetInstance()->GetWindow()->height();
    //glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &width, &height);

    CubismMatrix44 projection;
    csmUint32 modelCount = _models.GetSize();
    for (csmUint32 i = 0; i < modelCount; ++i) {
        LAppModel *model = GetModel(i);

        if (model->GetModel()->GetCanvasWidth() > 1.0f && width < height) {
            // 横に長いモデルを縦長ウィンドウに表示する際モデルの横サイズでscaleを算出する
            model->GetModelMatrix()->SetWidth(2.0f);
            projection.Scale(1.0f, static_cast<float>(width) / static_cast<float>(height));
        } else {
            projection.Scale(static_cast<float>(height) / static_cast<float>(width), 1.0f);
        }

        // 必要があればここで乗算
        if (_viewMatrix != nullptr) {
            projection.MultiplyByMatrix(_viewMatrix);
        }

        // モデル1体描画前コール
        LAppDelegate::GetInstance()->GetView()->PreModelDraw(*model);

        model->Update();
        model->Draw(projection);///< 参照渡しなのでprojectionは変質する

        // モデル1体描画後コール
        LAppDelegate::GetInstance()->GetView()->PostModelDraw(*model);
    }
}

//void LAppLive2DManager::NextScene()
//{
//    csmInt32 no = (_sceneIndex + 1) % ModelDirSize;
//   ChangeScene(no);
//}

bool LAppLive2DManager::ChangeScene(const QString &name) {
    //_sceneIndex = index;
    CF_LOG_DEBUG("model : %s", name.toStdString().c_str());
    QString modelPath = resource_loader::get_instance().get_resoures_path() + "/" + name + "/";
    QString modelJsonName = name + ".model3.json";
    ReleaseAllModel();
    _models.PushBack(new LAppModel());
    //_models[0]->LoadAssets(modelPath.c_str(), modelJsonName.c_str());
    if (!_models[0]->LoadAssets(modelPath.toUtf8().data(), modelJsonName.toUtf8().data())) {
        ReleaseAllModel();
        return false;
    }

    /*
     * モデル半透明表示を行うサンプルを提示する。
     * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
     * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
     */
    {
#if defined(USE_RENDER_TARGET)
        // LAppViewの持つターゲットに描画を行う場合、こちらを選択
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_ViewFrameBuffer;
#elif defined(USE_MODEL_RENDER_TARGET)
        // 各LAppModelの持つターゲットに描画を行う場合、こちらを選択
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_ModelFrameBuffer;
#else
        // デフォルトのメインフレームバッファへレンダリングする(通常)
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_None;
#endif

#if defined(USE_RENDER_TARGET) || defined(USE_MODEL_RENDER_TARGET)
        // モデル個別にαを付けるサンプルとして、もう1体モデルを作成し、少し位置をずらす
        _models.PushBack(new LAppModel());
        _models[1]->LoadAssets(modelPath.c_str(), modelJsonName.c_str());
        _models[1]->GetModelMatrix()->TranslateX(0.2f);
#endif

        LAppDelegate::GetInstance()->GetView()->SwitchRenderingTarget(useRenderTarget);

        // 別レンダリング先を選択した際の背景クリア色
        float clearColor[3] = {0.0f, 0.0f, 0.0f};
        LAppDelegate::GetInstance()->GetView()->SetRenderTargetClearColor(clearColor[0], clearColor[1], clearColor[2]);
    }
    return true;
}

csmUint32 LAppLive2DManager::GetModelNum() const {
    return _models.GetSize();
}

void LAppLive2DManager::SetViewMatrix(CubismMatrix44 *m) {
    for (int i = 0; i < 16; i++) {
        _viewMatrix->GetArray()[i] = m->GetArray()[i];
    }
}

void LAppLive2DManager::RobotControl(Csm::csmChar *motion_group, Csm::csmChar *expression,
                                     const std::shared_ptr<QByteArray> &sound) {
    CF_LOG_DEBUG("motion: %s, expression: %s", motion_group, expression);
    if (_models.GetSize() != 1) {
        CF_LOG_ERROR("model size is %d", _models.GetSize());
        return;
    }
    if (expression != nullptr && _models[0]->ExpressionExists(expression)) {
        CF_LOG_DEBUG("expression : %s start", expression);
        _models[0]->SetExpression(expression);
    }
    if (motion_group != nullptr && _models[0]->MotionGroupExists(motion_group)) {
        CF_LOG_DEBUG("motion : %s start", motion_group);
        _models[0]->StartMotion(motion_group, 0, PriorityForce, FinishedMotion, sound);
    } else if (sound) {
        CF_LOG_DEBUG("motion : Idle start");
        _models[0]->StartMotion("Idle", 0, PriorityForce, FinishedMotion, sound);
    }
}
