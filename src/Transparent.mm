#import "Transparent.h"
void MouseControl::enableMousePassThrough(WId windowId, bool enable) {
    NSView* qtNSView = reinterpret_cast<NSView*> (windowId);
    auto window = [qtNSView window];
    if(enable){
        [window setIgnoresMouseEvents:YES];
    }
    else{
        [window setIgnoresMouseEvents:NO];
    }
}
