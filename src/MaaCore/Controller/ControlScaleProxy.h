#pragma once

#include <memory>
#include <random>

#include "ControllerAPI.h"

#include "Common/AsstMsg.h"

namespace asst
{
class ControlScaleProxy
{
public:
    using ProxyCallback = std::function<void(const json::object&)>;

public:
    ControlScaleProxy(
        std::shared_ptr<ControllerAPI> controller,
        ControlFeat::Feat initial_features,
        ProxyCallback proxy_callback);
    ~ControlScaleProxy() = default;

    ControlScaleProxy(const ControlScaleProxy&) = delete;
    ControlScaleProxy(ControlScaleProxy&&) = delete;

    bool click(const Point& p);
    bool click(const Rect& rect);

    bool input(const std::string text);

    bool swipe(
        const Point& p1,
        const Point& p2,
        int duration = 0,
        bool extra_swipe = false,
        double slope_in = 1,
        double slope_out = 1,
        bool with_pause = false,
        bool high_resolution_swipe_fix = false);
    bool swipe(
        const Rect& r1,
        const Rect& r2,
        int duration = 0,
        bool extra_swipe = false,
        double slope_in = 1,
        double slope_out = 1,
        bool with_pause = false,
        bool high_resolution_swipe_fix = false);

    bool inject_input_event(InputEvent event);

    std::pair<int, int> get_scale_size() const noexcept;

private:
    Point rand_point_in_rect(const Rect& rect);

    void callback(const json::object& details);

    std::shared_ptr<ControllerAPI> m_controller;
    ControlFeat::Feat m_features = ControlFeat::NONE;
    ProxyCallback m_callback = nullptr;

    std::mt19937 m_rand_engine;

    std::pair<int, int> m_scale_size = { WindowWidthDefault, WindowHeightDefault };
    double m_control_scale = 1.0;
};
}
