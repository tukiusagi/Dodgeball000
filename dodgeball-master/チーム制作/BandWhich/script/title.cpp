//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "title.h"
#include "camera.h"
#include "fade.h"
#include "joypad.h"
#include "keyboard.h"
#include "meshField.h"
#include "meshWall.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "sound.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define TEXTUREFILE0	"data/TEXTURE/RideParty.png"			// 読み込むタイトルテクスチャ
#define TEXTUREFILE1	"data/TEXTURE/text_press_button.png"	// 読み込むテキストテクスチャ
#define TEXTUREFILE2	"data/TEXTURE/start1.png"				// 読み込むテキストテクスチャ
#define TEXTUREFILE3	"data/TEXTURE/ターン設定.png"			// 読み込むテキストテクスチャ
#define TEXTUREFILE4	"data/TEXTURE/NumSet.png"				// 読み込むテキストテクスチャ
#define TEXTUREFILE5	"data/TEXTURE/車.png"					// 読み込むテキストテクスチャ

#define TITLE_POS_X		(640)									// タイトルのX座標
#define TITLE_POS_Y		(150)									// タイトルのY座標
#define TITLE_WIDTH		(1000)									// タイトルの幅
#define TITLE_HEIGHT	(150)									// タイトルの高さ
#define TEXT_POS_X		(400)									// テキストのX座標
#define TEXT_POS_Y		(500)									// テキストのY座標
#define TEXT_WIDTH		(480)									// テキストの幅
#define TEXT_HEIGHT		(80)									// テキストの高さ

#define PAUSE_POS_X		(640)									// セレクトメニュー全体のX座標

#define CONTINUE_POS_Y	(410)									// 始めるのY座標
#define RETEY_POS_Y		(510)									// ターン設定のY座標
#define QUIT_POS_Y		(600)									// 終了のY座標

#define CONTINUE_WIDTH	(300)									// 始めるの幅
#define CONTINUE_HEIGHT	(100)									// 始めるの高さ
#define RETEY_WIDTH		(300)									// ターン設定の幅
#define RETEY_HEIGHT	(100)									// ターン設定の高さ
#define QUIT_WIDTH		(300)									// 戻るの幅
#define QUIT_HEIGHT		(100)									// 戻るの高さ
#define ARROW_WIDTH		(70)									// 矢印の幅
#define ARROW_HEIGHT	(50)									// 矢印の高さ

#define TURNSET_POS_Y	(370)									//ターン設定テクスチャのY座標
#define TURN5_POS_Y		(470)									//5ターンテクスチャのY座標
#define TURN10_POS_Y	(570)									//10ターンテクスチャのY座標
#define TURN15_POS_Y	(670)									//15ターンテクスチャのY座標

#define NUMSET_POS_Y	(370)									//人数設定テクスチャのY座標
#define N2_POS_Y		(470)									//2テクスチャのY座標
#define N3_POS_Y		(570)									//3テクスチャのY座標
#define N4_POS_Y		(670)									//4テクスチャのY座標

#define TURN_WIDTH		(230)									//ターンテクスチャの幅
#define TURN_HEIGHT		(70)									//ターンテクスチャの高さ

#define MAX_TEXTURE		(12)										// 使用するテクスチャ数
#define NUM_TEXT		(12)										// テキスト数
#define MAX_VERTEX		(4)										// テクスチャの頂点数

//=============================================================================
// タイトルの状態
//=============================================================================
typedef enum
{
	TITLESTATE_STANDBY = 0,		// 待機状態
	TITLESTATE_SELECT,			// モード選択状態
	TITLESTATE_FADE,			// 画面遷移状態
	TITLESTATE_MAX				// 状態の最大数
} TITLESTATE;

//=============================================================================
// 構造体定義＿[マクロ定義が上にないとマジックナンバーがわからない]
//=============================================================================
typedef struct
{
	D3DXVECTOR3 pos;
	D3DCOLOR col;
	bool bUse;
} TITLETEXT;

typedef struct
{
	D3DXVECTOR3 pos;
	bool bUse;
} ARROW;

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9		g_pTextureTitle[MAX_TEXTURE] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitle = NULL;				// 頂点バッファへのポインタ

TITLESTATE				g_TitleState;						// タイトルの状態

int						g_nCntTitleText;					// テキストカウント用
TITLETEXT				g_TitleText[NUM_TEXT];				// タイトルテキスト情報
ARROW					g_TitleArrow;						// 矢印の情報

JOYPADUSE				g_TitlePadUse;						// ジョイパッドの使用状態の情報

int						g_nSetTurn;							// 設定ターン数

//=============================================================================
// タイトル画面の初期化
//=============================================================================
void InitTitle()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();// デバイスの取得

	// 初期化
	g_nCntTitleText = 60;
	g_TitleState = TITLESTATE_STANDBY;
	g_TitleArrow.pos = D3DXVECTOR3(450, CONTINUE_POS_Y, 0.0f);
	g_TitleArrow.bUse = false;
	g_nSetTurn = 1;

	for (int nCntTitle = 0; nCntTitle < NUM_TEXT; nCntTitle++)
	{//テクスチャの初期化
		if (nCntTitle == 0 || nCntTitle == 1)
		{
			g_TitleText[nCntTitle].bUse = true;
		}
		else
		{
			g_TitleText[nCntTitle].bUse = false;
		}

		g_TitleText[nCntTitle].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_TitleText[nCntTitle].col= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャの読み込み
	// タイトル
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE0, &g_pTextureTitle[0]);

	// テキスト
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE1, &g_pTextureTitle[1]);		// プレスボタン
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE2, &g_pTextureTitle[2]);		// 始める
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE3, &g_pTextureTitle[3]);		// 設定
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE4, &g_pTextureTitle[4]);		// 戻る
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE5, &g_pTextureTitle[5]);		// 矢印

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TEXTURE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTitle,
		NULL);

	VERTEX_2D *pVtx;// 頂点バッファへのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	// タイトル
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(TITLE_POS_X - (TITLE_WIDTH / 2), TITLE_POS_Y - (TITLE_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TITLE_POS_X + (TITLE_WIDTH / 2), TITLE_POS_Y - (TITLE_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TITLE_POS_X - (TITLE_WIDTH / 2), TITLE_POS_Y + (TITLE_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TITLE_POS_X + (TITLE_WIDTH / 2), TITLE_POS_Y + (TITLE_HEIGHT / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// テキスト
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(TEXT_POS_X, TEXT_POS_Y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TEXT_POS_X + TEXT_WIDTH, TEXT_POS_Y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TEXT_POS_X, TEXT_POS_Y + TEXT_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TEXT_POS_X + TEXT_WIDTH, TEXT_POS_Y + TEXT_HEIGHT, 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// 始める
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(PAUSE_POS_X - (CONTINUE_WIDTH / 2), CONTINUE_POS_Y - (CONTINUE_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PAUSE_POS_X + (CONTINUE_WIDTH / 2), CONTINUE_POS_Y - (CONTINUE_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PAUSE_POS_X - (CONTINUE_WIDTH / 2), CONTINUE_POS_Y + (CONTINUE_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PAUSE_POS_X + (CONTINUE_WIDTH / 2), CONTINUE_POS_Y + (CONTINUE_HEIGHT / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// ターン設定
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(PAUSE_POS_X - (RETEY_WIDTH / 2), RETEY_POS_Y - (RETEY_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PAUSE_POS_X + (RETEY_WIDTH / 2), RETEY_POS_Y - (RETEY_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PAUSE_POS_X - (RETEY_WIDTH / 2), RETEY_POS_Y + (RETEY_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PAUSE_POS_X + (RETEY_WIDTH / 2), RETEY_POS_Y + (RETEY_HEIGHT / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// 戻る
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(PAUSE_POS_X - (QUIT_WIDTH / 2), QUIT_POS_Y - (QUIT_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PAUSE_POS_X + (QUIT_WIDTH / 2), QUIT_POS_Y - (QUIT_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PAUSE_POS_X - (QUIT_WIDTH / 2), QUIT_POS_Y + (QUIT_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PAUSE_POS_X + (QUIT_WIDTH / 2), QUIT_POS_Y + (QUIT_HEIGHT / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// 矢印
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(g_TitleArrow.pos.x - (ARROW_WIDTH / 2), g_TitleArrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(g_TitleArrow.pos.x + (ARROW_WIDTH / 2), g_TitleArrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(g_TitleArrow.pos.x - (ARROW_WIDTH / 2), g_TitleArrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(g_TitleArrow.pos.x + (ARROW_WIDTH / 2), g_TitleArrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データをアンロック
	g_pVtxBuffTitle->Unlock();

	// カメラの初期化処理
	InitCamera();

	// カメラの注視点設定
	SetCameraPosR(D3DXVECTOR3(0.0f, 50.0f, 0.0f), 0);

	// メッシュフィールドの初期化処理
	InitMeshField();

	// メッシュウォールの初期化処理
	InitMeshWall();

	// 影の初期化処理
	InitShadow();

	// プレイヤーの終了処理
	//InitPlayer();

	// モデルの初期化処理
	InitModel();

	SetModel(D3DXVECTOR3(200.0f, 0.0f, 100.0f), D3DXVECTOR3(0.0f, D3DX_PI / 2, 0.0f), MODELTYPE_BILL0);
	SetModel(D3DXVECTOR3(-200.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), MODELTYPE_BILL1);
	SetModel(D3DXVECTOR3(200.0f, 0.0f, -30.0f), D3DXVECTOR3(0.0f, D3DX_PI / 2, 0.0f), MODELTYPE_BILL2);
	SetModel(D3DXVECTOR3(200.0f, 0.0f, -150.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MODELTYPE_BILL3);
	SetModel(D3DXVECTOR3(-220.0f, 0.0f, -120.0f), D3DXVECTOR3(0.0f, -D3DX_PI / 2, 0.0f), MODELTYPE_BILL4);

	SetModel(D3DXVECTOR3(450.0f, 0.0f, -100.0f), D3DXVECTOR3(0.0f, -D3DX_PI / 2, 0.0f), MODELTYPE_CONVENI);
	SetModel(D3DXVECTOR3(-380.0f, 0.0f, 250.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MODELTYPE_CONVENI);

	SetModel(D3DXVECTOR3(-380.0f, 0.0f, -280.0f), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), MODELTYPE_WAC);
}

//=============================================================================
// タイトル画面の終了処理
//=============================================================================
void UninitTitle()
{
	// テクスチャの開放
	for (int nCntTex = 0; nCntTex < MAX_TEXTURE; nCntTex++)
	{
		if (g_pTextureTitle[nCntTex] != NULL)
		{
			g_pTextureTitle[nCntTex]->Release();
			g_pTextureTitle[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_pVtxBuffTitle != NULL)
	{
		g_pVtxBuffTitle->Release();
		g_pVtxBuffTitle = NULL;
	}

	// メッシュフィールドの終了処理
	UninitMeshField();

	// メッシュウォールの終了処理
	UninitMeshWall();

	// 影の終了処理
	UninitShadow();

	// プレイヤーの終了処理
	//UninitPlayer();

	// モデルの終了処理
	UninitModel();
}

//=============================================================================
// タイトル画面の更新処理
//=============================================================================
void UpdateTitle()
{
	VERTEX_2D *pVtx;// 頂点バッファへのポインタ
	int nCntTitle;
	int nStickH, nStickV;

	// アナログスティック左の取得
	GetJoypadStickLeft(0, &nStickH, &nStickV);

	g_nCntTitleText--;

	// カメラの注視点の回転
	RotMoveCameraR(0.005f, 0);

	// メッシュフィールドの更新処理
	UpdateMeshField();

	// メッシュウォールの更新処理
	UpdateMeshWall();

	// モデルの更新処理
	UpdateModel();

	switch (g_TitleState)
	{
//=============================================================================
	case TITLESTATE_STANDBY:
//=============================================================================

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += 4;

		// 頂点カラーの更新(時間で点滅)
		if (g_nCntTitleText <= 0)
		{
			g_nCntTitleText = 60;
		}
		else if (g_nCntTitleText <= 20)
		{
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		}
		else if (g_nCntTitleText <= 60)
		{
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// 頂点データをアンロック
		g_pVtxBuffTitle->Unlock();

		if (GetKeyboardTrigger(DIK_SPACE) || GetKeyboardTrigger(DIK_RETURN) ||
			GetJoypadTrigger(0, JOYPADKEY_START) || GetJoypadTrigger(0, JOYPADKEY_A))
		{
			// 効果音の設定
			PlaySound(SOUND_LABEL_SE_DECISION);

			g_TitleState = TITLESTATE_SELECT;

		}
		break;
//=============================================================================
	case TITLESTATE_SELECT:
//=============================================================================

		g_TitleArrow.bUse = true;
		g_TitleText[1].bUse = false;

		for (nCntTitle = 2; nCntTitle < NUM_TEXT; nCntTitle++)
		{
			g_TitleText[nCntTitle].bUse = true;

			if (nCntTitle == 6 || nCntTitle == 7 || nCntTitle == 8|| nCntTitle == 9 || nCntTitle == 10 || nCntTitle == 11)
			{
				g_TitleText[nCntTitle].bUse = false;
			}
		}

		if (GetKeyboardTrigger(DIK_W) || GetKeyboardTrigger(DIK_UP) || GetJoypadTrigger(0, JOYPADKEY_UP) || nStickV > 0)
		{
			if (g_TitlePadUse.UpThumb == false)
			{
				PlaySound(SOUND_LABEL_SE_POUSE_0);// 効果音の設定

				if (g_TitleArrow.pos.y == CONTINUE_POS_Y)
				{

				}
				else if (g_TitleArrow.pos.y == RETEY_POS_Y)
				{
					PlaySound(SOUND_LABEL_SE_POUSE_0);// 効果音の設定
					g_TitleArrow.pos.y = CONTINUE_POS_Y;
				}
				else if (g_TitleArrow.pos.y == QUIT_POS_Y)
				{
					PlaySound(SOUND_LABEL_SE_POUSE_0);// 効果音の設定
					g_TitleArrow.pos.y = RETEY_POS_Y;
				}
				g_TitlePadUse.UpThumb = true;
			}
		}
		else if (GetKeyboardTrigger(DIK_S) || GetKeyboardTrigger(DIK_DOWN) || GetJoypadTrigger(0, JOYPADKEY_DOWN) || nStickV < 0)
		{
			if (g_TitlePadUse.DownThumb == false)
			{
				PlaySound(SOUND_LABEL_SE_POUSE_0);// 効果音の設定

				if (g_TitleArrow.pos.y == CONTINUE_POS_Y)
				{
					PlaySound(SOUND_LABEL_SE_POUSE_0);// 効果音の設定
					g_TitleArrow.pos.y = RETEY_POS_Y;
				}
				else if (g_TitleArrow.pos.y == RETEY_POS_Y)
				{
					PlaySound(SOUND_LABEL_SE_POUSE_0);// 効果音の設定
					g_TitleArrow.pos.y = QUIT_POS_Y;
				}
				else if (g_TitleArrow.pos.y == QUIT_POS_Y)
				{

				}
				g_TitlePadUse.DownThumb = true;
			}
		}
		else
		{
			g_TitlePadUse.UpThumb = false;
			g_TitlePadUse.DownThumb = false;
		}

		if (GetKeyboardTrigger(DIK_SPACE) || GetKeyboardTrigger(DIK_RETURN) || GetJoypadTrigger(0, JOYPADKEY_A))
		{//モードの選択肢
			PlaySound(SOUND_LABEL_SE_POUSE_1);// 効果音の設定

			if (g_TitleArrow.pos.y == CONTINUE_POS_Y)
			{//始める
				g_TitleState = TITLESTATE_FADE;

				// フェードの設定
				SetFade(MODE_TUTORIAL);
			}
			else if (g_TitleArrow.pos.y == RETEY_POS_Y)
			{//オプション
				//g_TitleState = TITLESTATE_TURN;
			}
			else if (g_TitleArrow.pos.y == QUIT_POS_Y)
			{//戻る
				//g_TitleState = TITLESTATE_PLAYERNUM;
			}
		}

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += 4 * 3;//ターン設定まで進める

		pVtx[0].pos = D3DXVECTOR3(PAUSE_POS_X - (RETEY_WIDTH / 2), RETEY_POS_Y - (RETEY_HEIGHT / 2), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(PAUSE_POS_X + (RETEY_WIDTH / 2), RETEY_POS_Y - (RETEY_HEIGHT / 2), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(PAUSE_POS_X - (RETEY_WIDTH / 2), RETEY_POS_Y + (RETEY_HEIGHT / 2), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(PAUSE_POS_X + (RETEY_WIDTH / 2), RETEY_POS_Y + (RETEY_HEIGHT / 2), 0.0f);

		pVtx += 4;// 人数設定まで進める

		pVtx[0].pos = D3DXVECTOR3(PAUSE_POS_X - (QUIT_WIDTH / 2), QUIT_POS_Y - (QUIT_HEIGHT / 2), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(PAUSE_POS_X + (QUIT_WIDTH / 2), QUIT_POS_Y - (QUIT_HEIGHT / 2), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(PAUSE_POS_X - (QUIT_WIDTH / 2), QUIT_POS_Y + (QUIT_HEIGHT / 2), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(PAUSE_POS_X + (QUIT_WIDTH / 2), QUIT_POS_Y + (QUIT_HEIGHT / 2), 0.0f);

		pVtx += 4;// 矢印まで進める

		// 頂点の設定
		pVtx[0].pos = D3DXVECTOR3(g_TitleArrow.pos.x - (ARROW_WIDTH / 2), g_TitleArrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_TitleArrow.pos.x + (ARROW_WIDTH / 2), g_TitleArrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_TitleArrow.pos.x - (ARROW_WIDTH / 2), g_TitleArrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_TitleArrow.pos.x + (ARROW_WIDTH / 2), g_TitleArrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);

		// 頂点データをアンロック
		g_pVtxBuffTitle->Unlock();

		break;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += 4 * 5;// 矢印まで進める

		// 頂点の設定
		pVtx[0].pos = D3DXVECTOR3(g_TitleArrow.pos.x - (ARROW_WIDTH / 2), g_TitleArrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_TitleArrow.pos.x + (ARROW_WIDTH / 2), g_TitleArrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_TitleArrow.pos.x - (ARROW_WIDTH / 2), g_TitleArrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_TitleArrow.pos.x + (ARROW_WIDTH / 2), g_TitleArrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);

		// 頂点データをアンロック
		g_pVtxBuffTitle->Unlock();

		break;

//=============================================================================
	case TITLESTATE_FADE:
//=============================================================================

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += 4;

		// 頂点カラーの更新
		if (g_nCntTitleText <= 0)
		{
			g_nCntTitleText = 4;
		}
		else if (g_nCntTitleText <= 2)
		{
			pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		}
		else if (g_nCntTitleText <= 4)
		{
			pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
		// 頂点データをアンロック
		g_pVtxBuffTitle->Unlock();

		break;
	}
}

//=============================================================================
// タイトル画面の描画処理
//=============================================================================
void DrawTitle()
{
	// カメラの設定
	SetCamera(0);

	// メッシュフィールドの描画処理
	DrawMeshField();

	// メッシュウォールの描画処理
	DrawMeshWall();

	// モデルの描画処理
	DrawModel();

	LPDIRECT3DDEVICE9 pDevice = GetDevice();// デバイスを取得する

	// 頂点バッファをデバイスのデータストリーミングにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffTitle, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// タイトルとテキストの描画
	for (int nCntTitle = 0; nCntTitle < NUM_TEXT; nCntTitle++)
	{
		if (g_TitleText[nCntTitle].bUse == true)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureTitle[nCntTitle]);

			// ポリゴン描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTitle *4, 2);
		}
	}

	if (g_TitleArrow.bUse == true)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureTitle[5]);

		// ポリゴン描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 5 * 4, 2);
	}
}
