#include "GL/glew.h"
#import <Cocoa/Cocoa.h>
#import <QtGui/QWindow>
#include <QAbstractNativeEventFilter>
#include <ApplicationServices/ApplicationServices.h>
class MouseEventControl : public QObject{
public:
    explicit MouseEventControl(QObject *parent = nullptr);
    static void EnableMousePassThrough(WId windowId, bool enable);
    bool startMonitoring();
    bool stopMonitoring();
private:
    CFMachPortRef eventTap;
    CFRunLoopRef ref;
    bool is_monitoring{false};

    // 静态回调函数（实际上可以直接用handleMouseEvent函数作为回调函数，毕竟可以声明为静态，但是为了以后的扩展性，还是分开写了）
    static CGEventRef mouseEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon);
    CGEventRef handleMouseEvent(CGEventType type, CGEventRef event);
};

//class MyEventFilter : public QAbstractNativeEventFilter {
//    ///// 自定义的本地事件过滤器类，用于处理鼠标事件
//public:
//    // 重写nativeEventFilter函数，用于处理本地事件
//    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
//};