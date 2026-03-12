#include "AutoPlayLoader.h"

#include "Utils/Logger.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace asst
{

AutoPlayLoader::~AutoPlayLoader()
{
    unload();
}

bool AutoPlayLoader::load(const std::filesystem::path& dll_path)
{
    LogTraceFunction;

    if (m_module) {
        Log.warn("ap_ffi already loaded");
        return true;
    }

    std::filesystem::path full_path = dll_path;

#ifdef _WIN32
    if (!full_path.has_extension()) {
        full_path += ".dll";
    }

    Log.info("Loading ap_ffi from", full_path);

    m_module = LoadLibraryW(full_path.wstring().c_str());
    if (!m_module) {
        DWORD error = GetLastError();
        Log.error("Failed to load ap_ffi DLL, error code:", error);
        return false;
    }

    auto get_proc = [this](const char* name) -> void* {
        return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(m_module), name));
    };
#else
    if (!full_path.has_extension()) {
        full_path += ".so";
    }

    Log.info("Loading ap_ffi from", full_path);

    m_module = dlopen(full_path.c_str(), RTLD_NOW);
    if (!m_module) {
        Log.error("Failed to load ap_ffi:", dlerror());
        return false;
    }

    auto get_proc = [this](const char* name) -> void* { return dlsym(m_module, name); };
#endif

    m_create_android = reinterpret_cast<CreateAndroidFunc>(get_proc("ap_create_android"));
    m_create_windows = reinterpret_cast<CreateWindowsFunc>(get_proc("ap_create_windows"));
    m_destroy = reinterpret_cast<DestroyFunc>(get_proc("ap_destroy"));
    m_inited = reinterpret_cast<InitedFunc>(get_proc("ap_inited"));
    m_get_uuid = reinterpret_cast<GetUuidFunc>(get_proc("ap_get_uuid"));
    m_get_screen_res = reinterpret_cast<GetScreenResFunc>(get_proc("ap_get_screen_res"));
    m_support_features = reinterpret_cast<SupportFeaturesFunc>(get_proc("ap_support_features"));
    m_screencap = reinterpret_cast<ScreencapFunc>(get_proc("ap_screencap"));
    m_get_image = reinterpret_cast<GetImageFunc>(get_proc("ap_get_image"));
    m_click = reinterpret_cast<ClickFunc>(get_proc("ap_click"));
    m_swipe = reinterpret_cast<SwipeFunc>(get_proc("ap_swipe"));
    m_press_esc = reinterpret_cast<PressEscFunc>(get_proc("ap_press_esc"));
    m_input_text = reinterpret_cast<InputTextFunc>(get_proc("ap_input_text"));
    m_start_game = reinterpret_cast<StartGameFunc>(get_proc("ap_start_game"));
    m_stop_game = reinterpret_cast<StopGameFunc>(get_proc("ap_stop_game"));
    m_last_error = reinterpret_cast<LastErrorFunc>(get_proc("ap_last_error"));
    m_version = reinterpret_cast<VersionFunc>(get_proc("ap_version"));

    // Minimum required functions
    if (!m_create_android || !m_destroy || !m_screencap || !m_get_image || !m_click) {
        Log.error("Failed to resolve required ap_ffi function pointers");
        unload();
        return false;
    }

    if (m_version) {
        Log.info("ap_ffi version:", m_version());
    }

    return true;
}

void AutoPlayLoader::unload()
{
    if (m_module) {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(m_module));
#else
        dlclose(m_module);
#endif
        m_module = nullptr;
    }

    m_create_android = nullptr;
    m_create_windows = nullptr;
    m_destroy = nullptr;
    m_inited = nullptr;
    m_get_uuid = nullptr;
    m_get_screen_res = nullptr;
    m_support_features = nullptr;
    m_screencap = nullptr;
    m_get_image = nullptr;
    m_click = nullptr;
    m_swipe = nullptr;
    m_press_esc = nullptr;
    m_input_text = nullptr;
    m_start_game = nullptr;
    m_stop_game = nullptr;
    m_last_error = nullptr;
    m_version = nullptr;
}

void* AutoPlayLoader::create_android(const char* serial)
{
    if (!m_create_android) {
        Log.error("ap_create_android not available");
        return nullptr;
    }
    return m_create_android(serial);
}

void* AutoPlayLoader::create_windows(const char* title)
{
    if (!m_create_windows) {
        Log.error("ap_create_windows not available");
        return nullptr;
    }
    return m_create_windows(title);
}

void AutoPlayLoader::destroy(void* handle)
{
    if (m_destroy && handle) {
        m_destroy(handle);
    }
}

bool AutoPlayLoader::inited(const void* handle)
{
    if (!m_inited || !handle) return false;
    return m_inited(handle);
}

const char* AutoPlayLoader::get_uuid(const void* handle)
{
    if (!m_get_uuid || !handle) return nullptr;
    return m_get_uuid(handle);
}

bool AutoPlayLoader::get_screen_res(const void* handle, int32_t* w, int32_t* h)
{
    if (!m_get_screen_res || !handle) return false;
    return m_get_screen_res(handle, w, h);
}

int64_t AutoPlayLoader::support_features(const void* handle)
{
    if (!m_support_features || !handle) return 0;
    return m_support_features(handle);
}

bool AutoPlayLoader::screencap(void* handle)
{
    if (!m_screencap || !handle) return false;
    return m_screencap(handle);
}

bool AutoPlayLoader::get_image(const void* handle, uint32_t* w, uint32_t* h, const uint8_t** data, uint32_t* len)
{
    if (!m_get_image || !handle) return false;
    return m_get_image(handle, w, h, data, len);
}

bool AutoPlayLoader::click(void* handle, int32_t x, int32_t y)
{
    if (!m_click || !handle) return false;
    return m_click(handle, x, y);
}

bool AutoPlayLoader::swipe(void* handle, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t duration_ms,
                            double slope_in, double slope_out)
{
    if (!m_swipe || !handle) return false;
    return m_swipe(handle, x1, y1, x2, y2, duration_ms, slope_in, slope_out);
}

bool AutoPlayLoader::press_esc(void* handle)
{
    if (!m_press_esc || !handle) return false;
    return m_press_esc(handle);
}

bool AutoPlayLoader::input_text(void* handle, const char* text)
{
    if (!m_input_text || !handle) return false;
    return m_input_text(handle, text);
}

bool AutoPlayLoader::start_game(void* handle, const char* intent)
{
    if (!m_start_game || !handle) return false;
    return m_start_game(handle, intent);
}

bool AutoPlayLoader::stop_game(void* handle, const char* intent)
{
    if (!m_stop_game || !handle) return false;
    return m_stop_game(handle, intent);
}

const char* AutoPlayLoader::last_error(const void* handle)
{
    if (!m_last_error || !handle) return nullptr;
    return m_last_error(handle);
}

const char* AutoPlayLoader::version()
{
    if (!m_version) return nullptr;
    return m_version();
}

} // namespace asst
