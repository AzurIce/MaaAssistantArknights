#pragma once

// Shared C-ABI implementation for ControllerAPI-backed maa-ctrl-* plugins.
//
// Each plugin compiles exactly one TU that:
//   1. includes this header,
//   2. defines `asst::ControllerAPI* maa_ctrl_plugin_create_controller(const MaaCtrlCreateInfo*)`,
//   3. optionally defines MAA_CTRL_PLUGIN_VERSION before including.

#define MAA_CTRL_EXPORTS
#include "maa_ctrl_api.h"

#include <cstdint>
#include <memory>
#include <string>

#include <opencv2/imgproc.hpp>

#include "Common/AsstMsg.h"
#include "ControllerAPI.h"
#include "Platform/PlatformIO.h"
#include "MaaUtils/NoWarningCV.hpp"

#ifndef MAA_CTRL_PLUGIN_VERSION
#define MAA_CTRL_PLUGIN_VERSION "maa-ctrl-plugin 1.0.0"
#endif

// 由各插件实现：按平台/回调构造具体 controller（尚未 connect）。
asst::ControllerAPI* maa_ctrl_plugin_create_controller(const MaaCtrlCreateInfo* info);

namespace maa_ctrl_plugin
{
struct State
{
    std::unique_ptr<asst::ControllerAPI> ctrl;
    cv::Mat bgr;
    cv::Mat rgba;
    std::string uuid;
    std::string error;
    bool inited = false;
};

inline asst::AsstCallback make_callback(const MaaCtrlCreateInfo* info)
{
    if (info == nullptr || info->callback == nullptr) {
        return nullptr;
    }
    auto* cb = info->callback;
    void* user = info->user_data;
    return [cb, user](asst::AsstMsg msg, const json::value& details, asst::Assistant*) {
        cb(static_cast<int32_t>(msg), details.to_string().c_str(), user);
    };
}

inline asst::PlatformType platform_of(const MaaCtrlCreateInfo* info)
{
    return info == nullptr ? asst::PlatformType::Native : static_cast<asst::PlatformType>(info->platform_type);
}

inline State* state(MaaCtrlHandle* handle)
{
    return reinterpret_cast<State*>(handle);
}

inline State* state(const MaaCtrlHandle* handle)
{
    return reinterpret_cast<State*>(const_cast<MaaCtrlHandle*>(handle));
}

inline bool inject(
    asst::ControllerAPI& ctrl,
    asst::InputEvent::Type type,
    int pointer_id,
    int x,
    int y,
    int keycode = 0)
{
    asst::InputEvent event;
    event.type = type;
    event.pointerId = pointer_id;
    event.point = asst::Point(x, y);
    event.keycode = keycode;
    return ctrl.inject_input_event(event);
}
} // namespace maa_ctrl_plugin

extern "C" {

MAA_CTRL_API MaaCtrlHandle* maa_ctrl_create(const MaaCtrlCreateInfo* info)
{
    if (info == nullptr) {
        return nullptr;
    }
    auto st = std::make_unique<maa_ctrl_plugin::State>();
    st->ctrl.reset(maa_ctrl_plugin_create_controller(info));
    if (!st->ctrl) {
        return nullptr;
    }
    const char* adb_path = info->adb_path != nullptr ? info->adb_path : "";
    const char* address = info->address != nullptr ? info->address : "";
    const char* config = info->config_json != nullptr ? info->config_json : "";
    if (!st->ctrl->connect(adb_path, address, config)) {
        return nullptr;
    }
    st->inited = st->ctrl->inited();
    st->uuid = st->ctrl->get_uuid();
    return reinterpret_cast<MaaCtrlHandle*>(st.release());
}

MAA_CTRL_API void maa_ctrl_destroy(MaaCtrlHandle* handle)
{
    delete maa_ctrl_plugin::state(handle);
}

MAA_CTRL_API bool maa_ctrl_inited(const MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    return st != nullptr && st->inited;
}

MAA_CTRL_API const char* maa_ctrl_get_uuid(const MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    return st != nullptr ? st->uuid.c_str() : nullptr;
}

MAA_CTRL_API bool maa_ctrl_get_screen_res(const MaaCtrlHandle* handle, int32_t* w, int32_t* h)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    auto [width, height] = st->ctrl->get_screen_res();
    if (w != nullptr) {
        *w = static_cast<int32_t>(width);
    }
    if (h != nullptr) {
        *h = static_cast<int32_t>(height);
    }
    return true;
}

MAA_CTRL_API int64_t maa_ctrl_support_features(const MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return 0;
    }
    return static_cast<int64_t>(st->ctrl->support_features());
}

MAA_CTRL_API uint64_t maa_ctrl_get_pipe_data_size(const MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    return st != nullptr && st->ctrl ? static_cast<uint64_t>(st->ctrl->get_pipe_data_size()) : 0;
}

MAA_CTRL_API uint64_t maa_ctrl_get_version(const MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    return st != nullptr && st->ctrl ? static_cast<uint64_t>(st->ctrl->get_version()) : 0;
}

MAA_CTRL_API bool maa_ctrl_screencap(MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    st->rgba.release();
    return st->ctrl->screencap(st->bgr);
}

MAA_CTRL_API bool maa_ctrl_get_image(
    const MaaCtrlHandle* handle,
    uint32_t* w,
    uint32_t* h,
    const uint8_t** data,
    uint32_t* len)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || st->bgr.empty() || w == nullptr || h == nullptr || data == nullptr || len == nullptr) {
        return false;
    }
    cv::cvtColor(st->bgr, st->rgba, cv::COLOR_BGR2RGBA);
    *w = static_cast<uint32_t>(st->rgba.cols);
    *h = static_cast<uint32_t>(st->rgba.rows);
    *data = st->rgba.data;
    *len = static_cast<uint32_t>(st->rgba.total() * st->rgba.elemSize());
    return true;
}

MAA_CTRL_API bool maa_ctrl_click(MaaCtrlHandle* handle, int32_t x, int32_t y)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return st->ctrl->click(asst::Point(static_cast<int>(x), static_cast<int>(y)));
}

MAA_CTRL_API bool maa_ctrl_swipe(
    MaaCtrlHandle* handle,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    int32_t duration_ms,
    int32_t extra_swipe,
    double slope_in,
    double slope_out,
    bool with_pause)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return st->ctrl->swipe(
        asst::Point(static_cast<int>(x1), static_cast<int>(y1)),
        asst::Point(static_cast<int>(x2), static_cast<int>(y2)),
        static_cast<int>(duration_ms),
        static_cast<asst::SwipeExtraDirection>(extra_swipe),
        slope_in,
        slope_out,
        with_pause);
}

MAA_CTRL_API bool maa_ctrl_press_esc(MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    return st != nullptr && st->ctrl && st->ctrl->press_esc();
}

MAA_CTRL_API bool maa_ctrl_input_text(MaaCtrlHandle* handle, const char* text)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return st->ctrl->input(text != nullptr ? text : "");
}

MAA_CTRL_API bool maa_ctrl_touch_down(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure)
{
    (void)pressure;
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return maa_ctrl_plugin::inject(
        *st->ctrl,
        asst::InputEvent::Type::TOUCH_DOWN,
        static_cast<int>(contact),
        static_cast<int>(x),
        static_cast<int>(y));
}

MAA_CTRL_API bool maa_ctrl_touch_move(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure)
{
    (void)pressure;
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return maa_ctrl_plugin::inject(
        *st->ctrl,
        asst::InputEvent::Type::TOUCH_MOVE,
        static_cast<int>(contact),
        static_cast<int>(x),
        static_cast<int>(y));
}

MAA_CTRL_API bool maa_ctrl_touch_up(MaaCtrlHandle* handle, int32_t contact)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return maa_ctrl_plugin::inject(*st->ctrl, asst::InputEvent::Type::TOUCH_UP, static_cast<int>(contact), 0, 0);
}

MAA_CTRL_API bool maa_ctrl_touch_commit(MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return maa_ctrl_plugin::inject(*st->ctrl, asst::InputEvent::Type::COMMIT, 0, 0, 0);
}

MAA_CTRL_API bool maa_ctrl_key_down(MaaCtrlHandle* handle, int32_t keycode)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return maa_ctrl_plugin::inject(
        *st->ctrl,
        asst::InputEvent::Type::KEY_DOWN,
        0,
        0,
        0,
        static_cast<int>(keycode));
}

MAA_CTRL_API bool maa_ctrl_key_up(MaaCtrlHandle* handle, int32_t keycode)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return maa_ctrl_plugin::inject(*st->ctrl, asst::InputEvent::Type::KEY_UP, 0, 0, 0, static_cast<int>(keycode));
}

MAA_CTRL_API bool maa_ctrl_start_game(MaaCtrlHandle* handle, const char* package_name)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return st->ctrl->start_game(package_name != nullptr ? package_name : "");
}

MAA_CTRL_API bool maa_ctrl_stop_game(MaaCtrlHandle* handle, const char* package_name)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    return st->ctrl->stop_game(package_name != nullptr ? package_name : "");
}

MAA_CTRL_API bool maa_ctrl_back_to_home(MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st == nullptr || !st->ctrl) {
        return false;
    }
    st->ctrl->back_to_home();
    return true;
}

MAA_CTRL_API void maa_ctrl_set_kill_adb_on_exit(MaaCtrlHandle* handle, bool enable)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st != nullptr && st->ctrl) {
        st->ctrl->set_kill_adb_on_exit(enable);
    }
}

MAA_CTRL_API void maa_ctrl_set_swipe_with_pause(MaaCtrlHandle* handle, bool enable)
{
    auto* st = maa_ctrl_plugin::state(handle);
    if (st != nullptr && st->ctrl) {
        st->ctrl->set_swipe_with_pause(enable);
    }
}

MAA_CTRL_API const char* maa_ctrl_last_error(const MaaCtrlHandle* handle)
{
    auto* st = maa_ctrl_plugin::state(handle);
    return st != nullptr ? st->error.c_str() : "";
}

MAA_CTRL_API const char* maa_ctrl_version(void)
{
    return MAA_CTRL_PLUGIN_VERSION;
}

} // extern "C"
