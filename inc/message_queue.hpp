#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <mutex>
#include "queue"
#include "Log_util.h"

class msg_queue {
public:
    enum class status {
        error,
        success,
        fail
    };
    enum class message_type {
        none,
        app_exit,
        app_all_model_load_fail,
        app_current_model_load_fail,
        app_config_save,
        play_tts_audio,
        play_local_audio,
    };

    struct message {
        message() : type_(message_type::none), data_(nullptr) {};

        message(message_type type, void *data_) : type_(type), data_(data_) {};
        message_type type_;
        void *data_;
    };

    template<typename T>
    struct msg_guard {
        msg_guard() = default;

        msg_guard(message_type type, T *data) : type_(type), data_(data) {};

        msg_guard(message msg) : type_(msg.type_), data_(reinterpret_cast<T *>(msg.data_)) {};

        msg_guard(msg_guard &&msg) noexcept: type_(msg.type_), data_(msg.data_) {
            msg.data_ = nullptr;
        };
        // 防止数据被提前释放，尽量用移动语句，避免因为临时变量导致的数据被提前释放。
        msg_guard &operator=(msg_guard &&msg) noexcept {
            if (this != &msg) {
                type_ = msg.type_;
                data_ = msg.data_;
                msg.data_ = nullptr;
            }
            return *this;
        };

        ~msg_guard() {
            if (data_ != nullptr) {
                CF_LOG_DEBUG("msg guard delete data");
                delete data_;
            }
        }

        message_type type_;
        T *data_;
    };

    explicit msg_queue(int max_count);

    ~msg_queue() = default;

    msg_queue::message_type getType();

    template<class T>
    msg_queue::status post(message_type type, T *ptr);

    template<class T>
    status get(msg_queue::msg_guard<T> &msg);

private:
    int max_count_{0};
    std::queue<message> msg_queue_;
    std::mutex mut_;
    std::condition_variable cond_;
};

template<class T>
msg_queue::status msg_queue::get(msg_queue::msg_guard<T> &msg) {
    std::unique_lock<std::mutex> lock(mut_);
    if (msg_queue_.empty()) {
        cond_.wait(lock);
    }
    if (msg_queue_.empty()) {
        return status::fail;
    }
    msg = msg_queue_.front();
    msg_queue_.pop();
    return status::success;
};

template<class T>
msg_queue::status msg_queue::post(msg_queue::message_type type, T *ptr) {
    std::unique_lock<std::mutex> lock(mut_);
    if (msg_queue_.size() >= max_count_) {
        return status::fail;
    }
    msg_queue_.emplace(type, reinterpret_cast<void *>(ptr));
    cond_.notify_one();
    return status::success;
};


#endif // !MESSAGE_QUEUE_H

