#include "message_queue.hpp"

namespace {
    constexpr int message_max_size = 32;
    constexpr int message_max_count = 64;
}


msg_queue::msg_queue(int max_count) : max_count_(max_count) {}


msg_queue::message_type msg_queue::getType() {
    std::unique_lock<std::mutex> lock(mut_);
    if (msg_queue_.empty()) {
        cond_.wait(lock);
    }
    if (msg_queue_.empty()) {
        return message_type::none;
    }
    return msg_queue_.front().type_;
}