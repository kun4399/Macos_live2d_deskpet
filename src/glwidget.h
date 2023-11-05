#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GL/glew.h>
#include "QtOpenGLWidgets/QOpenGLWidget"
#include <QTimer>
// 这个窗口是通过编辑ui添加到mainWindow下面的
class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
    QTimer* ter;
public:
    explicit GLWidget(QWidget *parent = nullptr);

    ~GLWidget() override;

protected:
    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void timerEvent(QTimerEvent *event) override;

    void closeEvent(QCloseEvent *e) override;
};

#endif
