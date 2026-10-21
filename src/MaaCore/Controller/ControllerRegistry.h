#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace asst
{

struct ControllerInfo
{
    std::string name;              // "minitouch"
    std::string display_name;      // "Minitouch"
    int64_t supported_features;    // Feature flags
    std::string description;       // "Minitouch (Default)"
};

class ControllerRegistry
{
public:
    // Get all available controllers (checks if DLL exists)
    static std::vector<ControllerInfo> GetAvailableControllers();

    // Get info for a specific controller
    static ControllerInfo GetControllerInfo(const std::string& name);

private:
    // List of all known controllers
    static const std::vector<ControllerInfo> GetKnownControllers();

    // Get the directory where controller DLLs should be located
    static std::string GetControllerDllDirectory();

    // Check if a controller DLL exists
    static bool IsControllerAvailable(const std::string& name);
};

} // namespace asst
