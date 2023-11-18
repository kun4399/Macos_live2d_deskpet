#include "mainwindow.h"
#include "event_handler.hpp"
#include "resource_loader.hpp"
#include <QApplication>
#import "MouseEvent.h"

/// TODO 官方框架自带的json解析器似乎有问题，时而崩溃？需要排查一下。还有动画播放卡顿（Idle结束的时候）

int main(int argc, char *argv[]) {
    event_handler::get_instance();
    QApplication a(argc, argv);
    if (!resource_loader::get_instance().initialize()) {
        return 0;
    }
    MainWindow w(nullptr, &a);
    w.show();
    return QApplication::exec();
    return a.exec();
}
