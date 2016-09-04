
#pragma once

#if SKSE_VERSION_RELEASEIDX >= 40
// RUNTIME_VERSION_1_9_32_0
#define ADDR_OnCameraMove						0x0074F311
#define ADDR_SetAngleZ							0x006A8910
#define ADDR_SetAngleX							0x006AE540
#define ADDR_g_playerDialogueTarget				0x013105D8
#define ADDR_GetTarget              			0x00674450
#elif SKSE_VERSION_RELEASEIDX == 39
// RUNTIME_VERSION_1_9_29_0
#error 対応していないバージョンです。
#elif SKSE_VERSION_RELEASEIDX == 38
#error 対応していないバージョンです。
#elif SKSE_VERSION_RELEASEIDX >= 31
// RUNTIME_VERSION_1_8_151_0
#define ADDR_OnCameraMove						0x0074F461
#define ADDR_SetAngleZ							0x006A84F0
#define ADDR_SetAngleX							0x006AE200
#define ADDR_g_playerDialogueTarget				0x0130F7D8
#define ADDR_GetTarget							0x00674090
#else
#error 対応していないバージョンです。
#endif

