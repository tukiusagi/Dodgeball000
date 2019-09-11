//=============================================================================
//
// タイマー処理 [timer.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "timer.h"
#include "game.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define	TEXTURE_SCORE		"data/TEXTURE/number000.png"	// 読み込む数字テクスチャ
#define MAX_TEXTER			(1)								// 使用するテクスチャ数
#define MAX_TIME			(2)								// タイマーの桁
#define TIME_WIDTH			(30)							// タイマーの幅
#define TIME_HEIGHT			(30)							// タイマーの高さ
#define TIME_INTERVAL		(30)							// タイマーの間隔
#define TIMER_MAX			(10)							// 制限時間
#define TIMER1_MAX			(30)							// 制限時間

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void MakeVertexTimer(LPDIRECT3DDEVICE9 pDevice);

//=============================================================================
// 構造体定義＿[マクロ定義が上にないとマジックナンバーがわからない]
//=============================================================================
typedef struct
{
	D3DXVECTOR3 pos;	// 位置
	D3DXCOLOR col;		// 色
} TIME;

//=============================================================================
// グローバル変数宣言
//=============================================================================
LPDIRECT3DTEXTURE9		g_pTextureTimer[MAX_TEXTER] = {};				// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTimer = NULL;							// 頂点バッファへのポインタ

int						g_nTimer;										// 時間
int						g_nCntTimer;									// 時間カウント用
TIME					g_aTimer[MAX_TIME];								// 時間の情報

//=============================================================================
// 初期化処理
//=============================================================================
void InitTimer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 初期化
	g_nTimer = TIMER_MAX;
	
	g_nCntTimer = 0;
	for (int nCntScore = 0; nCntScore < MAX_TIME; nCntScore++)// タイマー
	{
		g_aTimer[nCntScore].pos = D3DXVECTOR3(600.0f, 5.0f, 0.0f);
		g_aTimer[nCntScore].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}
	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, TEXTURE_SCORE, &g_pTextureTimer[0]);

	// 頂点情報の作成
	MakeVertexTimer(pDevice);
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTimer(void)
{
	// テクスチャの開放
	for (int nCntTex = 0; nCntTex < MAX_TEXTER; nCntTex++)
	{
		if (g_pTextureTimer[nCntTex] != NULL)
		{
			g_pTextureTimer[nCntTex]->Release();
			g_pTextureTimer[nCntTex] = NULL;
		}
	}

	// 頂点バッファの開放
	if (g_pVtxBuffTimer != NULL)
	{
		g_pVtxBuffTimer->Release();
		g_pVtxBuffTimer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTimer(void)
{
	VERTEX_2D *pVtx;

	g_nCntTimer++;
	if (0 == g_nCntTimer % 60)
	{
		g_nTimer--;
	}

	if (g_nTimer >= 0)
	{
		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntScore = 0; nCntScore < MAX_TIME; nCntScore++)
		{
			int nNumber = g_nTimer % (int)powf(10.0f, (MAX_TIME - nCntScore)*1.0f) / (int)powf(10.0f, (MAX_TIME - 1 - nCntScore)*1.0f);

			// テクスチャ座標の更新
			pVtx[0].tex = D3DXVECTOR2(0.0f + nNumber*0.1f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.1f + nNumber*0.1f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f + nNumber*0.1f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.1f + nNumber*0.1f, 1.0f);

			pVtx += 4;// 頂点データのポインタを4つ分進める
		}
		// 頂点データをアンロック
		g_pVtxBuffTimer->Unlock();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTimer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffTimer, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// スコア&タイマーの描画
	for (int nCntScore = 0; nCntScore < MAX_TIME; nCntScore++)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureTimer[0]);

		// ポリゴン描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntScore * 4, 2);
	}
}

//=============================================================================
// 頂点の作成
//=============================================================================
void MakeVertexTimer(LPDIRECT3DDEVICE9 pDevice)
{
	VERTEX_2D *pVtx;

	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TIME,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTimer,
		NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntScore = 0; nCntScore < MAX_TIME; nCntScore++)// タイマー
	{
		// タイマーの座標の設定
		pVtx[0].pos = D3DXVECTOR3(g_aTimer[nCntScore].pos.x + TIME_INTERVAL*nCntScore,
			g_aTimer[nCntScore].pos.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_aTimer[nCntScore].pos.x + TIME_INTERVAL*nCntScore + TIME_WIDTH,
			g_aTimer[nCntScore].pos.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_aTimer[nCntScore].pos.x + TIME_INTERVAL*nCntScore,
			g_aTimer[nCntScore].pos.y + TIME_HEIGHT, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_aTimer[nCntScore].pos.x + TIME_INTERVAL*nCntScore + TIME_WIDTH,
			g_aTimer[nCntScore].pos.y + TIME_HEIGHT, 0.0f);

		// 同次座標
		pVtx[0].rhw = pVtx[1].rhw = pVtx[2].rhw = pVtx[3].rhw = 1.0f;

		// 頂点カラー
		pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = g_aTimer[nCntScore].col;

		int nNumber = g_nTimer % (int)powf(10.0f, (MAX_TIME - nCntScore)*1.0f) / (int)powf(10.0f, (MAX_TIME - 1 - nCntScore)*1.0f);

		// テクスチャ座標の更新
		pVtx[0].tex = D3DXVECTOR2(0.0f + nNumber*0.1f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f + nNumber*0.1f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f + nNumber*0.1f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f + nNumber*0.1f, 1.0f);

		pVtx += 4;// 頂点データのポインタを4つ分進める
	}
	// 頂点データをアンロックする
	g_pVtxBuffTimer->Unlock();
}

//=============================================================================
// 時間情報の取得
//=============================================================================
int *GetTimer(void)
{
	return &g_nTimer;
}
