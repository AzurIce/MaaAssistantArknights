#include "DllController.h"

#include "MaaUtils/NoWarningCV.hpp"

#include "Utils/Logger.hpp"

namespace asst
{

DllController::DllController(const AsstCallback& callback, Assistant* inst, const std::string& dll_name) :
    InstHelper(inst),
    m_callback(callback),
    m_dll_name(dll_name)
{
    LogTraceFunction;
}

DllController::~DllController()
{
    LogTraceFunction;

    if (m_handle) {
        m_loader.destroy(m_handle);
        m_handle = nullptr;
    }
}

bool DllController::connect(const std::string& adb_path, const std::string& address, const std::string& config)
{
    LogTraceFunction;

    // Load the DLL (searched via standard library path: exe dir, system dirs, etc.)
    if (!m_loader.load(m_dll_name)) {
        Log.error("DllController: failed to load", m_dll_name);
        return false;
    }

    m_handle = m_loader.create(adb_path.c_str(), address.c_str(), config.c_str());
    if (!m_handle) {
        Log.error("DllController: failed to create handle for", m_dll_name,
                  "(handle is nullptr, check DLL create() implementation)");
        return false;
    }

    if (!m_loader.inited(m_handle)) {
        const char* err = m_loader.last_error(m_handle);
        Log.error("DllController: handle not initialized:", err ? err : "unknown error");
        m_loader.destroy(m_handle);
        m_handle = nullptr;
        return false;
    }

    // Cache UUID
    const char* uuid = m_loader.get_uuid(m_handle);
    m_uuid = uuid ? uuid : "";
    Log.info("DllController UUID:", m_uuid);

    // Cache screen resolution
    int32_t w = 0, h = 0;
    if (m_loader.get_screen_res(m_handle, &w, &h)) {
        m_screen_size = { static_cast<int>(w), static_cast<int>(h) };
        Log.info("DllController screen resolution:", w, "x", h);
    }
    else {
        Log.warn("DllController: failed to get screen resolution");
    }

    m_inited = true;
    return true;
}

bool DllController::inited() const noexcept
{
    return m_inited;
}

void DllController::set_swipe_with_pause(bool enable) noexcept
{
    if (m_handle) {
        m_loader.set_swipe_with_pause(m_handle, enable);
    }
}

void DllController::set_kill_adb_on_exit(bool enable) noexcept
{
    if (m_handle) {
        m_loader.set_kill_adb_on_exit(m_handle, enable);
    }
}

const std::string& DllController::get_uuid() const
{
    return m_uuid;
}

size_t DllController::get_pipe_data_size() const noexcept
{
    if (!m_handle) return 0;
    return static_cast<size_t>(m_loader.get_pipe_data_size(m_handle));
}

size_t DllController::get_version() const noexcept
{
    if (!m_handle) return 0;
    return static_cast<size_t>(m_loader.get_version(m_handle));
}

bool DllController::screencap(cv::Mat& image_payload, bool allow_reconnect [[maybe_unused]])
{
    if (!m_handle) return false;

    if (!m_loader.screencap(m_handle)) {
        const char* err = m_loader.last_error(m_handle);
        Log.error("DllController screencap failed:", err ? err : "unknown");
        return false;
    }

    uint32_t w = 0, h = 0, len = 0;
    const uint8_t* data = nullptr;
    if (!m_loader.get_image(m_handle, &w, &h, &data, &len)) {
        Log.error("DllController get_image failed");
        return false;
    }

    // ap_ffi returns RGBA raw bytes (4 channels), need to convert to BGR for OpenCV
    // or try interpreting as BGR first if it's already in the right format
    cv::Mat rgba(static_cast<int>(h), static_cast<int>(w), CV_8UC4, const_cast<uint8_t*>(data));

    // Convert RGBA -> BGR (OpenCV uses BGR by default)
    cv::cvtColor(rgba, image_payload, cv::COLOR_RGBA2BGR);

    m_screen_size = { static_cast<int>(w), static_cast<int>(h) };
    return true;
}

bool DllController::start_game(const std::string& client_type)
{
    if (!m_handle || client_type.empty()) return false;
    return m_loader.start_game(m_handle, client_type.c_str());
}

bool DllController::stop_game(const std::string& client_type)
{
    if (!m_handle || client_type.empty()) return false;
    return m_loader.stop_game(m_handle, client_type.c_str());
}

bool DllController::click(const Point& p)
{
    if (!m_handle) return false;
    return m_loader.click(m_handle, p.x, p.y);
}

bool DllController::input(const std::string& text)
{
    if (!m_handle) return false;
    return m_loader.input_text(m_handle, text.c_str());
}

bool DllController::swipe(
    const Point& p1,
    const Point& p2,
    int duration,
    bool extra_swipe [[maybe_unused]],
    double slope_in,
    double slope_out,
    bool with_pause)
{
    if (!m_handle) return false;
    return m_loader.swipe(m_handle, p1.x, p1.y, p2.x, p2.y,
                          static_cast<int32_t>(duration), slope_in, slope_out, with_pause);
}

bool DllController::inject_input_event(const InputEvent& event)
{
    if (!m_handle) return false;

    switch (event.type) {
    case InputEvent::Type::TOUCH_DOWN:
        return m_loader.touch_down(m_handle, event.pointerId, event.point.x, event.point.y, 0);
    case InputEvent::Type::TOUCH_MOVE:
        return m_loader.touch_move(m_handle, event.pointerId, event.point.x, event.point.y, 0);
    case InputEvent::Type::TOUCH_UP:
        return m_loader.touch_up(m_handle, event.pointerId);
    case InputEvent::Type::TOUCH_RESET:
        // Reset all touches — commit with no pending events
        return m_loader.touch_commit(m_handle);
    case InputEvent::Type::KEY_DOWN:
        return m_loader.key_down(m_handle, event.keycode);
    case InputEvent::Type::KEY_UP:
        return m_loader.key_up(m_handle, event.keycode);
    case InputEvent::Type::COMMIT:
        return m_loader.touch_commit(m_handle);
    case InputEvent::Type::WAIT_MS:
        // Handled by caller (sleep); DLL does not need to know
        return true;
    default:
        return false;
    }
}

bool DllController::press_esc()
{
    if (!m_handle) return false;
    return m_loader.press_esc(m_handle);
}

ControlFeat::Feat DllController::support_features() const noexcept
{
    if (!m_handle) return ControlFeat::NONE;
    return m_loader.support_features(m_handle);
}

std::pair<int, int> DllController::get_screen_res() const noexcept
{
    return m_screen_size;
}

void DllController::back_to_home() noexcept
{
    if (m_handle) {
        m_loader.back_to_home(m_handle);
    }
}

void DllController::callback(AsstMsg msg, const json::value& details)
{
    if (m_callback) {
        m_callback(msg, details, m_inst);
    }
}

} // namespace asst
