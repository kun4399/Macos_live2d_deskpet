#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include <QMainWindow>
#include <QEvent>
#include "message_queue.hpp"
#include <thread>
#include "resource_loader.hpp"
#include "qf_log.h"
#include <QApplication>

// 发送给窗口的消息
class QfQevent : public QEvent {
public:
    enum class event_type {
        none,
        no_modle,
        load_default_model,
    };

    QfQevent(const char *why, event_type e);

    const char *why;
    event_type e;
};

// 发送给处理器的类
class event_handler {

public:
    static event_handler &get_instance();

    template<typename T>
    msg_queue::status report(msg_queue::message_type e, T* data);

    void release();

    static void register_main_window(QMainWindow *mw);

    static void handle_task();

private:
    static msg_queue mq;
    std::thread handle_thread;

    event_handler();

    ~event_handler();
};

template<typename T>
msg_queue::status event_handler::report(msg_queue::message_type e, T* data) {
    return mq.post<T>(e, data);
}

#endif
