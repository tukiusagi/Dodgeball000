//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "game.h"
#include "camera.h"
#include "effect.h"
#include "fade.h"
#include "joypad.h"
#include "keyboard.h"
#include "meshField.h"
#include "meshWall.h"
#include "model.h"
#include "pause.h"
#include "player.h"
#include "shadow.h"
#include "sound.h"
#include "title.h"
#include "time.h"
#include "ball.h"
#include "field.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_TEXTURE		(6)								// テクスチャ数
#define SELECT_POS_X	(165)							// ポーズの枠のX座標
#define ARROW_POS_X		(25)							// 矢印のX座標
#define DICE_POS_Y		(120)							// サイコロのY座標
#define ITEM_POS_Y		(180)							// アイテムのY座標
#define MAP_POS_Y		(240)							// マップのY座標
#define CONTINUE_POS_Y	(300)							// コンテニューのY座標
#define RETEY_POS_Y		(400)							// リトライのY座標
#define QUIT_POS_Y		(500)							// 終了のY座標
#define SELECT_WIDTH	(330)							// セレクトの幅
#define SELECT_HEIGHT	(50)							// セレクトの高さ
#define ARROW_WIDTH		(46)							// 矢印の幅
#define ARROW_HEIGHT	(46)							// 矢印の高さ
#define ITEM1_POS_X		(500)							// アイテムのX座標
#define ITEM1_POS_Y		(300)							// アイテムのY座標
#define ITEM1_WIDTH		(100)							// アイテムの幅
#define ITEM1_HEIGHT	(100)							// アイテムの高さ
#define ITEM2_POS_X		(800)							// アイテムのX座標
#define ITEM2_POS_Y		(300)							// アイテムのY座標
#define ITEM2_WIDTH		(100)							// アイテムの幅
#define ITEM2_HEIGHT	(100)							// アイテムの高さ
#define MAX_SELECT		(4)								// 選択テクスチャ数
#define SELECT_MAX		(3)								// 選択肢の数
#define MAX_PALYER		(4)	//プレイヤー数

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void SetChangePlayer(void);

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct// ゲーム
{
	JOYPADUSE				g_PadUse;							// ジョイパッドの使用状態の情報
	GAMESTATE				state = GAMESTATE_NONE;				// ゲーム状態
	int						nCounterState = 0;					// ゲーム状態カウンター
	int						nNowPlayer;							// 現在のプレイヤー番号
} GAME;

//=============================================================================
// グローバル変数宣言
//=============================================================================
GAME					g_game;								// ゲームの情報

//=============================================================================
// ゲーム画面の初期化
//=============================================================================
void InitGame(HWND hWnd)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();// デバイスの取得

	// 初期化
	g_game.nCounterState = 0;

	if (g_game.state == GAMESTATE_NONE)
	{// 最初のみ初期化する
		g_game.nNowPlayer = 0;		// 1Pから開始する
	}
	else
	{
		SetChangePlayer();		// 操作プレイヤーの変更
	}

	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化処理
	InitPlayer();

	SetPlayerPos(D3DXVECTOR3(-200.0f, 0.0f, 200.0f), 0);
	SetPlayerPos(D3DXVECTOR3(200.0f, 0.0f, 200.0f), 1);
	SetPlayerPos(D3DXVECTOR3(-200.0f, 0.0f, -200.0f), 2);
	SetPlayerPos(D3DXVECTOR3(200.0f, 0.0f, -200.0f), 3);

	// カメラの初期化処理
	InitCamera();

	// メッシュフィールドの初期化処理
	InitMeshField();

	// メッシュウォールの初期化処理
	InitMeshWall();

	// ポーズ画面初期化処理
	InitPause();

	// モデルの初期化処理
	InitModel();

	//ボールの初期化
	InitBall();

	// エフェクトの初期化処理
	InitEffect();

	// フィールドの初期化
	InitField();

	//カウントダウンの初期化
	InitTime();

	g_game.state = GAMESTATE_NORMAL;// スタート状態に設定
}

//=============================================================================
// ゲーム画面の終了処理
//=============================================================================
void UninitGame()
{
	// ポーズ画面終了処理
	UninitPause();

	// プレイヤーの終了処理
	//UninitPlayer();

	//ボールの終了
	UninitBall();

	// メッシュフィールドの終了処理
	UninitMeshField();

	// メッシュウォールの終了処理
	UninitMeshWall();

	//カウントダウンの終了
	UninitTime();

	// モデルの終了処理
	UninitModel();

	// 影の終了処理
	UninitShadow();

	// エフェクトの終了処理
	UninitEffect();

	// フィールドの終了
	UninitField();
}

//=============================================================================
// ゲーム画面の更新処理
//=============================================================================
void UpdateGame()
{
	ARROW *pArrow = GetArrow();									// ポーズの矢印情報取得
	D3DXVECTOR3 pPlayerPos = *GetPlayerPos(g_game.nNowPlayer);	// プレイヤーの位置
	int nStickH, nStickV;										// アナログスティック左の入力情報

	// アナログスティック左の取得
	GetJoypadStickLeft(g_game.nNowPlayer, &nStickH, &nStickV);

	if (g_game.state != GAMESTATE_PAUSE)
	{// ポーズ状態じゃないとき
		// カメラの更新処理
		UpdateCamera();

		// プレイヤーの更新処理
		UpdatePlayer();

		//ボールの更新
		UpdateBall();

		// メッシュフィールドの更新処理
		UpdateMeshField();

		// メッシュウォールの更新処理
		UpdateMeshWall();

		// モデルの更新処理
		UpdateModel();

		// 影の更新処理
		UpdateShadow();

		//カウントダウンの更新
		UpdateTime();

		// エフェクトの更新処理
		UpdateEffect();

		// フィールドの更新
		UpdateField();
	}

	int nNumPlayer = 0;	//生きているプレイヤー数
	int nID = 0;		//生き残ったプレイヤーの番号
	switch (g_game.state)
	{
	case GAMESTATE_NORMAL:// 通常状態

		g_game.nCounterState++;

		for (int nCnt = 0; nCnt < MAX_PALYER; nCnt++)
		{	
			if (GetPlayerUse(nCnt))
			{
				nNumPlayer++;	//プレイヤー数カウント
				nID = nCnt;
			}
		}
		if (nNumPlayer == 1)
		{	//最後の一人になったら終了
			SetGameState(GAMESTATE_END);
		}

		if (GetKeyboardTrigger(DIK_P) || GetJoypadTrigger(g_game.nNowPlayer, JOYPADKEY_START))
		{// ポーズを押した場合
			g_game.state = GAMESTATE_PAUSE;
		}
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_game.state = GAMESTATE_END;
		}
		if (g_game.nCounterState >= 60)
		{
			//if (GetKeyboardTrigger(DIK_SPACE) || GetKeyboardTrigger(DIK_RETURN) || GetJoypadTrigger(g_game.nNowPlayer, JOYPADKEY_A))
			//{
			//	PlaySound(SOUND_LABEL_SE_POUSE_1);// 効果音の設定
			//	g_game.state = GAMESTATE_END;
			//	g_game.nCounterState = 0;
			//}
		}

		break;

	case GAMESTATE_END:

		if (g_game.nCounterState == 0)
		{
			StopSound();
			PlaySound(SOUND_LABEL_GAMECLEAR);
		}

		g_game.nCounterState++;

		if (GetFade() != FADE_OUT)
		{
			if (g_game.nCounterState >= 300)
			{
				// 画面モードの設定
				SetFade(MODE_RESULT);
				g_game.state = GAMESTATE_NONE;
			}
		}
		break;

	case GAMESTATE_PAUSE:

		// ポーズの更新処理
		UpdatePause();

		if (GetFade() != FADE_OUT)
		{
			if (GetKeyboardTrigger(DIK_P) || GetJoypadTrigger(g_game.nNowPlayer, JOYPADKEY_START) || GetJoypadTrigger(g_game.nNowPlayer, JOYPADKEY_B))
			{
				PlaySound(SOUND_LABEL_SE_POUSE_1);// 効果音の設定

				g_game.state = GAMESTATE_NORMAL;
			}

			if (GetKeyboardTrigger(DIK_SPACE) || GetKeyboardTrigger(DIK_RETURN) || GetJoypadTrigger(g_game.nNowPlayer, JOYPADKEY_A))
			{
				PlaySound(SOUND_LABEL_SE_POUSE_1);// 効果音の設定

				if (pArrow->pos.y == CONTINUE_POS_Y)
				{
					g_game.state = GAMESTATE_NORMAL;
				}
				else if (pArrow->pos.y == RETEY_POS_Y)
				{
					SetFade(MODE_GAME);// 画面モードの設定
					g_game.state = GAMESTATE_NONE;
				}
				else if (pArrow->pos.y == QUIT_POS_Y)
				{
					SetFade(MODE_TITLE);
					g_game.state = GAMESTATE_NONE;
				}
			}
			break;
		}
	}
}

//=============================================================================
// ゲーム画面の描画処理
//=============================================================================
void DrawGame()
{
	LPDIRECT3DDEVICE9	pDevice = GetDevice();

	// カメラの設定
	SetCamera(g_game.nNowPlayer);
	
	// メッシュフィールドの描画処理
	//DrawMeshField();
	
	// フィールドの描画
	DrawField();

	// 影の描画処理
	DrawShadow();
	
	// プレイヤーの描画処理
	DrawPlayer();

	//ボールの描画
	DrawBall();

	// モデルの描画処理
	DrawModel();
	
	// メッシュウォールの描画処理
	//DrawMeshWall();

	// エフェクトの描画処理
	DrawEffect();

	//カウントダウンの描画
	DrawTime();

	// ポーズ画面描画
	if (g_game.state == GAMESTATE_PAUSE)
	{// ポーズ状態のとき
		DrawPause();
	}
}

//=============================================================================
// 操作中の状態の取得
//=============================================================================
int *GetPlayer(void)
{
	return &g_game.nNowPlayer;
}

//=============================================================================
// 操作プレイヤーの変更
//=============================================================================
void SetChangePlayer(void)
{
	g_game.nNowPlayer++;// 操作プレイヤー変更

	if (g_game.nNowPlayer >= GetNumPad())
	{// 操作プレイヤー以上になった場合
		g_game.nNowPlayer = 0;	// 1Pに変更
	}
}

//=============================================================================
// ゲーム状態の設定
//=============================================================================
void SetGameState(GAMESTATE state)
{
	g_game.state = state;
	g_game.nCounterState = 0;
}

//=============================================================================
// ゲーム状態の取得
//=============================================================================
GAMESTATE *GetGameState(void)
{
	return &g_game.state;
}