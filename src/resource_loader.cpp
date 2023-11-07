﻿#include "cJSON.h"
#include "qf_log.h"
#include <cstdio>
#include <cstring>
#include "resource_loader.hpp"
#include "event_handler.hpp"

#define  CONFIG_JSON_FILE_PATH  "/Users/gaoyukun/github/qf/Resources/config.json"
namespace {
    constexpr int config_file_size = 4096;
}

bool resource_loader::initialize() {
    if (is_init) {
        QF_LOG_ERROR("initialize finished");
        return true;
    }

    FILE *fd = std::fopen(CONFIG_JSON_FILE_PATH, "r");
    if (fd == nullptr) {
        QF_LOG_ERROR("open file fail:%s", CONFIG_JSON_FILE_PATH);
        return false;
    }
    fseek(fd, 0, SEEK_END);
    int file_size = ftell(fd);
    if (file_size >= config_file_size) {
        QF_LOG_ERROR("config file is to large");
        fclose(fd);
        return false;
    }
    rewind(fd);
    char buffer[config_file_size];
    file_size = fread(buffer, 1, file_size, fd);
    fclose(fd);
    buffer[file_size] = 0;
    cJSON *root = cJSON_Parse(buffer);
    if (root == nullptr) {
        QF_LOG_ERROR("parse fail:%s", buffer);
        fclose(fd);
        return false;
    }
    cJSON *node = cJSON_GetObjectItem(root, "systemtray");
    if (node != nullptr && cJSON_IsString(node)) {
        snprintf(system_tray_icon_path, resource_path_size, "/Users/gaoyukun/github/qf/Resources/%s",
                 cJSON_GetStringValue(node));
    } else {
        snprintf(system_tray_icon_path, resource_path_size, "/Users/gaoyukun/github/qf/Resources/Qf.PNG");
    }

    node = cJSON_GetObjectItem(root, "module");
    if (node != nullptr && cJSON_IsArray(node)) {
        for (int i = 0; i < cJSON_GetArraySize(node); i++) {
            cJSON *model_ptr = cJSON_GetArrayItem(node, i);
            // 提取模型
            if (model_ptr != nullptr && cJSON_IsObject(model_ptr)) {
                cJSON *tmp_value = cJSON_GetObjectItem(model_ptr, "name");
                resource_loader::model tmp_model{};
                if (tmp_value != nullptr && cJSON_IsString(tmp_value)) {
                    snprintf((char *) tmp_model.name, resource_name_size, "%s", cJSON_GetStringValue(tmp_value));
                } else {
                    continue;
                }

                tmp_value = cJSON_GetObjectItem(model_ptr, "window_x");
                if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
                    tmp_model.model_with = (int) cJSON_GetNumberValue(tmp_value);
                } else {
                    continue;
                }

                tmp_value = cJSON_GetObjectItem(model_ptr, "window_y");
                if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
                    tmp_model.model_height = (int) cJSON_GetNumberValue(tmp_value);
                } else {
                    continue;
                }
                model_list.push_back(tmp_model);
            } else {
                continue;
            }
        }
    } else {
        cJSON_Delete(root);
        return false;
    }

    if (model_list.empty()) {
        cJSON_Delete(root);
        return false;
    }

    current_model_index = 0;
    current_model = &model_list[0];
    node = cJSON_GetObjectItem(root, "userdata");
    if (node != nullptr && cJSON_IsObject(node)) {
        cJSON *tmp_value = cJSON_GetObjectItem(node, "current_model");
        if (tmp_value != nullptr && cJSON_IsString(tmp_value)) {
            update_current_model(cJSON_GetStringValue(tmp_value));
        }

        tmp_value = cJSON_GetObjectItem(node, "top");
        if (tmp_value != nullptr && cJSON_IsBool(tmp_value)) {
            top_ = cJSON_IsTrue(tmp_value);
        } else {
            cJSON_AddBoolToObject(node, "top", false);
            top_ = false;
        }

//        tmp_value = cJSON_GetObjectItem(node, "move");
//        if (tmp_value != nullptr && cJSON_IsBool(tmp_value)) {
//            move = cJSON_IsTrue(tmp_value);
//        } else {
//            cJSON_AddBoolToObject(node, "move", false);
//            move = false;
//        }

        tmp_value = cJSON_GetObjectItem(node, "window_x");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            current_model_x = (int) cJSON_GetNumberValue(tmp_value);
        }

        tmp_value = cJSON_GetObjectItem(node, "window_y");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            current_model_y = (int) cJSON_GetNumberValue(tmp_value);
        }

        tmp_value = cJSON_GetObjectItem(node, "dialog_x");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            dialog_x = (int) cJSON_GetNumberValue(tmp_value);
        }

        tmp_value = cJSON_GetObjectItem(node, "dialog_y");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            dialog_y = (int) cJSON_GetNumberValue(tmp_value);
        }

        tmp_value = cJSON_GetObjectItem(node, "dialog_width");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            dialog_width = (int) cJSON_GetNumberValue(tmp_value);
        }

        tmp_value = cJSON_GetObjectItem(node, "dialog_height");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            dialog_height = (int) cJSON_GetNumberValue(tmp_value);
        }
    } else {
        top_ = false;
//        move = false;
    }

    //cJSON_Delete(root);
    json_root = (void *) root;
    is_init = true;
    return true;
}

void resource_loader::release() {
    if (!is_init) {
        return;
    }

    char *new_config = cJSON_Print((cJSON *) json_root);
    event_handler::get_instance().report(event_handler::event_type::app_config_change, new_config);
    cJSON_Delete((cJSON *) json_root);
    QF_LOG_INFO("release");
    is_init = false;
}

const std::vector<resource_loader::model> &resource_loader::get_model_list() {
    return model_list;
}

const char *resource_loader::get_system_tray_icon_path() {
    return system_tray_icon_path;
}

const resource_loader::model *resource_loader::get_current_model() {
    return current_model;
}

int resource_loader::get_current_model_index() const {
    return current_model_index;
}

bool resource_loader::update_current_model(const char *name) {
    for (uint32_t i = 0; i < model_list.size(); i++) {
        if (strncmp(name, model_list[i].name, resource_name_size) == 0) {
            current_model_index = i;
            current_model = &model_list[i];
            return true;
        }
    }
    return false;
}

bool resource_loader::update_current_model(uint32_t index) {
    if (index < model_list.size()) {
        if (&model_list[index] != current_model) {
            current_model = &model_list[index];
            cJSON *node = cJSON_GetObjectItem((cJSON *) json_root, "userdata");
            if (node != nullptr && cJSON_IsObject(node)) {
                cJSON *tmp_value = cJSON_GetObjectItem(node, "current_model");
                if (tmp_value != nullptr && cJSON_IsString(tmp_value)) {
                    cJSON_SetValuestring(tmp_value, current_model->name);
                } else {
                    cJSON *m = cJSON_CreateString(current_model->name);
                    cJSON_AddItemToObject(node, "current_model", m);
                }
            } else {
                cJSON *obj = cJSON_CreateObject();
                cJSON_AddItemToObject((cJSON *) json_root, "userdata", obj);
                cJSON *m = cJSON_CreateString(current_model->name);
                cJSON_AddItemToObject(node, "current_model", m);
            }
            current_model_index = index;
            char *new_config = cJSON_Print((cJSON *) json_root);
            event_handler::get_instance().report(event_handler::event_type::app_config_change, new_config);
            return true;
        }
    }
    return false;
}

bool resource_loader::is_top() const {
    return top_;
}

void resource_loader::set_top(bool top) {
    if (this->top_ != top) {
        this->top_ = top;
        cJSON *node = cJSON_GetObjectItem((cJSON *) json_root, "userdata");
        if (node != nullptr && cJSON_IsObject(node)) {

            cJSON *new_item = cJSON_CreateBool(top);
            cJSON_ReplaceItemInObject(node, "top", new_item);
        } else {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddItemToObject((cJSON *) json_root, "userdata", obj);
            cJSON *m = cJSON_CreateBool(top);
            cJSON_AddItemToObject(node, "top", m);
        }
        char *new_config = cJSON_Print((cJSON *) json_root);
        event_handler::get_instance().report(event_handler::event_type::app_config_change, new_config);
    }
}

//bool resource_loader::moveable() {
//    return move;
//}

//void resource_loader::set_moveable(bool m) {
//    if (this->move != m) {
//        this->move = m;
//        cJSON *node = cJSON_GetObjectItem((cJSON *) json_root, "userdata");
//        if (node != nullptr && cJSON_IsObject(node)) {
//            cJSON *new_item = cJSON_CreateBool(this->move);
//            cJSON_ReplaceItemInObject(node, "move", new_item);
//        } else {
//            cJSON *obj = cJSON_CreateObject();
//            cJSON_AddItemToObject((cJSON *) json_root, "userdata", obj);
//            cJSON *m = cJSON_CreateBool(this->move);
//            cJSON_AddItemToObject(node, "move", m);
//        }
//
//        char *new_config = cJSON_Print((cJSON *) json_root);
//        event_handler::get_instance().report(event_handler::event_type::app_config_change, new_config);
//    }
//}


bool resource_loader::update_current_model_size(int x, int y) {
    current_model->model_with = x;
    current_model->model_height = y;
    do {
        cJSON *node = cJSON_GetObjectItem((cJSON *) json_root, "module");
        if (node != nullptr && cJSON_IsArray(node)) {
            int index = current_model - &model_list[0];
            QF_LOG_INFO("index:%d", index);
            cJSON *model_ptr = cJSON_GetArrayItem(node, index);
            if (model_ptr != nullptr && cJSON_IsObject(model_ptr)) {
                cJSON *tmp_value = cJSON_GetObjectItem(model_ptr, "name");
                if (tmp_value != nullptr && cJSON_IsString(tmp_value)) {
                    if (strncmp(cJSON_GetStringValue(tmp_value), (char *) current_model->name, resource_name_size) !=
                        0) {
                        QF_LOG_ERROR("%s,%s", cJSON_GetStringValue(tmp_value), (char *) current_model->name);
                        break;
                    }
                } else {
                    break;
                }

                tmp_value = cJSON_GetObjectItem(model_ptr, "window_x");
                if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
                    cJSON_SetNumberValue(tmp_value, x);
                } else {
                    break;
                }

                tmp_value = cJSON_GetObjectItem(model_ptr, "window_y");
                if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
                    cJSON_SetNumberValue(tmp_value, y);
                } else {
                    break;
                }
                char *new_config = cJSON_Print((cJSON *) json_root);
                event_handler::get_instance().report(event_handler::event_type::app_config_change, new_config);
            }
            return true;
        }
    } while (false);
    return false;
}

bool resource_loader::update_current_model_position(int x, int y) {
    current_model_x = x;
    current_model_y = y;
    cJSON *node = cJSON_GetObjectItem((cJSON *) json_root, "userdata");
    if (node != nullptr && cJSON_IsObject(node)) {
        cJSON *tmp_value = cJSON_GetObjectItem(node, "window_x");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            cJSON_SetNumberValue(tmp_value, x);
        } else {
            return false;
        }

        tmp_value = cJSON_GetObjectItem(node, "window_y");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            cJSON_SetNumberValue(tmp_value, y);
        } else {
            return false;
        }
        char *new_config = cJSON_Print((cJSON *) json_root);
        event_handler::get_instance().report(event_handler::event_type::app_config_change, new_config);
    }
    qDebug("success_update_current_model_position:%d,%d", x, y);
    return true;
}

const char *resource_loader::get_config_path() {
    return CONFIG_JSON_FILE_PATH;
}

resource_loader::~resource_loader() {
    release();
}

bool resource_loader::update_dialog_position(int x, int y) {
    dialog_x = x;
    dialog_y = y;
    cJSON *node = cJSON_GetObjectItem((cJSON *) json_root, "userdata");
    if (node != nullptr && cJSON_IsObject(node)) {
        cJSON *tmp_value = cJSON_GetObjectItem(node, "dialog_x");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            cJSON_SetNumberValue(tmp_value, x);
        } else {
            return false;
        }

        tmp_value = cJSON_GetObjectItem(node, "dialog_y");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            cJSON_SetNumberValue(tmp_value, y);
        } else {
            return false;
        }
        char *new_config = cJSON_Print((cJSON *) json_root);
        event_handler::get_instance().report(event_handler::event_type::app_config_change, new_config);
    }
    return true;
}

bool resource_loader::update_dialog_size(int width, int height) {
    dialog_width = width;
    dialog_height = height;
    cJSON *node = cJSON_GetObjectItem((cJSON *) json_root, "userdata");
    if (node != nullptr && cJSON_IsObject(node)) {
        cJSON *tmp_value = cJSON_GetObjectItem(node, "dialog_width");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            cJSON_SetNumberValue(tmp_value, width);
        } else {
            return false;
        }

        tmp_value = cJSON_GetObjectItem(node, "dialog_height");
        if (tmp_value != nullptr && cJSON_IsNumber(tmp_value)) {
            cJSON_SetNumberValue(tmp_value, height);
        } else {
            return false;
        }
        char *new_config = cJSON_Print((cJSON *) json_root);
        event_handler::get_instance().report(event_handler::event_type::app_config_change, new_config);
    }
    return true;
}
