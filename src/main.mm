#include "mainwindow.h"
#include "event_handler.hpp"
#include "resource_loader.hpp"
#include <QApplication>
#include <AppKit/NSApplication.h>
#include <QLabel>
#include <QPixmap>
#import <Cocoa/Cocoa.h>
//#include <AppKit/NSWindow.h>
//考虑使用macOS的语音控制功能（voice control）
int main(int argc, char *argv[]) {
    if (!event_handler::get_instance().initialize()) {
        return 0;
    }

    if (!resource_loader::get_instance().initialize()) {
        return 0;
    }
    QApplication a(argc, argv);
    MainWindow w(nullptr, &a);
//    QLabel *background = new QLabel(&w);
//    QPixmap backgroundImage("/Users/gaoyukun/Downloads/picture.png");
//    background->setPixmap(backgroundImage);
//    background->setGeometry(0, 0, backgroundImage.width(), backgroundImage.height());

    w.show();
    return a.exec();
}
