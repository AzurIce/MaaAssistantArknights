#include "MaaCtrlLoader.h"

#include "Utils/Logger.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace asst
{

MaaCtrlLoader::~MaaCtrlLoader()
{
    unload();
}

bool MaaCtrlLoader::load(const std::filesystem::path& dll_path)
{
    LogTraceFunction;

    if (m_module) {
        Log.warn("MaaCtrlLoader: already loaded");
        return true;
    }

    std::filesystem::path full_path = dll_path;

#ifdef _WIN32
    if (!full_path.has_extension()) {
        full_path += ".dll";
    }

    Log.info("MaaCtrlLoader: loading", full_path);

    m_module = LoadLibraryW(full_path.wstring().c_str());
    if (!m_module) {
        DWORD error = GetLastError();
        Log.error("MaaCtrlLoader: failed to load DLL, error code:", error);
        return false;
    }

    auto get_proc = [this](const char* name) -> void* {
        return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(m_module), name));
    };
#else
    if (!full_path.has_extension()) {
        full_path += ".so";
    }

    Log.info("MaaCtrlLoader: loading", full_path);

    m_module = dlopen(full_path.c_str(), RTLD_NOW);
    if (!m_module) {
        Log.error("MaaCtrlLoader: failed to load:", dlerror());
        return false;
    }

    auto get_proc = [this](const char* name) -> void* { return dlsym(m_module, name); };
#endif

    m_create = reinterpret_cast<CreateFunc>(get_proc("maa_ctrl_create"));
    m_destroy = reinterpret_cast<DestroyFunc>(get_proc("maa_ctrl_destroy"));
    m_inited = reinterpret_cast<InitedFunc>(get_proc("maa_ctrl_inited"));
    m_get_uuid = reinterpret_cast<GetUuidFunc>(get_proc("maa_ctrl_get_uuid"));
    m_get_screen_res = reinterpret_cast<GetScreenResFunc>(get_proc("maa_ctrl_get_screen_res"));
    m_support_features = reinterpret_cast<SupportFeaturesFunc>(get_proc("maa_ctrl_support_features"));
    m_get_pipe_data_size = reinterpret_cast<GetPipeDataSizeFunc>(get_proc("maa_ctrl_get_pipe_data_size"));
    m_get_version = reinterpret_cast<GetVersionFunc>(get_proc("maa_ctrl_get_version"));
    m_screencap = reinterpret_cast<ScreencapFunc>(get_proc("maa_ctrl_screencap"));
    m_get_image = reinterpret_cast<GetImageFunc>(get_proc("maa_ctrl_get_image"));
    m_click = reinterpret_cast<ClickFunc>(get_proc("maa_ctrl_click"));
    m_swipe = reinterpret_cast<SwipeFunc>(get_proc("maa_ctrl_swipe"));
    m_press_esc = reinterpret_cast<PressEscFunc>(get_proc("maa_ctrl_press_esc"));
    m_input_text = reinterpret_cast<InputTextFunc>(get_proc("maa_ctrl_input_text"));
    m_touch_down = reinterpret_cast<TouchDownFunc>(get_proc("maa_ctrl_touch_down"));
    m_touch_move = reinterpret_cast<TouchMoveFunc>(get_proc("maa_ctrl_touch_move"));
    m_touch_up = reinterpret_cast<TouchUpFunc>(get_proc("maa_ctrl_touch_up"));
    m_touch_commit = reinterpret_cast<TouchCommitFunc>(get_proc("maa_ctrl_touch_commit"));
    m_key_down = reinterpret_cast<KeyDownFunc>(get_proc("maa_ctrl_key_down"));
    m_key_up = reinterpret_cast<KeyUpFunc>(get_proc("maa_ctrl_key_up"));
    m_start_game = reinterpret_cast<StartGameFunc>(get_proc("maa_ctrl_start_game"));
    m_stop_game = reinterpret_cast<StopGameFunc>(get_proc("maa_ctrl_stop_game"));
    m_back_to_home = reinterpret_cast<BackToHomeFunc>(get_proc("maa_ctrl_back_to_home"));
    m_set_kill_adb_on_exit = reinterpret_cast<SetKillAdbOnExitFunc>(get_proc("maa_ctrl_set_kill_adb_on_exit"));
    m_set_swipe_with_pause = reinterpret_cast<SetSwipeWithPauseFunc>(get_proc("maa_ctrl_set_swipe_with_pause"));
    m_last_error = reinterpret_cast<LastErrorFunc>(get_proc("maa_ctrl_last_error"));
    m_version = reinterpret_cast<VersionFunc>(get_proc("maa_ctrl_version"));

    // Minimum required functions
    if (!m_create || !m_destroy || !m_screencap || !m_get_image || !m_click) {
        Log.error("MaaCtrlLoader: failed to resolve required function pointers");
        unload();
        return false;
    }

    if (m_version) {
        Log.info("MaaCtrlLoader: DLL version:", m_version());
    }

    return true;
}

void MaaCtrlLoader::unload()
{
    if (m_module) {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(m_module));
#else
        dlclose(m_module);
#endif
        m_module = nullptr;
    }

    m_create = nullptr;
    m_destroy = nullptr;
    m_inited = nullptr;
    m_get_uuid = nullptr;
    m_get_screen_res = nullptr;
    m_support_features = nullptr;
    m_get_pipe_data_size = nullptr;
    m_get_version = nullptr;
    m_screencap = nullptr;
    m_get_image = nullptr;
    m_click = nullptr;
    m_swipe = nullptr;
    m_press_esc = nullptr;
    m_input_text = nullptr;
    m_touch_down = nullptr;
    m_touch_move = nullptr;
    m_touch_up = nullptr;
    m_touch_commit = nullptr;
    m_key_down = nullptr;
    m_key_up = nullptr;
    m_start_game = nullptr;
    m_stop_game = nullptr;
    m_back_to_home = nullptr;
    m_set_kill_adb_on_exit = nullptr;
    m_set_swipe_with_pause = nullptr;
    m_last_error = nullptr;
    m_version = nullptr;
}

// --- Lifecycle ---

MaaCtrlHandle* MaaCtrlLoader::create(const char* adb_path, const char* address, const char* config_json)
{
    if (!m_create) {
        Log.error("maa_ctrl_create not available");
        return nullptr;
    }
    return m_create(adb_path, address, config_json);
}

void MaaCtrlLoader::destroy(MaaCtrlHandle* handle)
{
    if (m_destroy && handle) {
        m_destroy(handle);
    }
}

// --- Status ---

bool MaaCtrlLoader::inited(const MaaCtrlHandle* handle) const
{
    if (!m_inited || !handle) return false;
    return m_inited(handle);
}

const char* MaaCtrlLoader::get_uuid(const MaaCtrlHandle* handle) const
{
    if (!m_get_uuid || !handle) return nullptr;
    return m_get_uuid(handle);
}

bool MaaCtrlLoader::get_screen_res(const MaaCtrlHandle* handle, int32_t* w, int32_t* h) const
{
    if (!m_get_screen_res || !handle) return false;
    return m_get_screen_res(handle, w, h);
}

int64_t MaaCtrlLoader::support_features(const MaaCtrlHandle* handle) const
{
    if (!m_support_features || !handle) return 0;
    return m_support_features(handle);
}

uint64_t MaaCtrlLoader::get_pipe_data_size(const MaaCtrlHandle* handle) const
{
    if (!m_get_pipe_data_size || !handle) return 0;
    return m_get_pipe_data_size(handle);
}

uint64_t MaaCtrlLoader::get_version(const MaaCtrlHandle* handle) const
{
    if (!m_get_version || !handle) return 0;
    return m_get_version(handle);
}

// --- Screenshot ---

bool MaaCtrlLoader::screencap(MaaCtrlHandle* handle)
{
    if (!m_screencap || !handle) return false;
    return m_screencap(handle);
}

bool MaaCtrlLoader::get_image(const MaaCtrlHandle* handle, uint32_t* w, uint32_t* h,
                               const uint8_t** data, uint32_t* len) const
{
    if (!m_get_image || !handle) return false;
    return m_get_image(handle, w, h, data, len);
}

// --- Input ---

bool MaaCtrlLoader::click(MaaCtrlHandle* handle, int32_t x, int32_t y)
{
    if (!m_click || !handle) return false;
    return m_click(handle, x, y);
}

bool MaaCtrlLoader::swipe(MaaCtrlHandle* handle, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                           int32_t duration_ms, double slope_in, double slope_out, bool with_pause)
{
    if (!m_swipe || !handle) return false;
    return m_swipe(handle, x1, y1, x2, y2, duration_ms, slope_in, slope_out, with_pause);
}

bool MaaCtrlLoader::press_esc(MaaCtrlHandle* handle)
{
    if (!m_press_esc || !handle) return false;
    return m_press_esc(handle);
}

bool MaaCtrlLoader::input_text(MaaCtrlHandle* handle, const char* text)
{
    if (!m_input_text || !handle) return false;
    return m_input_text(handle, text);
}

// --- Multi-touch ---

bool MaaCtrlLoader::touch_down(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure)
{
    if (!m_touch_down || !handle) return false;
    return m_touch_down(handle, contact, x, y, pressure);
}

bool MaaCtrlLoader::touch_move(MaaCtrlHandle* handle, int32_t contact, int32_t x, int32_t y, int32_t pressure)
{
    if (!m_touch_move || !handle) return false;
    return m_touch_move(handle, contact, x, y, pressure);
}

bool MaaCtrlLoader::touch_up(MaaCtrlHandle* handle, int32_t contact)
{
    if (!m_touch_up || !handle) return false;
    return m_touch_up(handle, contact);
}

bool MaaCtrlLoader::touch_commit(MaaCtrlHandle* handle)
{
    if (!m_touch_commit || !handle) return false;
    return m_touch_commit(handle);
}

bool MaaCtrlLoader::key_down(MaaCtrlHandle* handle, int32_t keycode)
{
    if (!m_key_down || !handle) return false;
    return m_key_down(handle, keycode);
}

bool MaaCtrlLoader::key_up(MaaCtrlHandle* handle, int32_t keycode)
{
    if (!m_key_up || !handle) return false;
    return m_key_up(handle, keycode);
}

// --- Game control ---

bool MaaCtrlLoader::start_game(MaaCtrlHandle* handle, const char* package_name)
{
    if (!m_start_game || !handle) return false;
    return m_start_game(handle, package_name);
}

bool MaaCtrlLoader::stop_game(MaaCtrlHandle* handle, const char* package_name)
{
    if (!m_stop_game || !handle) return false;
    return m_stop_game(handle, package_name);
}

// --- Misc ---

bool MaaCtrlLoader::back_to_home(MaaCtrlHandle* handle)
{
    if (!m_back_to_home || !handle) return false;
    return m_back_to_home(handle);
}

void MaaCtrlLoader::set_kill_adb_on_exit(MaaCtrlHandle* handle, bool enable)
{
    if (m_set_kill_adb_on_exit && handle) {
        m_set_kill_adb_on_exit(handle, enable);
    }
}

void MaaCtrlLoader::set_swipe_with_pause(MaaCtrlHandle* handle, bool enable)
{
    if (m_set_swipe_with_pause && handle) {
        m_set_swipe_with_pause(handle, enable);
    }
}

// --- Error & version ---

const char* MaaCtrlLoader::last_error(const MaaCtrlHandle* handle) const
{
    if (!m_last_error || !handle) return nullptr;
    return m_last_error(handle);
}

const char* MaaCtrlLoader::version() const
{
    if (!m_version) return nullptr;
    return m_version();
}

} // namespace asst
