#ifndef RESOURCE_LOADED_HPP
#define RESOURCE_LOADED_HPP
/// TODO 添加记录模型大小的参数，添加一个记录对话框大小和位置的参数
#include <vector>
constexpr int resource_name_size = 32;
constexpr int resource_path_size = 64;
class resource_loader
{
public:
    struct model
    {
        char name[resource_name_size];
        int model_with;
        int model_height;
    };
    static resource_loader& get_instance()
    {
        static resource_loader ins;
        return ins;
    }
    bool initialize();
    void release();
    const std::vector<model>& get_model_list();
    const char* get_system_tray_icon_path();
    const model* get_current_model();
    bool update_current_model(const char* name);
    bool update_current_model(uint32_t index);
    bool update_current_model_size(int x, int y);
    bool update_dialog_size(int width, int height);
    bool update_current_model_position(int x, int y);
    bool update_dialog_position(int x, int y);
    int  get_current_model_index() const;
    const char* get_config_path();
//    bool moveable();
    void set_moveable(bool m);
    bool is_top();
    void set_top(bool top);
public:
    int current_model_x{800};
    int current_model_y{400};
    int dialog_x{100};
    int dialog_y{100};
    int dialog_width;
    int dialog_height;
    int screen_width{0};
    int screen_height{0};
private:
    bool is_init;
    bool top;
//    bool move;
    std::vector<model> model_list;
    char system_tray_icon_path[64];
     model* current_model;
    int current_model_index;
    void* json_root;
    resource_loader() {is_init = false;};
    ~resource_loader();
};



#endif
