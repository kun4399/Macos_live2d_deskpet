#import <Cocoa/Cocoa.h>
#import <QtGui/QWindow>
#include <QAbstractNativeEventFilter>
class MouseControl {
public:
    static void enableMousePassThrough(WId windowId,bool enable);
};

class MyEventFilter : public QAbstractNativeEventFilter {
    ///// 自定义的本地事件过滤器类，用于处理鼠标事件
public:
    // 重写nativeEventFilter函数，用于处理本地事件
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
};