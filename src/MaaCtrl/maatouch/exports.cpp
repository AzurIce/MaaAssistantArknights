// maa-ctrl-maatouch: MaaTouch controller plugin.
#define MAA_CTRL_PLUGIN_VERSION "maa-ctrl-maatouch 1.0.0"
#include "maa_ctrl_controller_plugin.hpp"

#include "MaatouchController.h"

asst::ControllerAPI* maa_ctrl_plugin_create_controller(const MaaCtrlCreateInfo* info)
{
    try {
        return new asst::MaatouchController(
            maa_ctrl_plugin::make_callback(info),
            nullptr,
            maa_ctrl_plugin::platform_of(info));
    }
    catch (...) {
        return nullptr;
    }
}
