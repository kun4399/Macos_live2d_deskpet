#include "event_handler.hpp"

namespace {
    const QEvent::Type mtype = (QEvent::Type) QEvent::registerEventType();
}
msg_queue event_handler::mq(8);

QfQevent::QfQevent(const char *why, event_type e) : QEvent(mtype) {
    this->why = why;
    this->e = e;
}

event_handler &event_handler::get_instance() {
    static event_handler handler;
    return handler;
}

QMainWindow *event_handler::qmw = nullptr;

event_handler::event_handler() : handle_thread(&event_handler::handle_task) {
    CF_LOG_DEBUG("event handler init");
}

event_handler::~event_handler() {
}

void event_handler::register_main_window(QMainWindow *mw) {
    qmw = mw;
}

void event_handler::handle_task() {
    while (true) {
        auto type = mq.getType();
        CF_LOG_DEBUG("get message type: %d", type);
        if (type == msg_queue::message_type::none) {
            continue;
        } else if (type == msg_queue::message_type::app_exit) {
            msg_queue::msg_guard<QJsonObject> msg{};
            mq.get<QJsonObject>(msg);
            if ((msg.data_ != nullptr)) {
                QJsonDocument doc(*msg.data_);
                QFile file(resource_loader::get_instance().get_config_path());
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(doc.toJson());
                    file.close();
                    CF_LOG_INFO("config save success!");
                }
            }
            CF_LOG_INFO("handler thread exit");
            return;
        } else if (type == msg_queue::message_type::app_config_save) {
            msg_queue::msg_guard<QJsonObject> msg{};
            mq.get<QJsonObject>(msg);
            if ((msg.data_ != nullptr)) {
                /// 将json写入文件
                QJsonDocument doc(*msg.data_);
                QFile file(resource_loader::get_instance().get_config_path());
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(doc.toJson());
                    file.close();
                    CF_LOG_INFO("config save success!");
                }
            } else {
                CF_LOG_ERROR("app config save fail!");
            }
        } else if (type == msg_queue::message_type::app_all_model_load_fail) {
            CF_LOG_ERROR("app all model load fail");
            QApplication::postEvent(qmw, new QfQevent("app all model load fail", QfQevent::event_type::no_modle));
        } else if (type == msg_queue::message_type::app_current_model_load_fail) {
            CF_LOG_ERROR("app current model load fail");
            QApplication::postEvent(qmw, new QfQevent("app current model load fail",
                                                      QfQevent::event_type::load_default_model));
        }

    }
}

void event_handler::release() {
    mq.post<QJsonObject>(msg_queue::message_type::app_exit, nullptr);
    handle_thread.join();
    CF_LOG_DEBUG("event handler release success");
}


