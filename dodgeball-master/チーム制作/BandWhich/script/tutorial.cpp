//=============================================================================
//
// チュートリアル画面処理 [tutorial.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "tutorial.h"
#include "camera.h"
#include "fade.h"
#include "game.h"
#include "joypad.h"
#include "keyboard.h"
#include "sound.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define TEXTUREFILE0		"data/TEXTURE/チキンチュー.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE1		"data/TEXTURE/早押しチュー.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE2		"data/TEXTURE/陣取りチュー.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE3		"data/TEXTURE/迷路.jpg"					// 読み込むテクスチャ
#define MAX_TEXTURE			(1)										// 使用するテクスチャ数
#define MAX_SET_TEXTURE		(1)										// セットするテクスチャ数
#define MAX_TUTORIAL		(1)										// チュートリアルの最大数
#define DESCRIPTION_POS_X	(0)										// 説明のX座標
#define DESCRIPTION_POS_Y	(0)										// 説明のY座標
#define TEXTURE_POS_X		(660)									// チュートリアルのX座標
#define TEXTURE_POS_Y		(10)									// チュートリアルのY座標
#define TEXTURE_WIDTH		(180)									// チュートリアルの幅
#define TEXTURE_HEIGHT		(40)									// チュートリアルの高さ
#define TEXT_POS_X			(900)									// テキストのX座標
#define TEXT_POS_Y			(640)									// テキストのY座標
#define TEXT_WIDTH			(360)									// テキストの幅
#define TEXT_HEIGHT			(60)									// テキストの高さ

//=============================================================================
// 構造体定義
//=============================================================================

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9			g_pTextureTutorial[MAX_TEXTURE] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9		g_pVtxBuffTutorial = NULL;				// 頂点バッファへのポインタ

TUTOSTATE					g_tutoState;							// チュートリアルの状態
int							g_nCntTutoState;						// チュートリアル状態カウンター

//=============================================================================
// チュートリアル画面の初期化
//=============================================================================
void InitTutorial(HWND hWnd)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 初期化
	g_tutoState = TUTOSTATE_NORMAL;// 通常状態に設定
	g_nCntTutoState = 0;

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE0, &g_pTextureTutorial[0]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_SET_TEXTURE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTutorial,
		NULL);

	VERTEX_2D *pVtx;// 頂点バッファへのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffTutorial->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);

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
	g_pVtxBuffTutorial->Unlock();
}

//=============================================================================
// チュートリアル画面の終了処理
//=============================================================================
void UninitTutorial()
{
	// テクスチャの開放
	for (int nCntTex = 0; nCntTex < MAX_TEXTURE; nCntTex++)
	{
		if (g_pTextureTutorial[nCntTex] != NULL)
		{
			g_pTextureTutorial[nCntTex]->Release();
			g_pTextureTutorial[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_pVtxBuffTutorial != NULL)
	{
		g_pVtxBuffTutorial->Release();
		g_pVtxBuffTutorial = NULL;
	}
}

//=============================================================================
// チュートリアル画面の更新処理
//=============================================================================
void UpdateTutorial()
{
	if (GetKeyboardTrigger(DIK_SPACE) || GetKeyboardTrigger(DIK_RETURN) ||
		GetJoypadTrigger(0, JOYPADKEY_START) || GetJoypadTrigger(0, JOYPADKEY_A) ||
		GetJoypadTrigger(1, JOYPADKEY_START) || GetJoypadTrigger(1, JOYPADKEY_A) ||
		GetJoypadTrigger(2, JOYPADKEY_START) || GetJoypadTrigger(2, JOYPADKEY_A) ||
		GetJoypadTrigger(3, JOYPADKEY_START) || GetJoypadTrigger(3, JOYPADKEY_A))
	{
		if (GetFade() != FADE_OUT)
		{// モードの設定
			SetFade(MODE_GAME);
		}
	}
}

//=============================================================================
// チュートリアル画面の描画処理
//=============================================================================
void DrawTutorial()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();// デバイスを取得する

	// 頂点バッファをデバイスのデータストリーミングにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffTutorial, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pTextureTutorial[0]);

	// ポリゴン描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
// チュートリアル状態の設定
//=============================================================================
void SetTutoState(TUTOSTATE state)
{
	g_tutoState = state;
	g_nCntTutoState = 0;
}

//=============================================================================
// チュートリアル状態の取得
//=============================================================================
TUTOSTATE *GetTutoState(void)
{
	return &g_tutoState;
}