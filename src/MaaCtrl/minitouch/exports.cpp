// maa-ctrl-minitouch exports.cpp
// Wraps MinitouchController behind the maa_ctrl_* C-ABI.

#define MAA_CTRL_EXPORTS
#include "maa_ctrl_api.h"

#include <string>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

struct MinitouchState
{
    std::string uuid;
    std::string last_error;
    bool initialized = false;
};

extern "C"
{

EXPORT MaaCtrlHandle* maa_ctrl_create(const char* adb_path, const char* address, const char* config_json)
{
    // TODO: Integrate MinitouchImpl (MinitouchController) here.
    (void)adb_path; (void)address; (void)config_json;
    auto* state = new MinitouchState();
    state->last_error = "maa-ctrl-minitouch: not yet fully implemented";
    return reinterpret_cast<MaaCtrlHandle*>(state);
}

EXPORT void maa_ctrl_destroy(MaaCtrlHandle* handle)
{
    if (!handle) return;
    delete reinterpret_cast<MinitouchState*>(handle);
}

EXPORT bool maa_ctrl_inited(const MaaCtrlHandle* handle)
{
    if (!handle) return false;
    return reinterpret_cast<const MinitouchState*>(handle)->initialized;
}

EXPORT const char* maa_ctrl_get_uuid(const MaaCtrlHandle* handle)
{
    if (!handle) return nullptr;
    return reinterpret_cast<const MinitouchState*>(handle)->uuid.c_str();
}

EXPORT bool maa_ctrl_get_screen_res(const MaaCtrlHandle* handle, int32_t* w, int32_t* h) { (void)handle; (void)w; (void)h; return false; }

EXPORT int64_t maa_ctrl_support_features(const MaaCtrlHandle* handle)
{
    (void)handle;
    return MAA_CTRL_FEAT_SWIPE_WITH_PAUSE | MAA_CTRL_FEAT_PRECISE_SWIPE;
}

EXPORT uint64_t maa_ctrl_get_pipe_data_size(const MaaCtrlHandle* handle) { (void)handle; return 0; }
EXPORT uint64_t maa_ctrl_get_version(const MaaCtrlHandle* handle) { (void)handle; return 0; }

EXPORT bool maa_ctrl_screencap(MaaCtrlHandle* handle) { (void)handle; return false; }
EXPORT bool maa_ctrl_get_image(const MaaCtrlHandle* handle, uint32_t* w, uint32_t* h, const uint8_t** data, uint32_t* len) { (void)handle; (void)w; (void)h; (void)data; (void)len; return false; }

EXPORT bool maa_ctrl_click(MaaCtrlHandle* handle, int32_t x, int32_t y) { (void)handle; (void)x; (void)y; return false; }
EXPORT bool maa_ctrl_swipe(MaaCtrlHandle* handle, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t duration_ms, double slope_in, double slope_out, bool with_pause)
{
    (void)handle; (void)x1; (void)y1; (void)x2; (void)y2; (void)duration_ms; (void)slope_in; (void)slope_out; (void)with_pause;
    return false;
}
EXPORT bool maa_ctrl_press_esc(MaaCtrlHandle* handle) { (void)handle; return false; }
EXPORT bool maa_ctrl_input_text(MaaCtrlHandle* handle, const char* text) { (void)handle; (void)text; return false; }

EXPORT bool maa_ctrl_touch_down(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure) { (void)handle; (void)contact; (void)x; (void)y; (void)pressure; return false; }
EXPORT bool maa_ctrl_touch_move(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure) { (void)handle; (void)contact; (void)x; (void)y; (void)pressure; return false; }
EXPORT bool maa_ctrl_touch_up(MaaCtrlHandle* handle, int32_t contact) { (void)handle; (void)contact; return false; }
EXPORT bool maa_ctrl_touch_commit(MaaCtrlHandle* handle) { (void)handle; return false; }
EXPORT bool maa_ctrl_key_down(MaaCtrlHandle* handle, int32_t keycode) { (void)handle; (void)keycode; return false; }
EXPORT bool maa_ctrl_key_up(MaaCtrlHandle* handle, int32_t keycode) { (void)handle; (void)keycode; return false; }

EXPORT bool maa_ctrl_start_game(MaaCtrlHandle* handle, const char* package_name) { (void)handle; (void)package_name; return false; }
EXPORT bool maa_ctrl_stop_game(MaaCtrlHandle* handle, const char* package_name) { (void)handle; (void)package_name; return false; }

EXPORT bool maa_ctrl_back_to_home(MaaCtrlHandle* handle) { (void)handle; return false; }
EXPORT void maa_ctrl_set_kill_adb_on_exit(MaaCtrlHandle* handle, bool enable) { (void)handle; (void)enable; }
EXPORT void maa_ctrl_set_swipe_with_pause(MaaCtrlHandle* handle, bool enable) { (void)handle; (void)enable; }

EXPORT const char* maa_ctrl_last_error(const MaaCtrlHandle* handle)
{
    if (!handle) return nullptr;
    return reinterpret_cast<const MinitouchState*>(handle)->last_error.c_str();
}

EXPORT const char* maa_ctrl_version(void)
{
    return "maa-ctrl-minitouch 1.0.0";
}

} // extern "C"
