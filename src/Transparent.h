#import <Cocoa/Cocoa.h>
#import <QtGui/QWindow>

class MouseControl {
public:
    static void enableMousePassThrough(WId windowId,bool enable);
};
