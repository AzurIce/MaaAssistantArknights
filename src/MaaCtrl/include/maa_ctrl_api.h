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

/* DLL export macro -------------------------------------------------------- */
#ifdef _WIN32
#ifdef MAA_CTRL_EXPORTS
#define MAA_CTRL_API __declspec(dllexport)
#else
#define MAA_CTRL_API __declspec(dllimport)
#endif
#else
#define MAA_CTRL_API
#endif

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
MAA_CTRL_API MaaCtrlHandle* maa_ctrl_create(const char* adb_path,
                               const char* address,
                               const char* config_json);
MAA_CTRL_API void maa_ctrl_destroy(MaaCtrlHandle* handle);

/* --- Status ------------------------------------------------------------- */
MAA_CTRL_API bool        maa_ctrl_inited(const MaaCtrlHandle* handle);
MAA_CTRL_API const char* maa_ctrl_get_uuid(const MaaCtrlHandle* handle);
MAA_CTRL_API bool        maa_ctrl_get_screen_res(const MaaCtrlHandle* handle,
                                    int32_t* w, int32_t* h);
MAA_CTRL_API int64_t     maa_ctrl_support_features(const MaaCtrlHandle* handle);
MAA_CTRL_API uint64_t    maa_ctrl_get_pipe_data_size(const MaaCtrlHandle* handle);
MAA_CTRL_API uint64_t    maa_ctrl_get_version(const MaaCtrlHandle* handle);

/* --- Screenshot (two-step: capture then read) --------------------------- */
MAA_CTRL_API bool maa_ctrl_screencap(MaaCtrlHandle* handle);
MAA_CTRL_API bool maa_ctrl_get_image(const MaaCtrlHandle* handle,
                        uint32_t* w, uint32_t* h,
                        const uint8_t** data, uint32_t* len);

/* --- Input -------------------------------------------------------------- */
MAA_CTRL_API bool maa_ctrl_click(MaaCtrlHandle* handle, int32_t x, int32_t y);
MAA_CTRL_API bool maa_ctrl_swipe(MaaCtrlHandle* handle,
                    int32_t x1, int32_t y1,
                    int32_t x2, int32_t y2,
                    int32_t duration_ms,
                    double slope_in, double slope_out,
                    bool with_pause);
MAA_CTRL_API bool maa_ctrl_press_esc(MaaCtrlHandle* handle);
MAA_CTRL_API bool maa_ctrl_input_text(MaaCtrlHandle* handle, const char* text);

/* --- Multi-touch / raw input -------------------------------------------- */
MAA_CTRL_API bool maa_ctrl_touch_down(MaaCtrlHandle* handle, int32_t contact,
                         int32_t x, int32_t y, int32_t pressure);
MAA_CTRL_API bool maa_ctrl_touch_move(MaaCtrlHandle* handle, int32_t contact,
                         int32_t x, int32_t y, int32_t pressure);
MAA_CTRL_API bool maa_ctrl_touch_up(MaaCtrlHandle* handle, int32_t contact);
MAA_CTRL_API bool maa_ctrl_touch_commit(MaaCtrlHandle* handle);
MAA_CTRL_API bool maa_ctrl_key_down(MaaCtrlHandle* handle, int32_t keycode);
MAA_CTRL_API bool maa_ctrl_key_up(MaaCtrlHandle* handle, int32_t keycode);

/* --- Game control ------------------------------------------------------- */
MAA_CTRL_API bool maa_ctrl_start_game(MaaCtrlHandle* handle, const char* package_name);
MAA_CTRL_API bool maa_ctrl_stop_game(MaaCtrlHandle* handle, const char* package_name);

/* --- Misc --------------------------------------------------------------- */
MAA_CTRL_API bool maa_ctrl_back_to_home(MaaCtrlHandle* handle);
MAA_CTRL_API void maa_ctrl_set_kill_adb_on_exit(MaaCtrlHandle* handle, bool enable);
MAA_CTRL_API void maa_ctrl_set_swipe_with_pause(MaaCtrlHandle* handle, bool enable);

/* --- Error & version ---------------------------------------------------- */
MAA_CTRL_API const char* maa_ctrl_last_error(const MaaCtrlHandle* handle);
MAA_CTRL_API const char* maa_ctrl_version(void);

#ifdef __cplusplus
} /* extern "C" */
#else
}
#endif

/* --- Controller Enumeration (MaaCore only, not per-controller) ------------ */

/* Controller information structure */
typedef struct {
    const char* name;              /* "minitouch", "adb", etc. */
    const char* display_name;      /* "Minitouch", "ADB Input", etc. */
    int64_t supported_features;    /* Feature flags (MAA_CTRL_FEAT_*) */
    const char* description;       /* "Minitouch (Default)", etc. */
} MaaCtrlInfo;

/* Enumerate all available controllers.
 * Returns the count of available controllers.
 * out_infos: pointer to MaaCtrlInfo array (caller must free with maa_ctrl_enum_free)
 */
typedef int (*MaaCtrlEnumerateFunc)(MaaCtrlInfo** out_infos);

/* Free memory allocated by maa_ctrl_enumerate */
typedef void (*MaaCtrlEnumFreeFunc)(MaaCtrlInfo* infos, int count);

/* These functions are exported from MaaCore.dll, not from controller DLLs */
#ifdef __cplusplus
extern "C"
{
#endif

MAA_CTRL_API int maa_ctrl_enumerate(MaaCtrlInfo** out_infos);
MAA_CTRL_API void maa_ctrl_enum_free(MaaCtrlInfo* infos, int count);

#ifdef __cplusplus
} /* extern "C" */
#else
}
#endif
