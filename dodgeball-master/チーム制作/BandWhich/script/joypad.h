//=============================================================================
//
// ジョイパッド処理 [joypad.h]
// Author : YUKI TAKAHASHI
//
//=============================================================================
#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#define _CRT_SECURE_NO_WARNINGS

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define NUM_JOYPAD_MAX		(4)		// パッド最大数

//=============================================================================
// キーの種類
//=============================================================================
typedef enum
{
	JOYPADKEY_X = 0,			// [0] Ｘボタン
	JOYPADKEY_Y,				// [1] Ｙボタン
	JOYPADKEY_A,				// [2] Ａボタン
	JOYPADKEY_B,				// [3] Ｂボタン
	JOYPADKEY_LEFT_SHOULDER,	// [4] 左手前ボタン
	JOYPADKEY_RIGHT_SHOULDER,	// [5] 右手前ボタン
	JOYPADKEY_LEFT_TRIGGER,		// [6] 左トリガー
	JOYPADKEY_RIGHT_TRIGGER,	// [7] 右トリガー
	JOYPADKEY_LEFT_THUMB,		// [8] 左スティック
	JOYPADKEY_RIGHT_THUMB,		// [9] 右スティック
	JOYPADKEY_BACK,				// [10] バックボタン
	JOYPADKEY_START,			// [11] スタートボタン
	JOYPADKEY_LEFT,				// [12] 方向キー[左]
	JOYPADKEY_RIGHT,			// [13] 方向キー[右]
	JOYPADKEY_UP,				// [14] 方向キー[上]
	JOYPADKEY_DOWN,				// [15] 方向キー[下]
	JOYPADKEY_MAX				// [16] キーの種類数
} JOYPADKEY;

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	bool UpThumb;	// 左スティック上
	bool DownThumb;	// 左スティック下
} JOYPADUSE;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
HRESULT InitJoypad(HINSTANCE hInstance, HWND hWnd);
void UninitJoypad(void);
void UpdateJoypad(void);

bool GetJoypadPress(int nIDPad, JOYPADKEY key);
bool GetJoypadTrigger(int nIDPad, JOYPADKEY key);
bool GetJoypadRelease(int nIDPad, JOYPADKEY key);
bool GetJoypadRepeat(int nIDPad, JOYPADKEY key);

int GetJoypadTriggerLeft(int nIDPad);
int GetJoypadTriggerRight(int nIDPad);

void GetJoypadStickLeft(int nIDPad, int *pValueH, int *pValueV);
void GetJoypadStickRight(int nIDPad, int *pValueH, int *pValueV);

static BOOL CALLBACK EnumJoyCallbackJoypad(const DIDEVICEINSTANCE* lpddi, VOID* pvRef);
static BOOL CALLBACK EnumAxesCallbackJoypad(const LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);
void SetKeyStateJoypad(int nIDPad);

void SetNumPad(int nNum);	// プレイヤー数の設定
int GetNumPad(void);		// プレイヤー数の取得

#endif