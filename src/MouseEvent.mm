#include <GL/glew.h> // 这个必须要放在这里的第一行，否则会报错
#import "MouseEvent.h"
#include "LAppDelegate.hpp"

void MouseControl::enableMousePassThrough(WId windowId, bool enable) {
    auto qtNSView = reinterpret_cast<NSView *> (windowId);
    auto window = [qtNSView window];
    if (enable) {
        [window setIgnoresMouseEvents:YES];
    } else {
        [window setIgnoresMouseEvents:NO];
    }
}

bool MyEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) {
    Q_UNUSED(eventType)  // 防止未使用的编译器警告
    Q_UNUSED(result)  // 防止未使用的编译器警告

    auto event = (NSEvent *) message;  // 将事件消息转换为Objective-C的NSEvent对象
    NSPoint mouseLocation = [NSEvent mouseLocation];
    auto &resource = resource_loader::get_instance();
    int x = (int)mouseLocation.x - resource.current_model_x;
    int y = (resource.screen_height-(int)resource.current_model_y)-(int)mouseLocation.y;
    // 根据事件类型执行不同的操作
    switch ([event type]) {
        case NSEventTypeLeftMouseDown:
//            qDebug() << "NSLeftMouseDown:x:" << x << "y:" << y;
            LAppDelegate::GetInstance()->mouseReleaseEvent(x, y);
//            LAppDelegate::GetInstance()->mousePressEvent(x, y);
            break;
        case NSEventTypeLeftMouseUp:
//            LAppDelegate::GetInstance()->mouseReleaseEvent(x, y);
            LAppDelegate::GetInstance()->mousePressEvent(x, y);
            break;
        case NSEventTypeMouseMoved:
            LAppDelegate::GetInstance()->mouseMoveEvent(x, y);
            break;
        default:
            return false;  // 对于其他事件类型，不处理，继续传递给下一个事件过滤器
    }
    return false; //为了触发qt的事件，必须返回false
}
