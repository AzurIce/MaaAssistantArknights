// maa-ctrl-mumu: MuMu external renderer controller plugin.
#define MAA_CTRL_PLUGIN_VERSION "maa-ctrl-mumu 1.0.0"
#include "maa_ctrl_controller_plugin.hpp"

#include "MumuController.h"

asst::ControllerAPI* maa_ctrl_plugin_create_controller(const MaaCtrlCreateInfo* info)
{
    try {
        return new asst::MumuController(
            maa_ctrl_plugin::make_callback(info),
            nullptr,
            maa_ctrl_plugin::platform_of(info));
    }
    catch (...) {
        return nullptr;
    }
}
