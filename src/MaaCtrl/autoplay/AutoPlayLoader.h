#pragma once

#include <cstdint>
#include <filesystem>

namespace asst
{

class AutoPlayLoader
{
public:
    AutoPlayLoader() = default;
    ~AutoPlayLoader();

    AutoPlayLoader(const AutoPlayLoader&) = delete;
    AutoPlayLoader& operator=(const AutoPlayLoader&) = delete;
    AutoPlayLoader(AutoPlayLoader&&) = delete;
    AutoPlayLoader& operator=(AutoPlayLoader&&) = delete;

    bool load(const std::filesystem::path& dll_path);
    void unload();
    bool loaded() const noexcept { return m_module != nullptr; }

    // ap_ffi functions
    void* create_android(const char* serial);
    void* create_windows(const char* title);
    void destroy(void* handle);
    bool inited(const void* handle);
    const char* get_uuid(const void* handle);
    bool get_screen_res(const void* handle, int32_t* w, int32_t* h);
    int64_t support_features(const void* handle);
    bool screencap(void* handle);
    bool get_image(const void* handle, uint32_t* w, uint32_t* h, const uint8_t** data, uint32_t* len);
    bool click(void* handle, int32_t x, int32_t y);
    bool swipe(void* handle, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t duration_ms, double slope_in,
               double slope_out);
    bool press_esc(void* handle);
    bool input_text(void* handle, const char* text);
    bool start_game(void* handle, const char* intent);
    bool stop_game(void* handle, const char* intent);
    const char* last_error(const void* handle);
    const char* version();

private:
    void* m_module = nullptr;

    // Function pointer types matching ap_ffi.h
    using CreateAndroidFunc = void* (*)(const char*);
    using CreateWindowsFunc = void* (*)(const char*);
    using DestroyFunc = void (*)(void*);
    using InitedFunc = bool (*)(const void*);
    using GetUuidFunc = const char* (*)(const void*);
    using GetScreenResFunc = bool (*)(const void*, int32_t*, int32_t*);
    using SupportFeaturesFunc = int64_t (*)(const void*);
    using ScreencapFunc = bool (*)(void*);
    using GetImageFunc = bool (*)(const void*, uint32_t*, uint32_t*, const uint8_t**, uint32_t*);
    using ClickFunc = bool (*)(void*, int32_t, int32_t);
    using SwipeFunc = bool (*)(void*, int32_t, int32_t, int32_t, int32_t, int32_t, double, double);
    using PressEscFunc = bool (*)(void*);
    using InputTextFunc = bool (*)(void*, const char*);
    using StartGameFunc = bool (*)(void*, const char*);
    using StopGameFunc = bool (*)(void*, const char*);
    using LastErrorFunc = const char* (*)(const void*);
    using VersionFunc = const char* (*)();

    CreateAndroidFunc m_create_android = nullptr;
    CreateWindowsFunc m_create_windows = nullptr;
    DestroyFunc m_destroy = nullptr;
    InitedFunc m_inited = nullptr;
    GetUuidFunc m_get_uuid = nullptr;
    GetScreenResFunc m_get_screen_res = nullptr;
    SupportFeaturesFunc m_support_features = nullptr;
    ScreencapFunc m_screencap = nullptr;
    GetImageFunc m_get_image = nullptr;
    ClickFunc m_click = nullptr;
    SwipeFunc m_swipe = nullptr;
    PressEscFunc m_press_esc = nullptr;
    InputTextFunc m_input_text = nullptr;
    StartGameFunc m_start_game = nullptr;
    StopGameFunc m_stop_game = nullptr;
    LastErrorFunc m_last_error = nullptr;
    VersionFunc m_version = nullptr;
};

} // namespace asst
