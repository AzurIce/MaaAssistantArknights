#include "ControllerRegistry.h"

#include <filesystem>
#include <algorithm>

#include "Utils/Logger.hpp"

// Define MAA_CTRL_EXPORTS so that maa_ctrl_api.h generates dllexport for these functions
#define MAA_CTRL_EXPORTS
#include "maa_ctrl_api.h"

namespace asst
{

const std::vector<ControllerInfo> ControllerRegistry::GetKnownControllers()
{
    return {
        {
            .name = "minitouch",
            .display_name = "Minitouch",
            .supported_features = MAA_CTRL_FEAT_PRECISE_SWIPE,
            .description = "Minitouch (Default)"
        },
        {
            .name = "maatouch",
            .display_name = "MaaTouch",
            .supported_features = MAA_CTRL_FEAT_SWIPE_WITH_PAUSE | MAA_CTRL_FEAT_PRECISE_SWIPE,
            .description = "MaaTouch (Experimental)"
        },
        {
            .name = "adb",
            .display_name = "ADB Input",
            .supported_features = MAA_CTRL_FEAT_IMPRECISE_SWIPE,
            .description = "ADB Input (Deprecated)"
        },
        {
            .name = "playtools",
            .display_name = "PlayTools",
            .supported_features = 0,
            .description = "PlayTools (macOS/iOS)"
        },
        {
            .name = "autoplay",
            .display_name = "AutoPlay",
            .supported_features = MAA_CTRL_FEAT_PRECISE_SWIPE,
            .description = "AutoPlay Framework"
        },
        {
            .name = "win32",
            .display_name = "Win32",
            .supported_features = MAA_CTRL_FEAT_PRECISE_SWIPE,
            .description = "Win32 Native Window"
        },
    };
}

std::string ControllerRegistry::GetControllerDllDirectory()
{
    // Get the directory of the currently running executable
    // This is where the controller DLLs should be located
    std::filesystem::path exe_path = std::filesystem::current_path();
    return exe_path.string();
}

bool ControllerRegistry::IsControllerAvailable(const std::string& name)
{
    std::string dll_dir = GetControllerDllDirectory();
    std::string dll_name = "maa-ctrl-" + name + ".dll";

#ifdef _WIN32
    std::filesystem::path dll_path = std::filesystem::path(dll_dir) / dll_name;
#else
    std::string so_name = "maa-ctrl-" + name + ".so";
    std::filesystem::path dll_path = std::filesystem::path(dll_dir) / so_name;
#endif

    return std::filesystem::exists(dll_path);
}

std::vector<ControllerInfo> ControllerRegistry::GetAvailableControllers()
{
    std::vector<ControllerInfo> result;
    const auto& known = GetKnownControllers();

    for (const auto& ctrl : known) {
        if (IsControllerAvailable(ctrl.name)) {
            result.push_back(ctrl);
            Log.info("ControllerRegistry: Found controller DLL:", "maa-ctrl-" + ctrl.name);
        } else {
            Log.debug("ControllerRegistry: Controller DLL not found:", "maa-ctrl-" + ctrl.name);
        }
    }

    if (result.empty()) {
        Log.warn("ControllerRegistry: No controller DLLs found, returning all known controllers");
        return known;
    }

    return result;
}

ControllerInfo ControllerRegistry::GetControllerInfo(const std::string& name)
{
    auto controllers = GetAvailableControllers();
    auto it = std::find_if(controllers.begin(), controllers.end(),
                          [&name](const ControllerInfo& ctrl) { return ctrl.name == name; });

    if (it != controllers.end()) {
        return *it;
    }

    // Fallback to default
    Log.warn("ControllerRegistry: Controller not found:", name, "using default minitouch");
    return {
        .name = "minitouch",
        .display_name = "Minitouch",
        .supported_features = MAA_CTRL_FEAT_PRECISE_SWIPE,
        .description = "Minitouch (Default)"
    };
}

} // namespace asst

/* C-ABI Enumeration Exports - these are exported from MaaCore.dll */

extern "C"
{

int maa_ctrl_enumerate(MaaCtrlInfo** out_infos)
{
    if (!out_infos) {
        return 0;
    }

    auto controllers = asst::ControllerRegistry::GetAvailableControllers();
    int count = static_cast<int>(controllers.size());

    if (count == 0) {
        *out_infos = nullptr;
        return 0;
    }

    // Allocate memory for controller info array
    auto* infos = new MaaCtrlInfo[count];

    for (int i = 0; i < count; ++i) {
        const auto& ctrl = controllers[i];
        size_t name_len = ctrl.name.length() + 1;
        size_t display_len = ctrl.display_name.length() + 1;
        size_t desc_len = ctrl.description.length() + 1;

        infos[i].name = new char[name_len];
        infos[i].display_name = new char[display_len];
        infos[i].description = new char[desc_len];
        infos[i].supported_features = ctrl.supported_features;

        memcpy(const_cast<char*>(infos[i].name), ctrl.name.c_str(), name_len);
        memcpy(const_cast<char*>(infos[i].display_name), ctrl.display_name.c_str(), display_len);
        memcpy(const_cast<char*>(infos[i].description), ctrl.description.c_str(), desc_len);
    }

    *out_infos = infos;
    return count;
}

void maa_ctrl_enum_free(MaaCtrlInfo* infos, int count)
{
    if (!infos || count <= 0) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        delete[] const_cast<char*>(infos[i].name);
        delete[] const_cast<char*>(infos[i].display_name);
        delete[] const_cast<char*>(infos[i].description);
    }

    delete[] infos;
}

} // extern "C"
