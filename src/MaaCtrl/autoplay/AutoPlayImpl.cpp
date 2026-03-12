#include "AutoPlayController.h"

#include "MaaUtils/NoWarningCV.hpp"

#include "Utils/Logger.hpp"

namespace asst
{

AutoPlayController::AutoPlayController(const AsstCallback& callback, Assistant* inst) :
    InstHelper(inst),
    m_callback(callback)
{
    LogTraceFunction;
}

AutoPlayController::~AutoPlayController()
{
    LogTraceFunction;

    if (m_handle) {
        m_loader.destroy(m_handle);
        m_handle = nullptr;
    }
}

bool AutoPlayController::connect(
    const std::string& adb_path [[maybe_unused]],
    const std::string& address,
    const std::string& config [[maybe_unused]])
{
    LogTraceFunction;

    // Load ap_ffi DLL (searched via standard LoadLibrary path: exe dir, system dirs, etc.)
    if (!m_loader.load("ap_ffi")) {
        Log.error("Failed to load ap_ffi");
        return false;
    }

    // Create Android handle using address as serial
    m_handle = m_loader.create_android(address.c_str());
    if (!m_handle) {
        const char* err = m_loader.last_error(nullptr);
        Log.error("Failed to create auto-play handle for", address, err ? err : "");
        return false;
    }

    if (!m_loader.inited(m_handle)) {
        const char* err = m_loader.last_error(m_handle);
        Log.error("auto-play handle not initialized:", err ? err : "unknown error");
        m_loader.destroy(m_handle);
        m_handle = nullptr;
        return false;
    }

    // Cache UUID
    const char* uuid = m_loader.get_uuid(m_handle);
    m_uuid = uuid ? uuid : "";
    Log.info("auto-play UUID:", m_uuid);

    // Cache screen resolution
    int32_t w = 0, h = 0;
    if (m_loader.get_screen_res(m_handle, &w, &h)) {
        m_screen_size = { static_cast<int>(w), static_cast<int>(h) };
        Log.info("auto-play screen resolution:", w, "x", h);
    }
    else {
        Log.warn("Failed to get screen resolution from auto-play");
    }

    m_inited = true;
    return true;
}

bool AutoPlayController::inited() const noexcept
{
    return m_inited;
}

const std::string& AutoPlayController::get_uuid() const
{
    return m_uuid;
}

bool AutoPlayController::screencap(cv::Mat& image_payload, bool allow_reconnect [[maybe_unused]])
{
    if (!m_handle) return false;

    if (!m_loader.screencap(m_handle)) {
        const char* err = m_loader.last_error(m_handle);
        Log.error("auto-play screencap failed:", err ? err : "unknown");
        return false;
    }

    uint32_t w = 0, h = 0, len = 0;
    const uint8_t* data = nullptr;
    if (!m_loader.get_image(m_handle, &w, &h, &data, &len)) {
        Log.error("auto-play get_image failed");
        return false;
    }

    // RGBA8 → cv::Mat (4 channels), then convert to BGR for MAA
    cv::Mat rgba(static_cast<int>(h), static_cast<int>(w), CV_8UC4, const_cast<uint8_t*>(data));
    cv::cvtColor(rgba, image_payload, cv::COLOR_RGBA2BGR);

    m_screen_size = { static_cast<int>(w), static_cast<int>(h) };
    return true;
}

bool AutoPlayController::start_game(const std::string& client_type)
{
    if (!m_handle || client_type.empty()) return false;
    // Pass client_type directly as intent; ap_ffi handles the mapping
    return m_loader.start_game(m_handle, client_type.c_str());
}

bool AutoPlayController::stop_game(const std::string& client_type)
{
    if (!m_handle || client_type.empty()) return false;
    return m_loader.stop_game(m_handle, client_type.c_str());
}

bool AutoPlayController::click(const Point& p)
{
    if (!m_handle) return false;
    return m_loader.click(m_handle, p.x, p.y);
}

bool AutoPlayController::input(const std::string& text)
{
    if (!m_handle) return false;
    return m_loader.input_text(m_handle, text.c_str());
}

bool AutoPlayController::swipe(
    const Point& p1,
    const Point& p2,
    int duration,
    bool extra_swipe [[maybe_unused]],
    double slope_in,
    double slope_out,
    bool with_pause [[maybe_unused]])
{
    if (!m_handle) return false;
    return m_loader.swipe(m_handle, p1.x, p1.y, p2.x, p2.y, static_cast<int32_t>(duration), slope_in, slope_out);
}

bool AutoPlayController::inject_input_event(const InputEvent& event [[maybe_unused]])
{
    // Not yet implemented (P2)
    return false;
}

bool AutoPlayController::press_esc()
{
    if (!m_handle) return false;
    return m_loader.press_esc(m_handle);
}

ControlFeat::Feat AutoPlayController::support_features() const noexcept
{
    if (!m_handle) return ControlFeat::NONE;
    return m_loader.support_features(m_handle);
}

std::pair<int, int> AutoPlayController::get_screen_res() const noexcept
{
    return m_screen_size;
}

void AutoPlayController::callback(AsstMsg msg, const json::value& details)
{
    if (m_callback) {
        m_callback(msg, details, m_inst);
    }
}

} // namespace asst
