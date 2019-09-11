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
LPDIRECT3DTEXTURE9			g_pTextureR[MAX_RTEXTURE] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9		g_pVtxBuffR = NULL;				// 頂点バッファへのポインタ

int							g_nCntR;						// 画面遷移カウント
int							g_nCntS[MAX_NUMPLAYER];			// スコアカウンター
int							aData[MAX_NUMPLAYER] = {};		// リザルトスコア
RESULT						g_Result[MAX_RTEXTURE];			// リザルトの情報

//=============================================================================
// リザルト画面の初期化
//=============================================================================
void InitResult()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();// デバイスの取得

	// 初期化
	g_nCntR = 1800;
	int nData;

	for (int nCnt = 0; nCnt < MAX_NUMPLAYER; nCnt++)
	{
		g_nCntS[nCnt] = 0;
	}

	for (int nCnt = 0; nCnt < MAX_RTEXTURE; nCnt++)
	{
		g_Result[nCnt].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Result[nCnt].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Result[nCnt].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		g_Result[nCnt].fWidht = 0.0f;
		g_Result[nCnt].fHeight = 0.0f;
		g_Result[nCnt].nState = 0;
		g_Result[nCnt].nType = 0;
		g_Result[nCnt].bUse = false;
	}

	// リザルトスコアのソート
	for (int nCnt = 0; nCnt < (MAX_NUMPLAYER - 1); nCnt++)
	{
		for (int nCntM = nCnt; nCntM < MAX_NUMPLAYER; nCntM++)
		{
			if (aData[nCnt] < aData[nCntM])
			{
				nData = aData[nCnt];
				aData[nCnt] = aData[nCntM];
				aData[nCntM] = nData;
			}
		}
	}

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE1, &g_pTextureR[0]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE2, &g_pTextureR[1]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE3, &g_pTextureR[2]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE4, &g_pTextureR[3]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE5, &g_pTextureR[4]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_RTEXTURE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffR,
		NULL);

	VERTEX_2D *pVtx;// 頂点バッファへのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffR->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < MAX_RTEXTURE; nCnt++)
	{
		// 頂点の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 同次座標
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラー
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}

	// 頂点データをアンロック
	g_pVtxBuffR->Unlock();

	// メッシュフィールドの初期化処理
	InitMeshField();

	// メッシュウォールの初期化処理
	InitMeshWall();

	// 結果発表
	SetResult(D3DXVECTOR3(420.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 390.0f, R_POS_Y, 4);
	// 一位
	SetResult(D3DXVECTOR3(R_POS_X + 0.0f, R_POS_Y, 0.0f), D3DXVECTOR3(-5.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.7f), R_WIDTH, 600.0f / (4 + 1)*2, 0);		
	// 二位
	SetResult(D3DXVECTOR3(R_POS_X + 180.0f, R_POS_Y + R_HEIGHT / (4 + 1) * 2, 0.0f), D3DXVECTOR3(-5.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.7f), R_WIDTH, R_HEIGHT / (4 + 1), 1);
	// 三位
	SetResult(D3DXVECTOR3(R_POS_X + 360.0f, R_POS_Y + R_HEIGHT / (4 + 1) * 2 + R_HEIGHT / (4 + 1) * 1, 0.0f), D3DXVECTOR3(-5.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.7f), R_WIDTH, R_HEIGHT / (4 + 1), 2);
	// 四位
	SetResult(D3DXVECTOR3(R_POS_X + 540.0f, R_POS_Y + R_HEIGHT / (4 + 1) * 2 + (R_HEIGHT / (4 + 1)) * 2, 0.0f), D3DXVECTOR3(-5.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.7f), R_WIDTH, R_HEIGHT / (4 + 1), 3);
}

//=============================================================================
// リザルト画面の終了処理
//=============================================================================
void UninitResult()
{
	// メッシュフィールドの終了処理
	UninitMeshField();

	// メッシュウォールの終了処理
	UninitMeshWall();

	// テクスチャの開放
	for (int nCntTex = 0; nCntTex < MAX_RTEXTURE; nCntTex++)
	{
		if (g_pTextureR[nCntTex] != NULL)
		{
			g_pTextureR[nCntTex]->Release();
			g_pTextureR[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_pVtxBuffR != NULL)
	{
		g_pVtxBuffR->Release();
		g_pVtxBuffR = NULL;
	}
}

//=============================================================================
// リザルト画面の更新処理
//=============================================================================
void UpdateResult()
{
	VERTEX_2D *pVtx;// 頂点バッファへのポインタ

	// メッシュフィールドの更新処理
	UpdateMeshField();

	// メッシュウォールの更新処理
	UpdateMeshWall();

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffR->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < MAX_RTEXTURE; nCnt++)
	{
		if (g_Result[nCnt].bUse)
		{
			g_Result[nCnt].pos.x += g_Result[nCnt].move.x;

			// 頂点の設定
			pVtx[0].pos = D3DXVECTOR3(g_Result[nCnt].pos.x, g_Result[nCnt].pos.y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_Result[nCnt].pos.x + g_Result[nCnt].fWidht, g_Result[nCnt].pos.y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(g_Result[nCnt].pos.x, g_Result[nCnt].pos.y + g_Result[nCnt].fHeight, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_Result[nCnt].pos.x + g_Result[nCnt].fWidht, g_Result[nCnt].pos.y + g_Result[nCnt].fHeight, 0.0f);

			// 移動制限
			if (g_Result[nCnt].pos.x <= 0.0f)
			{
				g_Result[nCnt].move.x = 0.0f;

				switch (g_Result[nCnt].nType)
				{
				case RTYPE_FIRST:	// 0

					g_nCntS[RTYPE_FIRST]++;

					break;

				case RTYPE_SECOND:	// 1

					g_nCntS[RTYPE_SECOND]++;

					//pFunds += RTYPE_SECOND;

					break;

				case RTYPE_THIRD:	// 2

					g_nCntS[RTYPE_THIRD]++;

					//pFunds += RTYPE_THIRD;
					break;

				case RTYPE_FOUR:	// 3

					g_nCntS[RTYPE_FOUR]++;

					//pFunds += RTYPE_FOUR;
					break;
				}
			}
		}

		pVtx += 4;
	}

	// 頂点データをアンロック
	g_pVtxBuffR->Unlock();

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
	LPDIRECT3DDEVICE9 pDevice;

	// メッシュフィールドの描画処理
	DrawMeshField();

	// メッシュウォールの描画処理
	DrawMeshWall();

	// デバイスを取得する
	pDevice = GetDevice();

	// 頂点バッファをデバイスのデータストリーミングにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffR, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntFR = 0; nCntFR < MAX_RTEXTURE; nCntFR++)
	{
		if (g_Result[nCntFR].bUse)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureR[g_Result[nCntFR].nType]);

			// ポリゴン描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntFR * 4, 2);
		}
	}
}

//=============================================================================
// リザルトの配置情報
//=============================================================================
void SetResult(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nType)
{
	VERTEX_2D *pVtx;// 頂点バッファへのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffR->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < MAX_RTEXTURE; nCnt++)
	{
		if (!g_Result[nCnt].bUse)
		{
			g_Result[nCnt].pos = pos;
			g_Result[nCnt].move = move;
			g_Result[nCnt].col = col;
			g_Result[nCnt].fWidht = fWidth;
			g_Result[nCnt].fHeight = fHeight;
			g_Result[nCnt].nType = nType;
			g_Result[nCnt].bUse = true;

			// 頂点の設定
			pVtx[0].pos = D3DXVECTOR3(g_Result[nCnt].pos.x, g_Result[nCnt].pos.y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_Result[nCnt].pos.x + g_Result[nCnt].fWidht, g_Result[nCnt].pos.y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(g_Result[nCnt].pos.x, g_Result[nCnt].pos.y + g_Result[nCnt].fHeight, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_Result[nCnt].pos.x + g_Result[nCnt].fWidht, g_Result[nCnt].pos.y + g_Result[nCnt].fHeight, 0.0f);

			// 色の設定
			pVtx[0].col = g_Result[nCnt].col;
			pVtx[1].col = g_Result[nCnt].col;
			pVtx[2].col = g_Result[nCnt].col;
			pVtx[3].col = g_Result[nCnt].col;

			break;
		}

		pVtx += 4;
	}

	// 頂点データをアンロック
	g_pVtxBuffR->Unlock();
}
