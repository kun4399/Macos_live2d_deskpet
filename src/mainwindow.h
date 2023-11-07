#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <qaction.h>
#include "ChatDialog.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr,QApplication* mapp = nullptr);
    ~MainWindow();
    static MainWindow* get_instance();
protected:
    void closeEvent(QCloseEvent * e) override;
//    void mousePressEvent(QMouseEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;
//    void mouseMoveEvent(QMouseEvent *event) override;
    void customEvent(QEvent* e) override;
private:
    void activeTray(QSystemTrayIcon::ActivationReason r);
    void action_exit();
    void action_move(QAction*);
    void action_change(QAction*);
    void action_set_top();
    void action_dialog(QAction*);
    Ui::MainWindow *ui;
    bool mouse_press;
    QSystemTrayIcon* m_systemTray;
    QAction *a_exit;
    QAction *move_off;
    QAction *move_on;
    QAction *set_top;
    QAction * close_dialog;
    QAction * open_dialog;
    QMenu * m_dialog;
    QActionGroup *g_move;
    QActionGroup *g_dialog;
    QMenu *m_move;
    QList<QAction*> model_list;
    QActionGroup *g_change;
    QMenu *m_change;
    QMenu *m_menu;
    QApplication* app;
    ChatDialog* dialog_window_;
    static MainWindow* instance;
};
#endif // MAINWINDOW_H
