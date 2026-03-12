#include "StartGameTaskPlugin.h"

#include "Controller/Controller.h"

using namespace asst;

bool StartGameTaskPlugin::start_game_with_retries(size_t pipe_data_size_limit, bool newer_android) const
{
    int extra_runs = 0;
    for (int i = 0; i < 30; ++i) {
        if (need_exit() || !ctrler()->start_game(m_client_type)) {
            return false;
        }

        if (ctrler()->get_pipe_data_size() > pipe_data_size_limit) {
            if (newer_android || ++extra_runs > 3) {
                return true;
            }
        }

        sleep(1500);
    }

    return false;
}

bool StartGameTaskPlugin::_run()
{
    if (m_client_type.empty()) {
        return false;
    }

    // PlayTools (macOS/iOS) and AutoPlay handle start_game directly via DLL,
    // no pipe_data_size check needed.  For these controllers, get_pipe_data_size
    // returns 0 and get_version returns 0, so the generic path would fail.
    // Just call start_game directly if pipe_data_size is not meaningful.
    if (ctrler()->get_pipe_data_size() == 0 && ctrler()->get_version() == 0) {
        return ctrler()->start_game(m_client_type);
    }

    // check for android version, because it leads to different magic values
    // >8:  167: magic value needs to be >164 but <172 (as that's max)
    // <=8: 145: needs to be >85 but <153 (as that's max)
    // https://github.com/MaaAssistantArknights/MaaAssistantArknights/pull/8966#issuecomment-2094369694
    // https://github.com/MaaAssistantArknights/MaaAssistantArknights/pull/8961#issue-2277568882
    int pipe_data_size_limit = (ctrler()->get_version() > 8) ? 167 : 145;

    // In Android 9 and above, there's a way to check if the game started correctly
    // so no need for limited tries
    bool newer_android = (ctrler()->get_version() > 8);

    return start_game_with_retries(pipe_data_size_limit, newer_android);
}

StartGameTaskPlugin& StartGameTaskPlugin::set_client_type(std::string client_type) noexcept
{
    m_client_type = std::move(client_type);
    return *this;
}
