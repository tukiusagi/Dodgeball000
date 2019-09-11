//=============================================================================
//
// サウンド処理 [sound.h]
// Author : YUKI TAKAHASHI
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// サウンドファイル
//=============================================================================
typedef enum
{
	// BGM
	SOUND_LABEL_BGM000 = 0,			// BGM0
	SOUND_LABEL_BGM001,				// BGM1
	SOUND_LABEL_BGM002,				// BGM2
	SOUND_LABEL_BGM003,				// BGM3
	SOUND_LABEL_BGM004,				// BGM4

	// SE
	SOUND_LABEL_SE_DECISION,		// 決定音
	SOUND_LABEL_SE_POWER_UP,		// パワーアップ音
	SOUND_LABEL_SE_SAIROLL,			// サイコロ転がし
	SOUND_LABEL_SE_CAR,				// 車エンジン
	SOUND_LABEL_SE_BANANA,			// バナナつるっ
	SOUND_LABEL_SE_DSAI,			// ダブルサイコロ
	SOUND_LABEL_SE_MINIQ,			// 早押し
	SOUND_LABEL_SE_MINIQB,			// 早押しミス
	SOUND_LABEL_SE_MINIC,			// チキンスタート

	SOUND_LABEL_GAMECLEAR,			// CLEAR音

	SOUND_LABEL_SE_POUSE_0,			// ポーズ選択音
	SOUND_LABEL_SE_POUSE_1,			// ポーズ決定音

	SOUND_LABEL_MAX,				// 音の最大数
} SOUND_LABEL;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
HRESULT InitSound(HWND hWnd);
void UninitSound(void);

HRESULT PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);

#endif
