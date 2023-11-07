#include <GL/glew.h> // 这个必须要放在这里的第一行，否则会报错
#include "mainwindow.h"
#include "event_handler.hpp"
#include "resource_loader.hpp"
#include <QApplication>
#include <AppKit/NSApplication.h>
#include <QAbstractNativeEventFilter>
#include <QDebug>
#import <AppKit/AppKit.h>
#include "LAppDelegate.hpp"
//考虑使用macOS的语音控制功能（voice control）
/// TODO 1. 添加动作表情控制
/// TODO 2. 添加聊天对话框，接入GPT https://kungptapi.openai.azure.com//openai/deployments/gpt35_turbo/chat/completions?api-version=2023-05-15
/// TODO 3. 优化模型管理，缩小内存的占用，似乎官方模型的内存占用很小
/// TODO 4. 接入语音api，可以爬 https://genshinvoice.top
/// TODO 5. 获取实时播放音频的音量，这个似乎很难实现

// 自定义的本地事件过滤器类，用于处理鼠标事件
class MyEventFilter : public QAbstractNativeEventFilter {
public:
    // 重写nativeEventFilter函数，用于处理本地事件
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override {
        Q_UNUSED(eventType)  // 防止未使用的编译器警告
        Q_UNUSED(result)  // 防止未使用的编译器警告

        NSEvent *event = (NSEvent *) message;  // 将事件消息转换为Objective-C的NSEvent对象
        NSPoint location = [event locationInWindow]; // 返回的是相对于窗口左上角的坐标
        auto &resource = resource_loader::get_instance();
        int x = location.x;
        int y = resource.current_model_y - location.y - 6; // 总是差6，可能是因为窗口的标题栏的高度
        // 根据事件类型执行不同的操作
        switch ([event type]) {
            case NSLeftMouseDown:
//                qDebug() << "NSLeftMouseDown";
                LAppDelegate::GetInstance()->mousePressEvent(x, y);
                break;
            case NSLeftMouseUp:
                LAppDelegate::GetInstance()->mouseReleaseEvent(x, y);
                break;
            case NSMouseMoved:
//                qDebug() << "NSMouseMoved";
                LAppDelegate::GetInstance()->mouseMoveEvent(x, y);
                break;
            case NSLeftMouseDragged:
                LAppDelegate::GetInstance()->mouseMoveEvent(x, y);
//                qDebug() << "NSLeftMouseDragged";
                break;
            default:
                return false;  // 对于其他事件类型，不处理，继续传递给下一个事件过滤器
        }
        return false;  // 返回false表示未完全处理事件，继续传递给下一个事件过滤器
    }
};


int main(int argc, char *argv[]) {
    if (!event_handler::get_instance().initialize()) {
        return 0;
    }

    if (!resource_loader::get_instance().initialize()) {
        return 0;
    }
    QApplication a(argc, argv);
    QSharedPointer<QAbstractNativeEventFilter> filter(new MyEventFilter);  // 创建自定义事件过滤器对象
    const int mask =
            NSLeftMouseDownMask | NSLeftMouseUpMask | NSMouseMovedMask | NSLeftMouseDragged;  // 设置需要监听的事件类型

    id monitorId = [NSEvent addGlobalMonitorForEventsMatchingMask:mask handler:^(NSEvent *event) {
        filter->nativeEventFilter("NSEvent", event, 0);
    }];
//    NSPoint mouseLocation = [NSEvent mouseLocation]; 这个可以获取到鼠标的坐标，也许能派上用场。留个眼。
    a.installNativeEventFilter(filter.data());
    MainWindow w(nullptr, &a);
    w.show();
    int x = QApplication::exec();
    [NSEvent removeMonitor:monitorId];
    return x;
}
