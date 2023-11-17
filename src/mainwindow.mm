#include "mainwindow.h"
#include <QMenu>
#include "./ui_mainwindow.h"
#include "Log_util.h"
#include "LAppLive2DManager.hpp"
#include "resource_loader.hpp"
#include "event_handler.hpp"
#include <QMessageBox>
#include <QMouseEvent>
#include <QtGui/QGuiApplication>
#include "qaction.h"
#include "qactiongroup.h"
#include <QMenuBar>
#import "MouseEvent.h" // 这个文件头不能放到mainwindow.h中，否则会报错

namespace {
    int pos_x;
    int pos_y;
}

MainWindow::MainWindow(QWidget *parent, QApplication *mapp)
        : QMainWindow(parent), ui(new Ui::MainWindow), m_systemTray(new QSystemTrayIcon(this)), app(mapp),
          dialog_window_(new ChatDialog(this)) {
    assert(app != nullptr);
    ui->setupUi(this);
    int cxScreen, cyScreen; // 获取屏幕的分辨率
    cxScreen = QApplication::primaryScreen()->availableGeometry().width();
    cyScreen = QApplication::primaryScreen()->availableGeometry().height();
    resource_loader::get_instance().screen_width = cxScreen;
    resource_loader::get_instance().screen_height = cyScreen;
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::NoDropShadowWindowHint); //去掉窗口阴影，这个bug查了好久好久！！！！
    this->move(resource_loader::get_instance().current_model_x, resource_loader::get_instance().current_model_y);
    auto model = resource_loader::get_instance().get_current_model();
    this->resize(model->model_width, model->model_height);
    if (resource_loader::get_instance().is_top()) {
        this->setWindowFlag(Qt::WindowStaysOnTopHint);
    }
//------------------------------------------------------------------
    m_menu = new QMenu(this);
    m_move = new QMenu(this);
    m_dialog = new QMenu(this);
    auto menuBar = new QMenuBar(this);
//    menuBar->setNativeMenuBar(false);
    this->setMenuBar(menuBar);
    menuBar->addMenu(m_menu);

    m_menu->setTitle(QStringLiteral("菜单"));
    m_move->setTitle(QStringLiteral("位置和大小调节"));
    m_dialog->setTitle(QStringLiteral("对话框"));
    g_move = new QActionGroup(m_move);
    move_on = new QAction(QStringLiteral("开"), g_move);
    move_off = new QAction(QStringLiteral("关"), g_move);
    g_dialog = new QActionGroup(m_dialog);
    open_dialog = new QAction(QStringLiteral("打开"), g_dialog);
    close_dialog = new QAction(QStringLiteral("关闭"), g_dialog);
    move_on->setCheckable(true);
    move_off->setCheckable(true);
    open_dialog->setCheckable(true);
    close_dialog->setCheckable(true);
    close_dialog->setChecked(true);
    move_off->setChecked(true);
    g_move->setExclusive(true);
    m_move->addAction(move_on);
    m_move->addAction(move_off);

    g_dialog->setExclusive(true);
    m_dialog->addAction(open_dialog);
    m_dialog->addAction(close_dialog);

    m_change = new QMenu(this);
    m_change->setTitle(QStringLiteral("切换"));
    g_change = new QActionGroup(m_change);
    auto ms = resource_loader::get_instance().get_model_list();
    auto cm = resource_loader::get_instance().get_current_model();
    bool find = true;
    for (auto &m: ms) {
        auto *tmp_model = new QAction(m.name, g_change);
        tmp_model->setCheckable(true);
        model_list.push_back(tmp_model);
    }
    g_change->setExclusive(true);
    m_change->addActions(model_list);

    a_exit = new QAction(m_menu);
    a_exit->setText(QStringLiteral("退出"));
    a_voice = new QAction(m_menu);
    a_voice->setText(QStringLiteral("语音"));
    a_voice->setCheckable(true);
    a_voice->setChecked(false);

    set_top = new QAction(m_menu);
    set_top->setText(QStringLiteral("置顶"));
    set_top->setCheckable(true);
    if (resource_loader::get_instance().is_top()) {
        set_top->setChecked(true);
    } else {
        set_top->setChecked(false);
    }

    m_menu->addAction(set_top);
    m_menu->addAction(a_voice);
    m_menu->addMenu(m_change);
    m_menu->addMenu(m_move);
    m_menu->addMenu(m_dialog);
    m_menu->addSeparator();
    m_menu->addAction(a_exit);
//显示系统托盘
    m_systemTray->setIcon(QIcon(resource_loader::get_instance().get_system_tray_icon_path()));
    m_systemTray->setToolTip("ChatFriend");
    m_systemTray->setContextMenu(m_menu);
    m_systemTray->show();
    connect(m_systemTray, &QSystemTrayIcon::activated, this, &MainWindow::activeTray);
    connect(set_top, &QAction::triggered, this, &MainWindow::action_set_top);
    connect(a_voice, &QAction::triggered, this, &MainWindow::action_voice);
    connect(a_exit, &QAction::triggered, this, &MainWindow::action_exit);
    connect(g_move, &QActionGroup::triggered, this, &MainWindow::action_move);
    connect(g_change, &QActionGroup::triggered, this, &MainWindow::action_change);
    connect(g_dialog, &QActionGroup::triggered, this, &MainWindow::action_dialog);
    mouse_event_ = new MouseEventHandle();
    event_handler::register_main_window(this);
    MouseEventHandle::EnableMousePassThrough(this->winId(), true);
        std::thread t(&MouseEventHandle::startMonitoring, mouse_event_);
        t.detach();
}

void MainWindow::activeTray(QSystemTrayIcon::ActivationReason r) {
    switch (r) {
        case QSystemTrayIcon::Context:
            m_menu->showNormal();
            break;
        default:
            break;
    }
}

void MainWindow::action_exit() {
    CF_LOG_INFO("main_window exit");
    resource_loader::get_instance().release();
    mouse_event_->stopMonitoring();
    QApplication::exit(0);
}

void MainWindow::action_move(QAction *a) {
    if (a == this->move_on) {
        CF_LOG_DEBUG("move on");
//        this->setWindowFlag(Qt::FramelessWindowHint, false);
        MouseEventHandle::EnableMousePassThrough(this->winId(), false);
        this->m_change->setEnabled(false);
        this->a_exit->setEnabled(false);
        this->m_dialog->setEnabled(false);
        if (dialog_window_->isVisible()) {
            dialog_window_->setWindowFlag(Qt::FramelessWindowHint, false);
            dialog_window_->show();
        }
    } else if (a == this->move_off) {
        CF_LOG_DEBUG("move off");
        this->setWindowFlag(Qt::FramelessWindowHint, true);
        MouseEventHandle::EnableMousePassThrough(this->winId(), true);
        this->m_change->setEnabled(true);
        this->a_exit->setEnabled(true);
        this->m_dialog->setEnabled(true);
        auto &model = resource_loader::get_instance();
        if (dialog_window_->isVisible()) {
            dialog_window_->setWindowFlag(Qt::FramelessWindowHint, false);
            dialog_window_->show();
            model.update_dialog_position(dialog_window_->x(), dialog_window_->y());
            model.update_dialog_size(dialog_window_->width(), dialog_window_->height());
        }
        model.update_current_model_position(this->x(), this->y());
        model.update_current_model_size(this->width(), this->height());
    }
    this->show();

}

void MainWindow::action_dialog(QAction *a) {
    if (a == this->open_dialog) {
        dialog_window_->show();
    } else if (a == this->close_dialog) {
        dialog_window_->hide();
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
        /// 将QString转化为char*
        QByteArray model_name = m->name.toUtf8();
        if (LAppLive2DManager::GetInstance()->ChangeScene((Csm::csmChar *) model_name.data())) {
            this->resize(m->model_width, m->model_height);
            load_fail = false;
        } else {
            int _counter = 0;
            for (auto &item: resource_loader::get_instance().get_model_list()) {
                if (_counter != counter) {
                    model_name = item.name.toUtf8();
                    if (LAppLive2DManager::GetInstance()->ChangeScene((Csm::csmChar *) model_name.data())) {
                        this->resize(item.model_width, item.model_height);
                        load_fail = false;
                        auto msgIcon = QSystemTrayIcon::MessageIcon(2);
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
            action_exit();
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *) {
    CF_LOG_INFO("app exit");
    action_exit();
}

void MainWindow::customEvent(QEvent *e) {
    auto *event = (QfQevent *) e;
    switch (event->e) {
        case QfQevent::event_type::no_modle:
            QMessageBox::critical(this, tr("QF"), tr(event->why));
            CF_LOG_ERROR("no model");
            action_exit();
            break;
        case QfQevent::event_type::load_default_model: {
            int index = resource_loader::get_instance().get_current_model_index();
            auto model_List = resource_loader::get_instance().get_model_list();
            bool load_fail = true;
            for (int i = 0; i < model_List.size(); i++) {
                if (i != index) {
                    QByteArray model_name = model_List[i].name.toUtf8();
                    if (LAppLive2DManager::GetInstance()->ChangeScene((Csm::csmChar *) model_name.data())) {
                        this->move(model_List[i].model_width, model_List[i].model_height);
                        load_fail = false;
                        auto msgIcon = QSystemTrayIcon::MessageIcon(2);
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
                cxScreen = QApplication::primaryScreen()->availableGeometry().width();
                cyScreen = QApplication::primaryScreen()->availableGeometry().height();
                this->move(cxScreen / 2 - 320, cyScreen / 2 - 240);
                this->show();
                QMessageBox::critical(this, tr("QF"), QStringLiteral("资源文件错误,程序终止"));
                action_exit();
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
            dialog_window_->setWindowFlag(Qt::WindowStaysOnTopHint, true);
        } else {
            this->setWindowFlag(Qt::WindowStaysOnTopHint, false);
            dialog_window_->setWindowFlag(Qt::WindowStaysOnTopHint, false);
        }
        this->show();
        MouseEventHandle::EnableMousePassThrough(this->winId(), true);
        if (dialog_window_->isVisible()) {
            dialog_window_->show();
        }
        resource_loader::get_instance().set_top(set_top->isChecked());
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    pos_x = event->globalPosition().x();
    pos_y = event->globalPosition().y();
//    QF_LOG_INFO("x:%d,y:%d", pos_x, pos_x);
    this->mouse_press = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *) {
    this->mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
    resource_loader::get_instance().update_current_model_position(this->x(), this->y());
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    if (this->mouse_press) {
        int x = (int) event->globalPosition().x();
        int y = (int) event->globalPosition().y();
        this->move(this->x() + x - pos_x, this->y() + y - pos_y);
        pos_x = x;
        pos_y = y;
        this->setCursor(Qt::SizeAllCursor);
    }
}

void MainWindow::action_voice() {
    if (a_voice->isChecked()) {
        resource_loader::get_instance().tts_enable_ = true;
    } else {
        resource_loader::get_instance().tts_enable_ = false;
    }
}
