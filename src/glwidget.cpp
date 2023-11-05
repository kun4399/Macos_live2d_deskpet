#include <GL/glew.h> // glew must put first,and can not include QtOpenGL
#include <QtGui>
#include "LAppDelegate.hpp"
#include "glwidget.h"
#include "resource_loader.hpp"
#include <QApplication>
#include "QtOpenGLWidgets/QOpenGLWidget"

namespace {
    constexpr int frame = 40;
    constexpr int fps = 1000 / frame;
}

GLWidget::GLWidget(QWidget *parent)
        : QOpenGLWidget(parent) {
    this->startTimer(fps);
//    this->setCursor(QCursor(Qt::PointingHandCursor));//通过这个可以获取到鼠标在全屏幕的坐标，即使鼠标穿透也可以
//    this->x(); //获取的是窗口在其父控件（如果有的话）中的 x 坐标
}

GLWidget::~GLWidget() {
}

void GLWidget::initializeGL() {
    LAppDelegate::GetInstance()->Initialize(this);
    LAppDelegate::GetInstance()->resize(this->width(), this->height());
}

void GLWidget::paintGL() {
//    glClearColor(0,0,0,0.0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClearDepthf(1.0);
    //qDebug("GLWidget::paintGL()");

    LAppDelegate::GetInstance()->update();
}

void GLWidget::resizeGL(int width, int height) {
#if 0
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double halfW = width / 2.0;
    double halfH = height / 2.0;
    glOrtho(-halfW, halfW, -halfH, +halfH, 4.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
#endif
    //qDebug("width:%d,height:%d",width,height);
    LAppDelegate::GetInstance()->resize(width, height);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {

    if (resource_loader::get_instance().moveable()) {
        QApplication::sendEvent(this->parent(), event);
    } else {
        int x = event->position().x();
        int y = event->position().y();
        //qDebug("x:%d y:%d",x,y);
        LAppDelegate::GetInstance()->mousePressEvent(x, y);
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (resource_loader::get_instance().moveable()) {
        QApplication::sendEvent(this->parent(), event);
    } else {
        int x = event->position().x();
        int y = event->position().y();
        LAppDelegate::GetInstance()->mouseReleaseEvent(x, y);
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (resource_loader::get_instance().moveable()) {
        QApplication::sendEvent(this->parent(), event);
    } else {
        int x = event->position().x();
        int y = event->position().y();
        //qDebug("x:%d y:%d",x,y);
        LAppDelegate::GetInstance()->mouseMoveEvent(x, y);
    }
}

void GLWidget::timerEvent(QTimerEvent *) {
//    this->updateGL();
//    int cursorX=QCursor::pos().x();
//    int tx=this->x();
//    std::cout << "cursorX:" << cursorX << "x:" << tx << std::endl;
    this->update();
}

void GLWidget::closeEvent(QCloseEvent *e) {
    QApplication::sendEvent(this->parent(), e);
}
