// maa-ctrl-maafw-android: MaaFramework Android native controller plugin.
#define MAA_CTRL_PLUGIN_VERSION "maa-ctrl-maafw-android 1.0.0"
#include "maa_ctrl_controller_plugin.hpp"

#include "MaaFwAndroidNativeController.h"

asst::ControllerAPI* maa_ctrl_plugin_create_controller(const MaaCtrlCreateInfo* info)
{
    try {
        return new asst::MaaFwAndroidNativeController(maa_ctrl_plugin::make_callback(info), nullptr);
    }
    catch (...) {
        return nullptr;
    }
}
