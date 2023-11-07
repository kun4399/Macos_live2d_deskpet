#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GL/glew.h>
#include "QtOpenGLWidgets/QOpenGLWidget"
#include <QTimer>
#include "ChatDialog.h"
// 这个窗口是ui中的“提升为”操作指定一个widget成为这个类的
class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);

    ~GLWidget() override;

protected:
    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int width, int height) override;
    // 一旦开启鼠标穿透，这些鼠标事件就无法捕获了
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void timerEvent(QTimerEvent *event) override;

    void closeEvent(QCloseEvent *e) override;

    void enterEvent(QEnterEvent *e) override;

    void leaveEvent(QEvent *e) override;
};

#endif
