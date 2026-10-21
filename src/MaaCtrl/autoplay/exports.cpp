// maa-ctrl-autoplay exports.cpp
// Wraps AutoPlayController (which internally loads ap_ffi) behind the maa_ctrl_* C-ABI.

#define MAA_CTRL_EXPORTS
#include "maa_ctrl_api.h"
#include "AutoPlayLoader.h"

#include <cstring>
#include <string>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#endif

// Internal state wrapping the ap_ffi handle
struct AutoPlayState
{
    asst::AutoPlayLoader loader;
    void* handle = nullptr;
    bool initialized = false;
    std::string uuid;
    std::string last_error_msg;
    int32_t screen_w = 0;
    int32_t screen_h = 0;
};

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

extern "C"
{

EXPORT MaaCtrlHandle* maa_ctrl_create(const char* adb_path, const char* address, const char* config_json)
{
    (void)adb_path;
    (void)config_json;

#ifdef _WIN32
    // Free any console window that might have been created by dependencies
    FreeConsole();
#endif

    auto* state = new AutoPlayState();

    if (!state->loader.load("ap_ffi")) {
        state->last_error_msg = "Failed to load ap_ffi";
        delete state;
        return nullptr;
    }

    state->handle = state->loader.create_android(address);
    if (!state->handle) {
        const char* err = state->loader.last_error(nullptr);
        state->last_error_msg = err ? err : "Failed to create handle";
        delete state;
        return nullptr;
    }

    if (!state->loader.inited(state->handle)) {
        const char* err = state->loader.last_error(state->handle);
        state->last_error_msg = err ? err : "Handle not initialized";
        state->loader.destroy(state->handle);
        delete state;
        return nullptr;
    }

    const char* uuid = state->loader.get_uuid(state->handle);
    state->uuid = uuid ? uuid : "";

    state->loader.get_screen_res(state->handle, &state->screen_w, &state->screen_h);

    state->initialized = true;
    return reinterpret_cast<MaaCtrlHandle*>(state);
}

EXPORT void maa_ctrl_destroy(MaaCtrlHandle* handle)
{
    if (!handle) return;
    auto* state = reinterpret_cast<AutoPlayState*>(handle);
    if (state->handle) {
        state->loader.destroy(state->handle);
    }
    delete state;
}

EXPORT bool maa_ctrl_inited(const MaaCtrlHandle* handle)
{
    if (!handle) return false;
    return reinterpret_cast<const AutoPlayState*>(handle)->initialized;
}

EXPORT const char* maa_ctrl_get_uuid(const MaaCtrlHandle* handle)
{
    if (!handle) return nullptr;
    return reinterpret_cast<const AutoPlayState*>(handle)->uuid.c_str();
}

EXPORT bool maa_ctrl_get_screen_res(const MaaCtrlHandle* handle, int32_t* w, int32_t* h)
{
    if (!handle) return false;
    auto* state = reinterpret_cast<const AutoPlayState*>(handle);
    if (w) *w = state->screen_w;
    if (h) *h = state->screen_h;
    return true;
}

EXPORT int64_t maa_ctrl_support_features(const MaaCtrlHandle* handle)
{
    if (!handle) return 0;
    auto* state = reinterpret_cast<const AutoPlayState*>(handle);
    return state->loader.support_features(state->handle);
}

EXPORT uint64_t maa_ctrl_get_pipe_data_size(const MaaCtrlHandle* handle)
{
    (void)handle;
    return 0;
}

EXPORT uint64_t maa_ctrl_get_version(const MaaCtrlHandle* handle)
{
    (void)handle;
    return 0;
}

EXPORT bool maa_ctrl_screencap(MaaCtrlHandle* handle)
{
    if (!handle) return false;
    auto* state = reinterpret_cast<AutoPlayState*>(handle);
    return state->loader.screencap(state->handle);
}

EXPORT bool maa_ctrl_get_image(const MaaCtrlHandle* handle,
                               uint32_t* w, uint32_t* h,
                               const uint8_t** data, uint32_t* len)
{
    if (!handle) return false;
    auto* state = reinterpret_cast<const AutoPlayState*>(handle);
    return state->loader.get_image(state->handle, w, h, data, len);
}

EXPORT bool maa_ctrl_click(MaaCtrlHandle* handle, int32_t x, int32_t y)
{
    if (!handle) return false;
    auto* state = reinterpret_cast<AutoPlayState*>(handle);
    return state->loader.click(state->handle, x, y);
}

EXPORT bool maa_ctrl_swipe(MaaCtrlHandle* handle,
                           int32_t x1, int32_t y1,
                           int32_t x2, int32_t y2,
                           int32_t duration_ms,
                           double slope_in, double slope_out,
                           bool with_pause)
{
    (void)with_pause;
    if (!handle) return false;
    auto* state = reinterpret_cast<AutoPlayState*>(handle);
    return state->loader.swipe(state->handle, x1, y1, x2, y2, duration_ms, slope_in, slope_out);
}

EXPORT bool maa_ctrl_press_esc(MaaCtrlHandle* handle)
{
    if (!handle) return false;
    auto* state = reinterpret_cast<AutoPlayState*>(handle);
    return state->loader.press_esc(state->handle);
}

EXPORT bool maa_ctrl_input_text(MaaCtrlHandle* handle, const char* text)
{
    if (!handle) return false;
    auto* state = reinterpret_cast<AutoPlayState*>(handle);
    return state->loader.input_text(state->handle, text);
}

EXPORT bool maa_ctrl_touch_down(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure)
{
    (void)handle; (void)contact; (void)x; (void)y; (void)pressure;
    return false; // Not yet implemented for AutoPlay
}

EXPORT bool maa_ctrl_touch_move(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure)
{
    (void)handle; (void)contact; (void)x; (void)y; (void)pressure;
    return false;
}

EXPORT bool maa_ctrl_touch_up(MaaCtrlHandle* handle, int32_t contact)
{
    (void)handle; (void)contact;
    return false;
}

EXPORT bool maa_ctrl_touch_commit(MaaCtrlHandle* handle)
{
    (void)handle;
    return false;
}

EXPORT bool maa_ctrl_key_down(MaaCtrlHandle* handle, int32_t keycode)
{
    (void)handle; (void)keycode;
    return false;
}

EXPORT bool maa_ctrl_key_up(MaaCtrlHandle* handle, int32_t keycode)
{
    (void)handle; (void)keycode;
    return false;
}

EXPORT bool maa_ctrl_start_game(MaaCtrlHandle* handle, const char* package_name)
{
    if (!handle || !package_name) return false;
    auto* state = reinterpret_cast<AutoPlayState*>(handle);
    return state->loader.start_game(state->handle, package_name);
}

EXPORT bool maa_ctrl_stop_game(MaaCtrlHandle* handle, const char* package_name)
{
    if (!handle || !package_name) return false;
    auto* state = reinterpret_cast<AutoPlayState*>(handle);
    return state->loader.stop_game(state->handle, package_name);
}

EXPORT bool maa_ctrl_back_to_home(MaaCtrlHandle* handle)
{
    (void)handle;
    return false;
}

EXPORT void maa_ctrl_set_kill_adb_on_exit(MaaCtrlHandle* handle, bool enable)
{
    (void)handle; (void)enable;
}

EXPORT void maa_ctrl_set_swipe_with_pause(MaaCtrlHandle* handle, bool enable)
{
    (void)handle; (void)enable;
}

EXPORT const char* maa_ctrl_last_error(const MaaCtrlHandle* handle)
{
    if (!handle) return nullptr;
    auto* state = reinterpret_cast<const AutoPlayState*>(handle);
    if (!state->last_error_msg.empty()) return state->last_error_msg.c_str();
    return state->loader.last_error(state->handle);
}

EXPORT const char* maa_ctrl_version(void)
{
    return "maa-ctrl-autoplay 1.0.0";
}

} // extern "C"
