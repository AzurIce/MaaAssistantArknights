#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include "maa_ctrl_api.h"

namespace asst
{

// Unified DLL loader for all maa-ctrl-* DLLs.
// Replaces Win32ControlUnitLoader and AutoPlayLoader.
class MaaCtrlLoader
{
public:
    MaaCtrlLoader() = default;
    ~MaaCtrlLoader();

    MaaCtrlLoader(const MaaCtrlLoader&) = delete;
    MaaCtrlLoader& operator=(const MaaCtrlLoader&) = delete;
    MaaCtrlLoader(MaaCtrlLoader&&) = delete;
    MaaCtrlLoader& operator=(MaaCtrlLoader&&) = delete;

    bool load(const std::filesystem::path& dll_path);
    void unload();
    bool loaded() const noexcept { return m_module != nullptr; }

    // --- Lifecycle ---
    MaaCtrlHandle* create(const char* adb_path, const char* address, const char* config_json);
    void destroy(MaaCtrlHandle* handle);

    // --- Status ---
    bool inited(const MaaCtrlHandle* handle);
    const char* get_uuid(const MaaCtrlHandle* handle);
    bool get_screen_res(const MaaCtrlHandle* handle, int32_t* w, int32_t* h);
    int64_t support_features(const MaaCtrlHandle* handle);
    uint64_t get_pipe_data_size(const MaaCtrlHandle* handle);
    uint64_t get_version(const MaaCtrlHandle* handle);

    // --- Screenshot ---
    bool screencap(MaaCtrlHandle* handle);
    bool get_image(const MaaCtrlHandle* handle, uint32_t* w, uint32_t* h,
                   const uint8_t** data, uint32_t* len);

    // --- Input ---
    bool click(MaaCtrlHandle* handle, int32_t x, int32_t y);
    bool swipe(MaaCtrlHandle* handle, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
               int32_t duration_ms, double slope_in, double slope_out, bool with_pause);
    bool press_esc(MaaCtrlHandle* handle);
    bool input_text(MaaCtrlHandle* handle, const char* text);

    // --- Multi-touch ---
    bool touch_down(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure);
    bool touch_move(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure);
    bool touch_up(MaaCtrlHandle* handle, int32_t contact);
    bool touch_commit(MaaCtrlHandle* handle);
    bool key_down(MaaCtrlHandle* handle, int32_t keycode);
    bool key_up(MaaCtrlHandle* handle, int32_t keycode);

    // --- Game control ---
    bool start_game(MaaCtrlHandle* handle, const char* package_name);
    bool stop_game(MaaCtrlHandle* handle, const char* package_name);

    // --- Misc ---
    bool back_to_home(MaaCtrlHandle* handle);
    void set_kill_adb_on_exit(MaaCtrlHandle* handle, bool enable);
    void set_swipe_with_pause(MaaCtrlHandle* handle, bool enable);

    // --- Error & version ---
    const char* last_error(const MaaCtrlHandle* handle);
    const char* version();

private:
    void* m_module = nullptr;

    // Function pointer types matching maa_ctrl_api.h
    using CreateFunc = MaaCtrlHandle* (*)(const char*, const char*, const char*);
    using DestroyFunc = void (*)(MaaCtrlHandle*);
    using InitedFunc = bool (*)(const MaaCtrlHandle*);
    using GetUuidFunc = const char* (*)(const MaaCtrlHandle*);
    using GetScreenResFunc = bool (*)(const MaaCtrlHandle*, int32_t*, int32_t*);
    using SupportFeaturesFunc = int64_t (*)(const MaaCtrlHandle*);
    using GetPipeDataSizeFunc = uint64_t (*)(const MaaCtrlHandle*);
    using GetVersionFunc = uint64_t (*)(const MaaCtrlHandle*);
    using ScreencapFunc = bool (*)(MaaCtrlHandle*);
    using GetImageFunc = bool (*)(const MaaCtrlHandle*, uint32_t*, uint32_t*, const uint8_t**, uint32_t*);
    using ClickFunc = bool (*)(MaaCtrlHandle*, int32_t, int32_t);
    using SwipeFunc = bool (*)(MaaCtrlHandle*, int32_t, int32_t, int32_t, int32_t, int32_t, double, double, bool);
    using PressEscFunc = bool (*)(MaaCtrlHandle*);
    using InputTextFunc = bool (*)(MaaCtrlHandle*, const char*);
    using TouchDownFunc = bool (*)(MaaCtrlHandle*, int32_t, int32_t, int32_t, int32_t);
    using TouchMoveFunc = bool (*)(MaaCtrlHandle*, int32_t, int32_t, int32_t, int32_t);
    using TouchUpFunc = bool (*)(MaaCtrlHandle*, int32_t);
    using TouchCommitFunc = bool (*)(MaaCtrlHandle*);
    using KeyDownFunc = bool (*)(MaaCtrlHandle*, int32_t);
    using KeyUpFunc = bool (*)(MaaCtrlHandle*, int32_t);
    using StartGameFunc = bool (*)(MaaCtrlHandle*, const char*);
    using StopGameFunc = bool (*)(MaaCtrlHandle*, const char*);
    using BackToHomeFunc = bool (*)(MaaCtrlHandle*);
    using SetKillAdbOnExitFunc = void (*)(MaaCtrlHandle*, bool);
    using SetSwipeWithPauseFunc = void (*)(MaaCtrlHandle*, bool);
    using LastErrorFunc = const char* (*)(const MaaCtrlHandle*);
    using VersionFunc = const char* (*)();

    CreateFunc m_create = nullptr;
    DestroyFunc m_destroy = nullptr;
    InitedFunc m_inited = nullptr;
    GetUuidFunc m_get_uuid = nullptr;
    GetScreenResFunc m_get_screen_res = nullptr;
    SupportFeaturesFunc m_support_features = nullptr;
    GetPipeDataSizeFunc m_get_pipe_data_size = nullptr;
    GetVersionFunc m_get_version = nullptr;
    ScreencapFunc m_screencap = nullptr;
    GetImageFunc m_get_image = nullptr;
    ClickFunc m_click = nullptr;
    SwipeFunc m_swipe = nullptr;
    PressEscFunc m_press_esc = nullptr;
    InputTextFunc m_input_text = nullptr;
    TouchDownFunc m_touch_down = nullptr;
    TouchMoveFunc m_touch_move = nullptr;
    TouchUpFunc m_touch_up = nullptr;
    TouchCommitFunc m_touch_commit = nullptr;
    KeyDownFunc m_key_down = nullptr;
    KeyUpFunc m_key_up = nullptr;
    StartGameFunc m_start_game = nullptr;
    StopGameFunc m_stop_game = nullptr;
    BackToHomeFunc m_back_to_home = nullptr;
    SetKillAdbOnExitFunc m_set_kill_adb_on_exit = nullptr;
    SetSwipeWithPauseFunc m_set_swipe_with_pause = nullptr;
    LastErrorFunc m_last_error = nullptr;
    VersionFunc m_version = nullptr;
};

} // namespace asst
