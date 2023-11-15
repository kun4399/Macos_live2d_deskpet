#include "Log_util.h"
#include "resource_loader.hpp"
#include "event_handler.hpp"


#define  Resources_FILE_PATH  "/Users/gaoyukun/github/qf/Resources"
namespace {
    constexpr int config_file_size = 4096;
}

bool resource_loader::initialize() {
    if (is_init) {
        CF_LOG_ERROR("initialize has finished");
        return true;
    }
    resource_file_path = QString(Resources_FILE_PATH);
//    QStringList path_list = QCoreApplication::applicationDirPath().split("/");
//    path_list.removeLast();
//    resource_file_path = path_list.join("/").append("/Resources");
    QFile file(resource_file_path + "/config.json");

    if (!file.open(QIODevice::ReadOnly)) {
        CF_LOG_ERROR("open config.json failed, path:%s", (resource_file_path + "/config.json").toStdString().c_str());
        return false;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonParseError json_error;
    QJsonDocument json_doc(QJsonDocument::fromJson(data, &json_error));
    if (json_error.error != QJsonParseError::NoError) {
        CF_LOG_ERROR("parse json failed, error:%s", json_error.errorString().toStdString().c_str());
        return false;
    }
    root_ = json_doc.object();
    QJsonValue system_tray = root_.value("systemtray");
    if (system_tray.isString()) {
        system_tray_icon_path = system_tray.toString();
    } else {
        system_tray_icon_path = "/Qf.PNG";
        CF_LOG_ERROR("system tray is not defined, use default icon");
    }

    QJsonValue module = root_.value("module");
    if (module.isArray()) {
        // 加载模型
        QJsonArray module_array = std::move(module.toArray());
        for (auto &&i: module_array) {
            QJsonValue model_value = i;
            if (model_value.isObject()) {
                QJsonObject model_object = model_value.toObject();
                QJsonValue name_value = model_object.value("name");
                QJsonValue width = model_object.value("width");
                QJsonValue height = model_object.value("height");
                if (name_value.isString() && width.isDouble() && height.isDouble()) {
                    resource_loader::model tmp_model{};
                    tmp_model.name = name_value.toString();
                    tmp_model.model_width = width.toInt();
                    tmp_model.model_height = height.toInt();
                    model_list.push_back(tmp_model);
                } else {
                    CF_LOG_ERROR("model format error: name:%s, width:%s, height:%s",
                                 name_value.toString().toStdString().c_str(),
                                 width.toString().toStdString().c_str(),
                                 height.toString().toStdString().c_str());
                    return false;
                }
            } else {
                CF_LOG_ERROR("model format error: model json format error");
                return false;
            }
        }
    } else {
        CF_LOG_ERROR("module format error: module json is not array");
        return false;
    }

    if (model_list.empty()) {
        CF_LOG_ERROR("module format error: module array is empty");
        return false;
    }

    current_model_index = 0;
    QJsonValue userdata = root_.value("userdata");
    if (userdata.isObject()) { // 检查是否是json对象
        QJsonObject userdata_object = userdata.toObject();
        QJsonValue current_model_value = userdata_object.value("current_model");
        if (current_model_value.isString()) {
            update_current_model(current_model_value.toString());
        }

        QJsonValue top_value = userdata_object.value("top");
        if (top_value.isBool()) {
            top_ = top_value.toBool();
        } else {
            top_ = false;
        }

        QJsonValue window_x_value = userdata_object.value("window_x");
        if (window_x_value.isDouble()) {
            current_model_x = window_x_value.toInt();
        }

        QJsonValue window_y_value = userdata_object.value("window_y");
        if (window_y_value.isDouble()) {
            current_model_y = window_y_value.toInt();
        }

        QJsonValue dialog_x_value = userdata_object.value("dialog_x");
        if (dialog_x_value.isDouble()) {
            dialog_x = dialog_x_value.toInt();
        }

        QJsonValue dialog_y_value = userdata_object.value("dialog_y");
        if (dialog_y_value.isDouble()) {
            dialog_y = dialog_y_value.toInt();
        }

        QJsonValue dialog_width_value = userdata_object.value("dialog_width");
        if (dialog_width_value.isDouble()) {
            dialog_width = dialog_width_value.toInt();
        }
        QJsonValue dialog_height_value = userdata_object.value("dialog_height");
        if (dialog_height_value.isDouble()) {
            dialog_height = dialog_height_value.toInt();
        }
    } else {
        CF_LOG_ERROR("userdata format error: userdata is not json object");
        return false;
    }
    QJsonValue gpt_api = root_.value("azure_api");
    if (gpt_api.isObject()) {
        QJsonObject gpt_api_object = gpt_api.toObject();
        QJsonValue gpt_api_url_value = gpt_api_object.value("url");
        if (gpt_api_url_value.isString()) {
            gpt_api_url = std::move(gpt_api_url_value.toString());
        } else {
            CF_LOG_INFO("azure_api_url is not defined");
        }
        QJsonValue gpt_api_key_value = gpt_api_object.value("key");
        if (gpt_api_key_value.isString()) {
            gpt_api_key = std::move(gpt_api_key_value.toString());
        } else {
            CF_LOG_INFO("azure_api_key is not defined");
        }
        QJsonValue gpt_system_prompt_value = gpt_api_object.value("system_prompt");
        if (gpt_system_prompt_value.isString()) {
            gpt_system_prompt = std::move(gpt_system_prompt_value.toString());
        } else {
            gpt_system_prompt="你的回复要包括三个参数：expression（无可选值），motion（可选 Idle），message,需要json格式回复,无有特殊符号。简洁的回答";
            CF_LOG_INFO("system_prompt is not defined");
        }
        if (!gpt_api_url.isEmpty() && !gpt_api_key.isEmpty()) {
            gpt_enable_ = true;
        }
    } else {
        CF_LOG_INFO("azure_api is not defined");
    }
    is_init = true;
    return true;

}


bool resource_loader::update_current_model_size(int x, int y) {
    model_list[current_model_index].model_width = x;
    model_list[current_model_index].model_height = y;
    config_change = true;
    return true;
}

bool resource_loader::update_current_model_position(int x, int y) {
    current_model_x = x;
    current_model_y = y;
    config_change = true;
    return true;
}

resource_loader::~resource_loader() {
    release();
}

bool resource_loader::update_dialog_position(int x, int y) {
    dialog_x = x;
    dialog_y = y;
    config_change = true;
    return true;
}

bool resource_loader::update_dialog_size(int width, int height) {
    dialog_width = width;
    dialog_height = height;
    config_change = true;
    return true;
}

bool resource_loader::save_config() {
    if (!is_init || !config_change) { return true; }
    config_change = false;
    CF_LOG_DEBUG("start to save config");
    auto *root = new QJsonObject();
    root->insert("systemtray", system_tray_icon_path);
    QJsonArray module_array;
    for (const auto &item: model_list) {
        QJsonObject model_object;
        model_object.insert("name", item.name);
        model_object.insert("width", item.model_width);
        model_object.insert("height", item.model_height);
        module_array.append(model_object);
    }
    root->insert("module", module_array);
    QJsonObject userdata_object;
    userdata_object.insert("current_model", model_list[current_model_index].name);
    userdata_object.insert("top", top_);
    userdata_object.insert("window_x", current_model_x);
    userdata_object.insert("window_y", current_model_y);
    userdata_object.insert("dialog_x", dialog_x);
    userdata_object.insert("dialog_y", dialog_y);
    userdata_object.insert("dialog_width", dialog_width);
    userdata_object.insert("dialog_height", dialog_height);
    root->insert("userdata", userdata_object);
    QJsonObject azure_api_object;
    azure_api_object.insert("url", gpt_api_url);
    azure_api_object.insert("key", gpt_api_key);
    azure_api_object.insert("system_prompt", gpt_system_prompt);
    root->insert("azure_api", azure_api_object);
    auto result = event_handler::get_instance().report<QJsonObject>(msg_queue::message_type::app_config_save, root);
    if (result != msg_queue::status::success) {
        CF_LOG_ERROR("save config failed, error:%d", result);
        return false;
    }
    return true;
}

void resource_loader::release() {
    if (!is_init) {
        return;
    }
    if (config_change) {
        save_config();
    }
    is_init = false;
    event_handler::get_instance().release();
}

const QVector<resource_loader::model> &resource_loader::get_model_list() {
    return model_list;
}

QString resource_loader::get_system_tray_icon_path() {
    return resource_file_path + "/" + system_tray_icon_path;
}

const resource_loader::model *resource_loader::get_current_model() {
    return &model_list[current_model_index];
}

int resource_loader::get_current_model_index() const {
    return current_model_index;
}

bool resource_loader::update_current_model(QString name) {
    for (uint32_t i = 0; i < model_list.size(); i++) {
        if (name == model_list[i].name) {
            current_model_index = i;
            config_change = true;
            return true;
        }
    }
    return false;
}

bool resource_loader::update_current_model(uint32_t index) {
    if (index < model_list.size()) {
        current_model_index = (int) index;
        config_change = true;
        return true;
    }
    return false;

}

bool resource_loader::is_top() const {
    return top_;
}

void resource_loader::set_top(bool top) {
    if (this->top_ != top) {
        top_ = top;
        config_change = true;
    }
}

QString resource_loader::get_config_path() const {
    return resource_file_path + "/config.json";
}

const QString &resource_loader::get_gpt_url() const {
    return gpt_api_url;
}

const QString &resource_loader::get_gpt_key() const {
    return gpt_api_key;
}

const QString &resource_loader::get_gpt_system_prompt() const {
    return gpt_system_prompt;
}