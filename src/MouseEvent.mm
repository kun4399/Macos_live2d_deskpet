//#include <GL/glew.h> // 这个必须要放在这里的第一行，否则会报错
#import "MouseEvent.h"
#include "LAppDelegate.hpp"
#include "Log_util.h"

void MouseEventControl::EnableMousePassThrough(WId windowId, bool enable) {
    auto qtNSView = reinterpret_cast<NSView *> (windowId);
    auto window = [qtNSView window];
    if (enable) {
        [window setIgnoresMouseEvents:YES];
    } else {
        [window setIgnoresMouseEvents:NO];
    }
}

bool MouseEventControl::startMonitoring() {
    // 创建事件源
    ref = CFRunLoopGetCurrent();
    // 设置事件掩码，这里监听左键按下、左键释放、左键拖拽事件
    CGEventMask eventMask = CGEventMaskBit( kCGEventLeftMouseDragged) | CGEventMaskBit(kCGEventLeftMouseDown) |
                            CGEventMaskBit(kCGEventLeftMouseUp);
    // 创建事件监听器
    eventTap = CGEventTapCreate(
            kCGSessionEventTap, kCGHeadInsertEventTap,kCGEventTapOptionListenOnly,
            eventMask, mouseEventCallback, this);
    if (!eventTap) {
        CF_LOG_ERROR("failed to create event tap");
        return false;
    }
    CF_LOG_DEBUG("start monitoring");
    is_monitoring = true;
    // 将事件监听器添加到当前线程的runloop中
    CFRunLoopSourceRef source =
            CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(ref, source, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);
    CFRunLoopRun();
    // 释放资源
    CGEventTapEnable(eventTap, false);
    CFRunLoopRemoveSource(ref, source, kCFRunLoopCommonModes);
    CFRelease(source);
    CFRelease(eventTap);
    CF_LOG_DEBUG("moues event thread exit, end monitoring");
    return true;
}

CGEventRef
MouseEventControl::mouseEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    auto *observer = static_cast<MouseEventControl *>(refcon);
    return observer->handleMouseEvent(type, event);
}

CGEventRef MouseEventControl::handleMouseEvent(CGEventType type, CGEventRef event) {
    CF_LOG_DEBUG("mouse event type: %d", type);
    NSPoint mouseLocation = [NSEvent mouseLocation];
    auto &resource = resource_loader::get_instance();
    int x = (int) mouseLocation.x - resource.current_model_x;
    int y = (resource.screen_height - (int) resource.current_model_y) - (int) mouseLocation.y;
    // 根据事件类型执行不同的操作
    switch (type) {
        case kCGEventLeftMouseDown:
            LAppDelegate::GetInstance()->mousePressEvent(x, y);
            CF_LOG_DEBUG("Left Mouse Down!");
            break;
        case kCGEventLeftMouseUp:
            LAppDelegate::GetInstance()->mouseReleaseEvent(x, y);
            CF_LOG_DEBUG("Left Mouse Up!");
            break;
        case kCGEventLeftMouseDragged:
            LAppDelegate::GetInstance()->mouseMoveEvent(x, y);
        default:
            break;
    }
    return event;
}

MouseEventControl::MouseEventControl(QObject *parent) : QObject(parent) {
}

bool MouseEventControl::stopMonitoring() {
    if (!is_monitoring) {
        return false;
    }
    is_monitoring = false;
    CFRunLoopStop(ref);
    return true;
}



