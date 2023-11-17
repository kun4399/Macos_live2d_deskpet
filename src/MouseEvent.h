#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H
#include "GL/glew.h"
#import <QtGui/QWindow>
#include <ApplicationServices/ApplicationServices.h>

class MouseEventHandle : public QObject {

//    Q_OBJECT

public:
    explicit MouseEventHandle(QObject *parent = nullptr);

    static void EnableMousePassThrough(WId windowId, bool enable);

    void startMonitoring();

    bool stopMonitoring();

private:
    CFMachPortRef eventTap;
    CFRunLoopRef ref;
    bool is_monitoring{false};

    // 静态回调函数（实际上可以直接用handleMouseEvent函数作为回调函数，毕竟可以声明为静态，但是为了以后的扩展性，还是分开写了）
    static CGEventRef mouseEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon);

    CGEventRef handleMouseEvent(CGEventType type, CGEventRef event);
};
#endif //MOUSEEVENT_H