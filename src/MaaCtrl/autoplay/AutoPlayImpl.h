#pragma once

#include <string>
#include <utility>

#include "AutoPlayLoader.h"
#include "Common/AsstMsg.h"
#include "ControllerAPI.h"
#include "InstHelper.h"
#include "MaaUtils/NoWarningCVMat.hpp"

namespace asst
{
class Assistant;

class AutoPlayController : public ControllerAPI, private InstHelper
{
public:
    AutoPlayController(const AsstCallback& callback, Assistant* inst);
    virtual ~AutoPlayController() override;

    AutoPlayController(const AutoPlayController&) = delete;
    AutoPlayController& operator=(const AutoPlayController&) = delete;
    AutoPlayController(AutoPlayController&&) = delete;
    AutoPlayController& operator=(AutoPlayController&&) = delete;

public: // ControllerAPI interface
    virtual bool connect(const std::string& adb_path, const std::string& address,
                         const std::string& config) override;
    virtual bool inited() const noexcept override;

    virtual const std::string& get_uuid() const override;

    virtual size_t get_pipe_data_size() const noexcept override { return 0; }
    virtual size_t get_version() const noexcept override { return 0; }

    virtual bool screencap(cv::Mat& image_payload, bool allow_reconnect = false) override;

    virtual bool start_game(const std::string& client_type) override;
    virtual bool stop_game(const std::string& client_type) override;

    virtual bool click(const Point& p) override;
    virtual bool input(const std::string& text) override;
    virtual bool swipe(const Point& p1, const Point& p2, int duration = 0, bool extra_swipe = false,
                       double slope_in = 1, double slope_out = 1, bool with_pause = false) override;

    virtual bool inject_input_event(const InputEvent& event) override;

    virtual bool press_esc() override;
    virtual ControlFeat::Feat support_features() const noexcept override;

    virtual std::pair<int, int> get_screen_res() const noexcept override;

private:
    void callback(AsstMsg msg, const json::value& details);

    AsstCallback m_callback = nullptr;
    AutoPlayLoader m_loader;
    void* m_handle = nullptr; // ApHandle* from ap_ffi
    bool m_inited = false;
    std::string m_uuid;
    std::pair<int, int> m_screen_size = { 0, 0 };
};
} // namespace asst
