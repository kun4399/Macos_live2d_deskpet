#include "mainwindow.h"
#include <QMenu>
#include "./ui_mainwindow.h"
#include "qf_log.h"
#include "LAppLive2DManager.hpp"
#include "resource_loader.hpp"
#include "event_handler.hpp"
#include <QMessageBox>
#include <QMouseEvent>
#include <QtGui/QGuiApplication>
#include "qaction.h"
#include "qactiongroup.h"
#include <QMenuBar>

namespace {
    int pos_x;
    int pos_y;
}

MainWindow::MainWindow(QWidget *parent, QApplication *mapp)
        : QMainWindow(parent), ui(new Ui::MainWindow), m_systemTray(new QSystemTrayIcon(this)), app(mapp),
          dialog_window_(new ChatDialog(this)) {
    assert(app != nullptr);
    int cxScreen, cyScreen;
    cxScreen = QApplication::primaryScreen()->availableGeometry().width();
    cyScreen = QApplication::primaryScreen()->availableGeometry().height();
    this->mouse_press = false;
    this->setAttribute(Qt::WA_TranslucentBackground);
//  this->setWindowFlag(Qt::WindowStaysOnTopHint);//一旦启用这个鼠标穿透就没有效果了
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::NoDropShadowWindowHint); //去掉窗口阴影，这个bug查了好久好久！！！！
//  this->setAttribute(Qt::WA_TransparentForMouseEvents, true);//鼠标穿透
    auto model = resource_loader::get_instance().get_current_model();
    if (model->x == -1 && model->y == -1) {
        this->move(cxScreen - this->width() * 0.4, cyScreen - this->height() * 0.9);
    } else {
        this->move(model->x, model->y);
    }

    if (resource_loader::get_instance().is_top()) {
        this->setWindowFlag(Qt::WindowStaysOnTopHint);
    }
//------------------------------------------------------------------
    m_menu = new QMenu(this);
    m_move = new QMenu(this);

    auto menuBar = new QMenuBar(this);
//    menuBar->setNativeMenuBar(false);
    this->setMenuBar(menuBar);
    menuBar->addMenu(m_menu);

    m_menu->setTitle(QStringLiteral("菜单"));
    m_move->setTitle(QStringLiteral("移动"));
    g_move = new QActionGroup(m_move);
    move_on = new QAction(QStringLiteral("开"), g_move);
    move_off = new QAction(QStringLiteral("关"), g_move);
    move_on->setCheckable(true);
    move_off->setCheckable(true);
    if (resource_loader::get_instance().moveable()) {
        move_on->setChecked(true);
    } else {
        move_off->setChecked(true);
    }

    g_move->setExclusive(true);
    m_move->addAction(move_on);
    m_move->addAction(move_off);

    m_change = new QMenu(this);
    m_change->setTitle(QStringLiteral("切换"));
    g_change = new QActionGroup(m_change);
    auto ms = resource_loader::get_instance().get_model_list();
    auto cm = resource_loader::get_instance().get_current_model();
    bool find = true;
    for (uint32_t i = 0; i < ms.size(); i++) {
        QAction *tmp_model = new QAction(QString(ms[i].name), g_change);
        tmp_model->setCheckable(true);
        if (find && strncmp(cm->name, ms[i].name, resource_name_size) == 0) {
            tmp_model->setChecked(true);
            find = false;
        }
        //m_change->addAction(tmp_model);
        model_list.push_back(tmp_model);
    }
    g_change->setExclusive(true);
    m_change->addActions(model_list);

    a_exit = new QAction(m_menu);
    a_exit->setText(QStringLiteral("退出"));

    set_top = new QAction(m_menu);
    set_top->setText(QStringLiteral("置顶"));
    set_top->setCheckable(true);
    if (resource_loader::get_instance().is_top()) {
        set_top->setChecked(true);
    } else {
        set_top->setChecked(false);
    }

    m_menu->addAction(set_top);
    m_menu->addMenu(m_change);
    m_menu->addMenu(m_move);
    m_menu->addSeparator();
    m_menu->addAction(a_exit);
//显示系统托盘
    m_systemTray->setIcon(QIcon(resource_loader::get_instance().get_system_tray_icon_path()));
    m_systemTray->setToolTip("Qf");
    m_systemTray->setContextMenu(m_menu);
    m_systemTray->show();
    connect(m_systemTray, &QSystemTrayIcon::activated, this, &MainWindow::activeTray);
    connect(set_top, &QAction::triggered, this, &MainWindow::action_set_top);
    connect(a_exit, &QAction::triggered, this, &MainWindow::action_exit);
    connect(g_move, &QActionGroup::triggered, this, &MainWindow::action_move);
    connect(g_change, &QActionGroup::triggered, this, &MainWindow::action_change);
    event_handler::get_instance().register_main_window(this);
    ui->setupUi(this);
}

void MainWindow::activeTray(QSystemTrayIcon::ActivationReason r) {
    switch (r) {
        case QSystemTrayIcon::Context:
            //m_menu->showNormal();
            break;
        default:
            break;
    }
}

void MainWindow::action_exit() {
    QF_LOG_INFO("app exit");
    resource_loader::get_instance().release();
    event_handler::get_instance().release();
    app->exit(0);
}

void MainWindow::action_move(QAction *a) {
    if (a == this->move_on) {
        QF_LOG_DEBUG("move on");
        resource_loader::get_instance().set_moveable(true);
    } else if (a == this->move_off) {
        QF_LOG_DEBUG("move off");
        resource_loader::get_instance().set_moveable(false);
    }
}

void MainWindow::action_change(QAction *a) {
    int counter = 0;
    for (auto &i: model_list) {
        if (i == a) {
            break;
        }
        counter++;
    }

    if (resource_loader::get_instance().update_current_model(counter)) {
        bool load_fail = true;
        auto m = resource_loader::get_instance().get_current_model();
        if (LAppLive2DManager::GetInstance()->ChangeScene((Csm::csmChar *) m->name)) {
            this->move(m->x, m->y);
            load_fail = false;
        } else {
            int _counter = 0;
            for (auto &item: resource_loader::get_instance().get_model_list()) {
                if (_counter != counter) {
                    if (LAppLive2DManager::GetInstance()->ChangeScene((Csm::csmChar *) item.name)) {
                        this->move(m->x, m->y);
                        load_fail = false;
                        QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(2);
                        this->m_systemTray->showMessage(QStringLiteral("waring"),
                                                        tr("load model fail,try load default model"), msgIcon, 5000);
                        this->model_list[_counter]->setChecked(true);
                        resource_loader::get_instance().update_current_model(_counter);
                        break;
                    }
                }
                _counter++;
            }
        }

        if (load_fail) {
            this->hide();
            this->resize(640, 480);
            int cxScreen, cyScreen;
            cxScreen = QApplication::primaryScreen()->availableGeometry().width();
            cyScreen = QApplication::primaryScreen()->availableGeometry().height();
            this->move(cxScreen / 2 - 320, cyScreen / 2 - 240);
            this->show();
            QMessageBox::critical(this, tr("QF"), QStringLiteral("资源文件错误,程序终止"));
            QF_LOG_INFO("app exit");
            resource_loader::get_instance().release();
            event_handler::get_instance().release();
            this->app->exit(0);
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *) {
    QF_LOG_INFO("app exit");
    resource_loader::get_instance().release();
    event_handler::get_instance().release();
    this->app->exit(0);
}

void MainWindow::customEvent(QEvent *e) {
    QfQevent *event = (QfQevent *) e;
    switch (event->e) {
        case QfQevent::event_type::no_modle:
            QMessageBox::critical(this, tr("QF"), tr(event->why));
            QF_LOG_INFO("app exit");
            resource_loader::get_instance().release();
            event_handler::get_instance().release();
            this->app->exit(0);
            break;
        case QfQevent::event_type::load_default_model: {
            int index = resource_loader::get_instance().get_current_model_index();
            auto model_list = resource_loader::get_instance().get_model_list();
            bool load_fail = true;
            for (int i = 0; i < model_list.size(); i++) {
                if (i != index) {
                    if (LAppLive2DManager::GetInstance()->ChangeScene((Csm::csmChar *) model_list[i].name)) {
                        this->move(model_list[i].x, model_list[i].y);
                        load_fail = false;
                        QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(2);
                        this->m_systemTray->showMessage(QStringLiteral("waring"), tr(event->why), msgIcon, 2000);
                        this->model_list[i]->setChecked(true);
                        resource_loader::get_instance().update_current_model(i);
                        break;
                    }
                }
            }

            if (load_fail) {
                this->hide();
                this->resize(640, 480);
                int cxScreen, cyScreen;
//            cxScreen=GetSystemMetrics(SM_CXSCREEN);
//            cyScreen=GetSystemMetrics(SM_CYSCREEN);

                this->move(cxScreen / 2 - 320, cyScreen / 2 - 240);
                this->show();
                QMessageBox::critical(this, tr("QF"), QStringLiteral("资源文件错误,程序终止"));
                QF_LOG_INFO("app exit");
                resource_loader::get_instance().release();
                event_handler::get_instance().release();
                this->app->exit(0);
            }
            break;
        }
        default:
            break;
    }
}

void MainWindow::action_set_top() {
    if (set_top->isChecked() != resource_loader::get_instance().is_top()) {
        if (set_top->isChecked()) {
            this->setWindowFlag(Qt::WindowStaysOnTopHint, true);
        } else {
            this->setWindowFlag(Qt::WindowStaysOnTopHint, false);
        }
        this->show();
        resource_loader::get_instance().set_top(set_top->isChecked());
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
//    pos_x = event->globalX();
    pos_x = event->globalPosition().x();
    pos_y = event->globalPosition().y();
    QF_LOG_INFO("x:%d,y:%d", pos_x, pos_x);
    this->mouse_press = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *) {
    this->mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
    resource_loader::get_instance().update_current_model_position(this->x(), this->y());
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (this->mouse_press) {
        int x = event->globalPosition().x();
        int y = event->globalPosition().y();
        QF_LOG_INFO("1x:%d,y:%d", x, y);
        QF_LOG_INFO("2x:%d,y:%d", this->x(), this->y());
        this->move(this->x() + x - pos_x, this->y() + y - pos_y);
        pos_x = x;
        pos_y = y;
        this->setCursor(Qt::SizeAllCursor);
    }
}
