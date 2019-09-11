//=============================================================================
//
// キーボード処理 [keyboard.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "keyboard.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define	NUM_KEY_MAX			(256)	// キーの最大数

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECTINPUT8 g_pKeyboard = NULL;			// DirectInputオブジェクトへのポインタ
LPDIRECTINPUTDEVICE8 g_pDevKeyboard = NULL;	// 入力デバイス(キーボード)へのポインタ

BYTE g_aKeyState[NUM_KEY_MAX];				// キーボードの入力情報ワーク
BYTE g_aKeyStateTrigger[NUM_KEY_MAX];		// トリガー
BYTE g_aKeyStateRelease[NUM_KEY_MAX];		// リリース
BYTE g_aKeyStateRepeat[NUM_KEY_MAX];		// リピート

//=============================================================================
// キーボードの初期化
//=============================================================================
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	// DirectInputオブジェクトの作成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pKeyboard,NULL)))
	{
		return E_FAIL;
	}
	
	// 入力デバイス（キーボード）の作成
	if (FAILED(g_pKeyboard->CreateDevice(GUID_SysKeyboard,&g_pDevKeyboard,NULL)))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(g_pDevKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	if (FAILED(g_pDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	g_pDevKeyboard->Acquire();

	return S_OK;
}

//=============================================================================
// キーボードの終了処理
//=============================================================================
void UninitKeyboard(void)
{
	// 入力デバイス(キーボード)の開放
	if (g_pDevKeyboard != NULL)
	{
		g_pDevKeyboard->Unacquire();
		g_pDevKeyboard->Release();
		g_pDevKeyboard = NULL;
	}

	// DirectInputオブジェクトの開放
	if (g_pKeyboard != NULL)
	{
		g_pKeyboard->Release();
		g_pKeyboard = NULL;
	}
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
void UpdateKeyboard(void)
{
	BYTE aKeyState[NUM_KEY_MAX];
	int nCntKey;

	// デバイスからデータを取得
	if (SUCCEEDED(g_pDevKeyboard->GetDeviceState(sizeof(aKeyState), aKeyState)))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			// トリガー処理
			g_aKeyStateTrigger[nCntKey] = (g_aKeyState[nCntKey] ^ aKeyState[nCntKey])&aKeyState[nCntKey];

			// リリース処理
			g_aKeyStateRelease[nCntKey] = (g_aKeyState[nCntKey] ^ aKeyState[nCntKey])&g_aKeyState[nCntKey];

			// リピート処理
			//

			g_aKeyState[nCntKey] = aKeyState[nCntKey];// キープレス情報保存
		}
	}
	else
	{
		g_pDevKeyboard->Acquire();// キーボードへのアクセス権を獲得
	}
}

//=============================================================================
// キーボードを押したときの処理＿プレス
//=============================================================================
bool GetKeyboardTrigger(int nKey)
{
	return(g_aKeyStateTrigger[nKey]) ? true : false;
}

//=============================================================================
// キーボードを離したときの処理＿リリース
//=============================================================================
bool GetKeyboardRelease(int nKey)
{
	return(g_aKeyStateRelease[nKey]) ? true : false;
}

//=============================================================================
// キーボードを長押ししたときの処理＿リピート
//=============================================================================
bool GetKeyboardRepeat(int nKey)
{
	return(g_aKeyStateRepeat[nKey]) ? true : false;
}

//=============================================================================
// キーボードを長押ししたときの処理＿ロングプレス
//=============================================================================
bool GetKeyboardPress(int nKey)
{
	return(g_aKeyState[nKey] & 0x80) ? true : false;
}

