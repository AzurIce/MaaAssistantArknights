#pragma once

/*
 * maa_ctrl_api.h — Unified C-ABI interface for MaaCore controller DLLs.
 *
 * Every controller implementation (maa-ctrl-adb, maa-ctrl-minitouch, ...)
 * exports this exact set of symbols.  MaaCore loads them at runtime via
 * MaaCtrlLoader.
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Opaque handle --------------------------------------------------------- */
typedef void MaaCtrlHandle;

/* Feature flags returned by maa_ctrl_support_features -------------------- */
#define MAA_CTRL_FEAT_NONE 0
#define MAA_CTRL_FEAT_SWIPE_WITH_PAUSE (1LL << 0)
#define MAA_CTRL_FEAT_PRECISE_SWIPE (1LL << 1)
#define MAA_CTRL_FEAT_IMPRECISE_SWIPE (1LL << 2)

/* --- Lifecycle ---------------------------------------------------------- */

/*  config_json semantics vary per DLL:
 *   - ADB-based DLLs: JSON object with adb command templates
 *   - Win32:          JSON with screencap_method / mouse_method / keyboard_method
 *   - AutoPlay:       unused (may be empty string)
 *   - PlayTools:      screencap method string ("RGBA" / "BGR" / "MacSCK")
 */
MaaCtrlHandle* maa_ctrl_create(const char* adb_path,
                               const char* address,
                               const char* config_json);
void maa_ctrl_destroy(MaaCtrlHandle* handle);

/* --- Status ------------------------------------------------------------- */
bool        maa_ctrl_inited(const MaaCtrlHandle* handle);
const char* maa_ctrl_get_uuid(const MaaCtrlHandle* handle);
bool        maa_ctrl_get_screen_res(const MaaCtrlHandle* handle,
                                    int32_t* w, int32_t* h);
int64_t     maa_ctrl_support_features(const MaaCtrlHandle* handle);
uint64_t    maa_ctrl_get_pipe_data_size(const MaaCtrlHandle* handle);
uint64_t    maa_ctrl_get_version(const MaaCtrlHandle* handle);

/* --- Screenshot (two-step: capture then read) --------------------------- */
bool maa_ctrl_screencap(MaaCtrlHandle* handle);
bool maa_ctrl_get_image(const MaaCtrlHandle* handle,
                        uint32_t* w, uint32_t* h,
                        const uint8_t** data, uint32_t* len);

/* --- Input -------------------------------------------------------------- */
bool maa_ctrl_click(MaaCtrlHandle* handle, int32_t x, int32_t y);
bool maa_ctrl_swipe(MaaCtrlHandle* handle,
                    int32_t x1, int32_t y1,
                    int32_t x2, int32_t y2,
                    int32_t duration_ms,
                    double slope_in, double slope_out,
                    bool with_pause);
bool maa_ctrl_press_esc(MaaCtrlHandle* handle);
bool maa_ctrl_input_text(MaaCtrlHandle* handle, const char* text);

/* --- Multi-touch / raw input -------------------------------------------- */
bool maa_ctrl_touch_down(MaaCtrlHandle* handle, int32_t contact,
                         int32_t x, int32_t y, int32_t pressure);
bool maa_ctrl_touch_move(MaaCtrlHandle* handle, int32_t contact,
                         int32_t x, int32_t y, int32_t pressure);
bool maa_ctrl_touch_up(MaaCtrlHandle* handle, int32_t contact);
bool maa_ctrl_touch_commit(MaaCtrlHandle* handle);
bool maa_ctrl_key_down(MaaCtrlHandle* handle, int32_t keycode);
bool maa_ctrl_key_up(MaaCtrlHandle* handle, int32_t keycode);

/* --- Game control ------------------------------------------------------- */
bool maa_ctrl_start_game(MaaCtrlHandle* handle, const char* package_name);
bool maa_ctrl_stop_game(MaaCtrlHandle* handle, const char* package_name);

/* --- Misc --------------------------------------------------------------- */
bool maa_ctrl_back_to_home(MaaCtrlHandle* handle);
void maa_ctrl_set_kill_adb_on_exit(MaaCtrlHandle* handle, bool enable);
void maa_ctrl_set_swipe_with_pause(MaaCtrlHandle* handle, bool enable);

/* --- Error & version ---------------------------------------------------- */
const char* maa_ctrl_last_error(const MaaCtrlHandle* handle);
const char* maa_ctrl_version(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
