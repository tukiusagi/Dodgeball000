//=============================================================================
//
// ポーズ画面処理 [pause.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "pause.h"
#include "joypad.h"
#include "keyboard.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define TEXTUREFILE0	"data/TEXTURE/bg_pause.jpg"	// 読み込むテクスチャ1
#define TEXTUREFILE1	"data/TEXTURE/pause100.jpg"	// 読み込むテクスチャ2
#define TEXTUREFILE2	"data/TEXTURE/pause000.png"	// 読み込むテクスチャ3
#define TEXTUREFILE3	"data/TEXTURE/pause001.png"	// 読み込むテクスチャ4
#define TEXTUREFILE4	"data/TEXTURE/pause002.png"	// 読み込むテクスチャ5
#define TEXTUREFILE5	"data/TEXTURE/arrow.png"	// 読み込むテクスチャ6
#define MAX_TEXTURE		(6)							// テクスチャ数
#define BG_POS_X		(0)							// 背景のX座標
#define BG_POS_Y		(0)							// 背景のY座標
#define PAUSE_POS_X		(640)						// ポーズの枠のX座標
#define PAUSE_POS_Y		(360)						// ポーズの枠のY座標
#define CONTINUE_POS_Y	(300)						// コンテニューのY座標
#define RETEY_POS_Y		(400)						// リトライのY座標
#define QUIT_POS_Y		(500)						// 終了のY座標
#define PAUSE_WIDTH		(500)						// ポーズの枠の幅
#define CONTINUE_WIDTH	(300)						// コンテニューの幅
#define CONTINUE_HEIGHT	(50)						// コンテニューの高さ
#define RETEY_WIDTH		(300)						// コンテニューの幅
#define RETEY_HEIGHT	(50)						// コンテニューの高さ
#define QUIT_WIDTH		(300)						// コンテニューの幅
#define QUIT_HEIGHT		(50)						// コンテニューの高さ
#define ARROW_WIDTH		(50)						// 矢印の幅
#define ARROW_HEIGHT	(50)						// 矢印の高さ

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void MakeVertexPause(LPDIRECT3DDEVICE9 pDevice);

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9		g_pTexturePause[MAX_TEXTURE] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffPause = NULL;				// 頂点バッファへのポインタ

ARROW					g_arrow;							// 矢印の情報
JOYPADUSE				g_PausePadUse;						// ジョイパッドの使用状態の情報

//=============================================================================
// ポーズ画面の初期化
//=============================================================================
void InitPause(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();// デバイスの取得

	// 初期化
	g_arrow.pos = D3DXVECTOR3(465, CONTINUE_POS_Y, 0.0f);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE0, &g_pTexturePause[0]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE1, &g_pTexturePause[1]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE2, &g_pTexturePause[2]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE3, &g_pTexturePause[3]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE4, &g_pTexturePause[4]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE5, &g_pTexturePause[5]);

	// 頂点情報の作成
	MakeVertexPause(pDevice);
}

//=============================================================================
// ポーズ画面の解放処理
//=============================================================================
void UninitPause(void)
{
	// テクスチャの開放
	for (int nCntTex = 0; nCntTex < MAX_TEXTURE; nCntTex++)
	{
		if (g_pTexturePause[nCntTex] != NULL)
		{
			g_pTexturePause[nCntTex]->Release();
			g_pTexturePause[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_pVtxBuffPause != NULL)
	{
		g_pVtxBuffPause->Release();
		g_pVtxBuffPause = NULL;
	}
}

//=============================================================================
// ポーズ画面の更新処理
//=============================================================================
void UpdatePause(void)
{
	VERTEX_2D *pVtx;// 頂点バッファへのポインタ
	int nStickH, nStickV;

	// アナログスティック左の取得
	GetJoypadStickLeft(0, &nStickH, &nStickV);

	if (GetKeyboardTrigger(DIK_W) || GetKeyboardTrigger(DIK_UP) || GetJoypadTrigger(0, JOYPADKEY_UP)
		|| GetJoypadTrigger(1, JOYPADKEY_UP) 
		|| GetJoypadTrigger(2, JOYPADKEY_UP) 
		|| GetJoypadTrigger(3, JOYPADKEY_UP) || nStickV > 0)
	{
		if (g_PausePadUse.UpThumb == false)
		{
			//PlaySound(SOUND_LABEL_SE_POUSE_0);// 効果音の設定

			if (g_arrow.pos.y == CONTINUE_POS_Y)
			{

			}
			else if (g_arrow.pos.y == RETEY_POS_Y)
			{
				g_arrow.pos.y = CONTINUE_POS_Y;
			}
			else if (g_arrow.pos.y == QUIT_POS_Y)
			{
				g_arrow.pos.y = RETEY_POS_Y;
			}
			g_PausePadUse.UpThumb = true;
		}
	}
	else if (GetKeyboardTrigger(DIK_S) || GetKeyboardTrigger(DIK_DOWN) || GetJoypadTrigger(0, JOYPADKEY_DOWN)
		|| GetJoypadTrigger(1, JOYPADKEY_DOWN) 
		|| GetJoypadTrigger(2, JOYPADKEY_DOWN) 
		|| GetJoypadTrigger(3, JOYPADKEY_DOWN) || nStickV < 0)
	{
		if (g_PausePadUse.DownThumb == false)
		{
			//PlaySound(SOUND_LABEL_SE_POUSE_0);// 効果音の設定

			if (g_arrow.pos.y == CONTINUE_POS_Y)
			{
				g_arrow.pos.y = RETEY_POS_Y;
			}
			else if (g_arrow.pos.y == RETEY_POS_Y)
			{
				g_arrow.pos.y = QUIT_POS_Y;
			}
			else if (g_arrow.pos.y == QUIT_POS_Y)
			{

			}
			g_PausePadUse.DownThumb = true;
		}
	}
	else
	{
		g_PausePadUse.UpThumb = false;
		g_PausePadUse.DownThumb = false;
	}

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffPause->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += 4 * 5;// 矢印まで進める

	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(g_arrow.pos.x - (ARROW_WIDTH / 2), g_arrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(g_arrow.pos.x + (ARROW_WIDTH / 2), g_arrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(g_arrow.pos.x - (ARROW_WIDTH / 2), g_arrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(g_arrow.pos.x + (ARROW_WIDTH / 2), g_arrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);

	// 頂点データをアンロック
	g_pVtxBuffPause->Unlock();
}

//=============================================================================
// ポーズ画面の描画処理
//=============================================================================
void DrawPause(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();// デバイスを取得する

	// 頂点バッファをデバイスのデータストリーミングにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffPause, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntPause = 0; nCntPause < MAX_TEXTURE; nCntPause++)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTexturePause[nCntPause]);

		// ポリゴン描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntPause * 4, 2);
	}
}

//=============================================================================
// 頂点の作成
//=============================================================================
void MakeVertexPause(LPDIRECT3DDEVICE9 pDevice)
{
	VERTEX_2D *pVtx;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TEXTURE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffPause,
		NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffPause->Lock(0, 0, (void**)&pVtx, 0);

	// 半透明な背景
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(BG_POS_X, BG_POS_Y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(BG_POS_X + SCREEN_WIDTH, BG_POS_Y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(BG_POS_X, BG_POS_Y + SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(BG_POS_X + SCREEN_WIDTH, BG_POS_Y + SCREEN_HEIGHT, 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DCOLOR_RGBA(0, 0, 0, 150);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// ポーズメニューの枠
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(PAUSE_POS_X - (PAUSE_WIDTH / 2), PAUSE_POS_Y - (PAUSE_WIDTH / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PAUSE_POS_X + (PAUSE_WIDTH / 2), PAUSE_POS_Y - (PAUSE_WIDTH / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PAUSE_POS_X - (PAUSE_WIDTH / 2), PAUSE_POS_Y + (PAUSE_WIDTH / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PAUSE_POS_X + (PAUSE_WIDTH / 2), PAUSE_POS_Y + (PAUSE_WIDTH / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// コンテニュー
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(PAUSE_POS_X - (CONTINUE_WIDTH / 2), CONTINUE_POS_Y - (CONTINUE_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PAUSE_POS_X + (CONTINUE_WIDTH / 2), CONTINUE_POS_Y - (CONTINUE_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PAUSE_POS_X - (CONTINUE_WIDTH / 2), CONTINUE_POS_Y + (CONTINUE_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PAUSE_POS_X + (CONTINUE_WIDTH / 2), CONTINUE_POS_Y + (CONTINUE_HEIGHT / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// リトライ
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(PAUSE_POS_X - (RETEY_WIDTH / 2), RETEY_POS_Y - (RETEY_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PAUSE_POS_X + (RETEY_WIDTH / 2), RETEY_POS_Y - (RETEY_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PAUSE_POS_X - (RETEY_WIDTH / 2), RETEY_POS_Y + (RETEY_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PAUSE_POS_X + (RETEY_WIDTH / 2), RETEY_POS_Y + (RETEY_HEIGHT / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// 終了
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(PAUSE_POS_X - (QUIT_WIDTH / 2), QUIT_POS_Y - (QUIT_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PAUSE_POS_X + (QUIT_WIDTH / 2), QUIT_POS_Y - (QUIT_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PAUSE_POS_X - (QUIT_WIDTH / 2), QUIT_POS_Y + (QUIT_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PAUSE_POS_X + (QUIT_WIDTH / 2), QUIT_POS_Y + (QUIT_HEIGHT / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// 矢印
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(g_arrow.pos.x - (ARROW_WIDTH / 2), g_arrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
	pVtx[1].pos = D3DXVECTOR3(g_arrow.pos.x + (ARROW_WIDTH / 2), g_arrow.pos.y - (ARROW_HEIGHT / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(g_arrow.pos.x - (ARROW_WIDTH / 2), g_arrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(g_arrow.pos.x + (ARROW_WIDTH / 2), g_arrow.pos.y + (ARROW_HEIGHT / 2), 0.0f);

	// 同次座標
	pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
	
	// 頂点データをアンロックする
	g_pVtxBuffPause->Unlock();
}

//=============================================================================
// 矢印情報の取得
//=============================================================================
ARROW *GetArrow(void)
{
	return &g_arrow;
}