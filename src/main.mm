#include "mainwindow.h"
#include "event_handler.hpp"
#include "resource_loader.hpp"
#include <QApplication>
#import "MouseEvent.h"
//考虑使用macOS的语音控制功能（voice control）
/// TODO 1. 添加动作表情控制
/// TODO 2. 添加聊天对话框，接入GPT https://kungptapi.openai.azure.com//openai/deployments/gpt35_turbo/chat/completions?api-version=2023-05-15
/// TODO 3. 优化模型管理，缩小内存的占用，似乎官方模型的内存占用很小,这个似乎是系统缓存？
/// TODO 4. 接入语音api，可以爬 https://genshinvoice.top
/// TODO 5. 获取实时播放音频的音量，这个似乎很难实现
/// TODO 6. 官方框架自带的json解析器似乎有问题，时而崩溃？需要排查一下。还有动画播放卡顿（Idle结束的时候，也需要排查）


int main(int argc, char *argv[]) {
    event_handler::get_instance();
    QApplication a(argc, argv);
    if (!resource_loader::get_instance().initialize()) {
        return 0;
    }
//    QSharedPointer<QAbstractNativeEventFilter> filter(new MyEventFilter);  // 创建自定义事件过滤器对象
//    const int mask = NSLeftMouseDownMask | NSLeftMouseUpMask | NSMouseMovedMask | NSLeftMouseDragged;  // 设置需要监听的事件类型
//    const int mask = NSEventMaskLeftMouseDown | NSEventMaskLeftMouseUp | NSEventMaskMouseMoved;
//    id monitorId = [NSEvent addGlobalMonitorForEventsMatchingMask:mask handler:^(NSEvent *event) {
//        filter->nativeEventFilter("NSEvent", event, 0);
//    }];
//    a.installNativeEventFilter(filter.data());
    MainWindow w(nullptr, &a);
    w.show();

    int x = QApplication::exec();
//    [NSEvent removeMonitor:monitorId];
    return x;
}
