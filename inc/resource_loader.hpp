#ifndef RESOURCE_LOADED_HPP
#define RESOURCE_LOADED_HPP
/// TODO 添加记录模型大小的参数，添加一个记录对话框大小和位置的参数
#include <vector>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

constexpr int resource_name_size = 32;
constexpr int resource_path_size = 64;

class resource_loader {
public:
    struct model {
        QString name;
        int model_width{400};
        int model_height{400};
    };

    static resource_loader &get_instance() {
        static resource_loader ins;
        return ins;
    }

    bool initialize();

    void release();

    const QVector<model> &get_model_list();

    QString get_system_tray_icon_path();

    const model *get_current_model();

    bool update_current_model(QString name);

    bool update_current_model(uint32_t index);

    bool update_current_model_size(int x, int y);

    bool update_dialog_size(int width, int height);

    bool update_current_model_position(int x, int y);

    bool update_dialog_position(int x, int y);

    const QString &get_gpt_url() const;

    const QString &get_gpt_key() const;

    int get_current_model_index() const;

    bool save_config();

    QString get_config_path() const;

//    bool moveable();
//    void set_moveable(bool m);
    bool is_top() const;

    void set_top(bool top);

public:
    int current_model_x{800};
    int current_model_y{400};
    int dialog_x{100};
    int dialog_y{100};
    int dialog_width{400};
    int dialog_height{400};
    int screen_width{0};
    int screen_height{0};
    bool tts_enable_{false};
    bool gpt_enable_{false};
private:
    bool config_change{false};
    bool is_init;
    bool top_;
    QVector<model> model_list;
    QString system_tray_icon_path;
    QString resource_file_path;
    QString gpt_api_url;
    QString gpt_api_key;
    int current_model_index;
    QJsonObject root_;

//    void* json_root;
    resource_loader() { is_init = false; };

    ~resource_loader();
};


#endif
