#include "mainwindow.h"
#include "event_handler.hpp"
#include "resource_loader.hpp"
#include <QApplication>
#include <AppKit/NSApplication.h>
#include <QLabel>
//#include <QPixmap>
//#import <Cocoa/Cocoa.h>
//#include <AppKit/NSWindow.h>
//考虑使用macOS的语音控制功能（voice control）
/// TODO 1. 添加动作表情控制
/// TODO 2. 添加聊天对话框，接入GPT https://kungptapi.openai.azure.com//openai/deployments/gpt35_turbo/chat/completions?api-version=2023-05-15
/// TODO 3. 优化模型管理，缩小内存的占用，似乎官方模型的内存占用很小
/// TODO 4. 接入语音api，可以爬 https://genshinvoice.top
/// TODO 5. 获取实时播放音频的音量，这个似乎很难实现

int main(int argc, char *argv[]) {
    if (!event_handler::get_instance().initialize()) {
        return 0;
    }

    if (!resource_loader::get_instance().initialize()) {
        return 0;
    }
    QApplication a(argc, argv);
    MainWindow w(nullptr, &a);
    w.show();
    return QApplication::exec();
}
