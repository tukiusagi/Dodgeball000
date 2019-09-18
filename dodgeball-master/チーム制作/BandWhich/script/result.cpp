//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : DAIKI TSUCHIDA
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "result.h"
#include "camera.h"
#include "fade.h"
#include "game.h"
#include "joypad.h"
#include "keyboard.h"
#include "meshField.h"
#include "meshWall.h"
#include "sound.h"
#include "player.h"
#include "title.h"
#include"logo.h"

//=============================================================================
// リザルトの種類と状態
//=============================================================================
typedef enum
{
	RTYPE_FIRST = 0,	// 一位
	RTYPE_SECOND,		// 二位
	RTYPE_THIRD,		// 三位
	RTYPE_FOUR,			// 四位
	RTYPE_RESULT,		// 結果発表
	RTYPE_MAX
} RTYPE;

typedef enum
{
	RSTATE_NONE = 0,	// 何もしていない
	RSTATE_NORMAL,		// 通常状態
	RSTATE_INSCORE,		// スコア表示
	RSTATE_NEXT = 0,	// 次の画面へ
	RSTATE_MAX
} RSTATE;

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 move;
	D3DXCOLOR	col;
	float		fWidht;
	float		fHeight;
	int			nState;
	int			nType;
	bool		bUse;
} RESULT;

//=============================================================================
// マクロ定義
//=============================================================================
#define TEXTUREFILE1		"data/TEXTURE/Gold.png"				// 読み込むテクスチャ1
#define TEXTUREFILE2		"data/TEXTURE/Silver.png"			// 読み込むテクスチャ2
#define TEXTUREFILE3		"data/TEXTURE/Copper.png"			// 読み込むテクスチャ3
#define TEXTUREFILE4		"data/TEXTURE/black.png"			// 読み込むテクスチャ4
#define TEXTUREFILE5		"data/TEXTURE/結果発表.png"			// 読み込むテクスチャ5
#define MAX_RTEXTURE		(5)									// 使用するテクスチャ数
#define R_POS_X				(1280.0f)							// テクスチャのX座標
#define R_POS_Y				(120.0f)							// テクスチャのY座標
#define R_WIDTH				(1280.0f)							// テクスチャの幅
#define R_HEIGHT			(600.0f)							// テクスチャの高さ
#define MAX_NUMPLAYER		(4)									// 参加人数

//=============================================================================
// グローバル変数
//=============================================================================
int	g_nCntR;						// 画面遷移カウント

//=============================================================================
// リザルト画面の初期化
//=============================================================================
void InitResult()
{
	// 初期化
	g_nCntR = 1800;

	// メッシュフィールドの初期化処理
	InitMeshField();

	// メッシュウォールの初期化処理
	InitMeshWall();

	//勝利ロゴの初期化
	InitLogo();

	int nID = 0;		//生き残ったプレイヤーの番号
	for (int nCnt = 0; nCnt < MAX_NUMPLAYER; nCnt++)
	{
		if (GetPlayerUse(nCnt))
		{	//何番が生き残った？
			nID = nCnt;
		}
	}
	SetLogo((LOGOTYPE)nID);
}

//=============================================================================
// リザルト画面の終了処理
//=============================================================================
void UninitResult()
{
	// プレイヤーの終了処理
	UninitPlayer();

	// メッシュフィールドの終了処理
	UninitMeshField();

	// メッシュウォールの終了処理
	UninitMeshWall();

	//ロゴの終了
	UninitLogo();
}

//=============================================================================
// リザルト画面の更新処理
//=============================================================================
void UpdateResult()
{
	// メッシュフィールドの更新処理
	UpdateMeshField();

	// メッシュウォールの更新処理
	UpdateMeshWall();

	// プレイヤーの更新処理
	UpdatePlayer();

	//ロゴの更新
	UpdateLogo();

	g_nCntR--;		// カウンターを進める

	if (GetFade() != FADE_OUT)
	{
		if (g_nCntR <= 0)
		{
			//モードの設定
			SetFade(MODE_TITLE);
		}
		else if (GetKeyboardTrigger(DIK_SPACE) || GetKeyboardTrigger(DIK_RETURN) ||
			GetJoypadTrigger(0, JOYPADKEY_START) || GetJoypadTrigger(0, JOYPADKEY_A))
		{
			// モードの設定
			SetFade(MODE_TITLE);
		}
	}
}

//=============================================================================
// リザルト画面の描画処理
//=============================================================================
void DrawResult()
{
	// メッシュフィールドの描画処理
	DrawMeshField();

	// メッシュウォールの描画処理
	DrawMeshWall();

	// プレイヤーの描画処理
	DrawPlayer();

	//ロゴの描画
	DrawLogo();
}