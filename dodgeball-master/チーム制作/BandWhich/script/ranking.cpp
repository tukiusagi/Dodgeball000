//=============================================================================
//
// ランキング処理 [ranking.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "ranking.h"
#include "camera.h"
#include "fade.h"
#include "game.h"
#include "joypad.h"
#include "keyboard.h"
#include "meshField.h"
#include "meshWall.h"
#include "model.h"
#include "sound.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define TEXTUREFILE0			"data/TEXTURE/text_ranking.png"			// 読み込むタイトルテクスチャ
#define TEXTUREFILE1			"data/TEXTURE/text_ranking_rank.png"	// 読み込む順位テクスチャ
#define TEXTUREFILE2			"data/TEXTURE/number000.png"			// 読み込むスコアテクスチャ
#define MAX_TEXTURE				(3)										// 使用するテクスチャ数
#define NUM_RANK				(5)										// 順位の数
#define MAX_SCORE				(5)										// 使用するスコアの数
#define NUM_SCORE				(8)										// スコアの桁数
#define MAX_SET_TEXTURE			(2+NUM_RANK+MAX_SCORE*NUM_SCORE)		// 設置するテクスチャの合計数
#define LOGO_POS_X				(460)									// ロゴのX座標
#define LOGO_POS_Y				(50)									// ロゴのY座標
#define LOGO_WIDTH				(360)									// ロゴの幅
#define LOGO_HEIGHT				(60)									// ロゴの高さ
#define RANK_POS_X				(260)									// 順位のX座標
#define RANK_POS_Y				(150)									// 順位のY座標
#define RANK_WIDTH				(128)									// 順位の幅
#define RANK_HEIGHT				(96)									// 順位の高さ
#define RANK_INTERVAL			(100)									// 順位の間隔
#define SCORE_POS_X				(360)									// スコアのX座標
#define SCORE_POS_Y				(165)									// スコアのY座標
#define SCORE_WIDTH				(35)									// スコアの幅
#define SCORE_HEIGHT			(70)									// スコアの高さ
#define SCORE_INTERVAL_WIDTH	(38)									// スコア横の間隔
#define SCORE_INTERVAL_HEIGHT	(100)									// スコア縦の間隔
#define RANKING_TIME			(600)									// ランキング画面表示時間
#define MAX_VERTEX				(4)										// テクスチャの頂点数
#define MAX_DATA				(6)										// ソート用

//=============================================================================
// 構造体定義＿[マクロ定義が上にないとマジックナンバーがわからない]
//=============================================================================
typedef struct// スコア
{
	D3DXVECTOR3 pos;	// 位置
	D3DXCOLOR col;		// 色
} SCORE;

typedef struct // 順位
{
	D3DXVECTOR3 pos;	// 位置
	D3DXCOLOR col;		// 色
} RANK;

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9		g_pTextureRanking[MAX_TEXTURE] = {};						// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRanking = NULL;									// 頂点バッファへのポインタ

int						g_nCntRanking;												// 画面遷移カウント
int						g_aData[MAX_DATA] = { 13500,13250,13000,12750,12500,0 };	// ランキングスコア
int						g_aFlashingData[MAX_DATA];									// 点滅用
SCORE					g_aRankingScore[MAX_SCORE][NUM_SCORE];						// スコアの情報
RANK					g_aRank[NUM_RANK];											// 順位の情報
int						g_nCntRankingText;

//=============================================================================
// ランキング画面の初期化
//=============================================================================
void InitRanking()
{
	LPDIRECT3DDEVICE9 pDevice;
	int nCnt;
	int nCntRanking;
	GAMESTATE *GameState;
	//int *pScore;
	//int *pTime;
	int nData;

	// 初期化
	g_nCntRanking = RANKING_TIME;
	g_nCntRankingText = 30;

	for (nCntRanking = 0; nCntRanking < MAX_DATA; nCntRanking++)
	{
		g_aFlashingData[nCntRanking] = 0;
	}

	for (nCnt = 0; nCnt < MAX_SCORE; nCnt++)// スコア
	{
		for (nCntRanking = 0; nCntRanking < NUM_SCORE; nCntRanking++)
		{
			g_aRankingScore[nCnt][nCntRanking].pos = D3DXVECTOR3(SCORE_POS_X + RANK_WIDTH,
														SCORE_POS_Y + nCnt*SCORE_INTERVAL_HEIGHT*1.0f, 0.0f);
			g_aRankingScore[nCnt][nCntRanking].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	for (nCntRanking = 0; nCntRanking < NUM_RANK; nCntRanking++)// 順位
	{
		g_aRank[nCntRanking].pos=D3DXVECTOR3(RANK_POS_X,
									       RANK_POS_Y + nCnt*RANK_INTERVAL*1.0f, 0.0f);
		g_aRank[nCntRanking].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// ゲーム状態の取得
	GameState = GetGameState();
	
	g_aFlashingData[MAX_DATA - 1] = 1;

	// ランキングのソート
	for (nCnt = 0; nCnt < (MAX_DATA - 1); nCnt++)
	{
		for (nCntRanking = nCnt + 1; nCntRanking < MAX_DATA; nCntRanking++)
		{
			if (g_aData[nCnt] < g_aData[nCntRanking])
			{
				// スコア
				nData = g_aData[nCnt];
				g_aData[nCnt] = g_aData[nCntRanking];
				g_aData[nCntRanking] = nData;

				// 点滅
				nData = g_aFlashingData[nCnt];
				g_aFlashingData[nCnt] = g_aFlashingData[nCntRanking];
				g_aFlashingData[nCntRanking] = nData;
			}
		}
	}

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	// ランキング
	D3DXCreateTextureFromFile(pDevice,
		TEXTUREFILE0,
		&g_pTextureRanking[0]);

	// 順位
	D3DXCreateTextureFromFile(pDevice,
		TEXTUREFILE1,
		&g_pTextureRanking[1]);

	// スコア
	D3DXCreateTextureFromFile(pDevice,
		TEXTUREFILE2,
		&g_pTextureRanking[2]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * (MAX_SET_TEXTURE),
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffRanking,
		NULL);

	VERTEX_2D *pVtx;// 頂点バッファへのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffRanking->Lock(0, 0, (void**)&pVtx, 0);

	// ロゴ
	// 頂点の設定
	pVtx[0].pos = D3DXVECTOR3(LOGO_POS_X, LOGO_POS_Y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(LOGO_POS_X + LOGO_WIDTH, LOGO_POS_Y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(LOGO_POS_X, LOGO_POS_Y + LOGO_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(LOGO_POS_X + LOGO_WIDTH, LOGO_POS_Y + LOGO_HEIGHT, 0.0f);

	// 同次座標
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx += 4;// 頂点データのポインタを4つ分進める

	// 順位
	for (nCntRanking = 0; nCntRanking < NUM_RANK; nCntRanking++)
	{
		// 頂点の設定
		pVtx[0].pos = D3DXVECTOR3(RANK_POS_X, RANK_POS_Y + RANK_INTERVAL*nCntRanking*1.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(RANK_POS_X + RANK_WIDTH, RANK_POS_Y + RANK_INTERVAL*nCntRanking*1.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(RANK_POS_X, RANK_POS_Y + RANK_HEIGHT + RANK_INTERVAL*nCntRanking*1.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(RANK_POS_X + RANK_WIDTH, RANK_POS_Y + RANK_HEIGHT + RANK_INTERVAL*nCntRanking*1.0f, 0.0f);

		// 同次座標
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラー
		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f + nCntRanking*0.2f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f + nCntRanking*0.2f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 0.2f + nCntRanking*0.2f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 0.2f + nCntRanking*0.2f);

		pVtx += 4;// 頂点データのポインタを4つ分進める
	}

	// スコア
	for (nCnt = 0; nCnt < MAX_SCORE; nCnt++)
	{
		for (nCntRanking = 0; nCntRanking < NUM_SCORE; nCntRanking++)
		{
			// スコアの座標の設定
			pVtx[0].pos = D3DXVECTOR3(g_aRankingScore[nCnt][nCntRanking].pos.x + SCORE_INTERVAL_WIDTH*nCntRanking,
				g_aRankingScore[nCnt][nCntRanking].pos.y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aRankingScore[nCnt][nCntRanking].pos.x + SCORE_INTERVAL_WIDTH*nCntRanking + SCORE_WIDTH,
				g_aRankingScore[nCnt][nCntRanking].pos.y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(g_aRankingScore[nCnt][nCntRanking].pos.x + SCORE_INTERVAL_WIDTH*nCntRanking,
				g_aRankingScore[nCnt][nCntRanking].pos.y + SCORE_HEIGHT, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aRankingScore[nCnt][nCntRanking].pos.x + SCORE_INTERVAL_WIDTH*nCntRanking + SCORE_WIDTH,
				g_aRankingScore[nCnt][nCntRanking].pos.y + SCORE_HEIGHT, 0.0f);

			// 同次座標
			pVtx[0].rhw = 1.0f;
			pVtx[1].rhw = 1.0f;
			pVtx[2].rhw = 1.0f;
			pVtx[3].rhw = 1.0f;

			// 頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			int nNumber = g_aData[nCnt] % (int)powf(10.0f, (NUM_SCORE - nCntRanking)*1.0f) /
				(int)powf(10.0f, (NUM_SCORE - 1 - nCntRanking)*1.0f);

			// テクスチャ座標の更新
			pVtx[0].tex = D3DXVECTOR2(0.0f + nNumber*0.1f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.1f + nNumber*0.1f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f + nNumber*0.1f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.1f + nNumber*0.1f, 1.0f);

			pVtx += 4;// 頂点データのポインタを4つ分進める
		}
	}

	// 頂点データをアンロック
	g_pVtxBuffRanking->Unlock();

	// カメラの初期化処理
	InitCamera();

	// カメラの注視点の設定
	SetCameraPosR(D3DXVECTOR3(0.0f, 50.0f, 100.0f), 0);

	// カメラの視点の設定
	SetCameraPosV(D3DXVECTOR3(0.0f, 20.0f, 0.0f), 0);

	// メッシュフィールドの初期化処理
	InitMeshField();

	// メッシュウォールの初期化処理
	InitMeshWall();

	// モデルの初期化処理
	InitModel();

	//SetModel(D3DXVECTOR3(-70.0f, 0.0f, -140.0f), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), MODELTYPE_BILL0);
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
// ランキング画面の終了処理
//=============================================================================
void UninitRanking()
{
	// メッシュフィールドの終了処理
	UninitMeshField();

	// メッシュウォールの終了処理
	UninitMeshWall();

	// モデルの終了処理
	UninitModel();

	// テクスチャの開放
	for (int nCntTex = 0; nCntTex < MAX_TEXTURE; nCntTex++)
	{
		if (g_pTextureRanking[nCntTex] != NULL)
		{
			g_pTextureRanking[nCntTex]->Release();
			g_pTextureRanking[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_pVtxBuffRanking != NULL)
	{
		g_pVtxBuffRanking->Release();
		g_pVtxBuffRanking = NULL;
	}
}

//=============================================================================
// ランキング画面の更新処理
//=============================================================================
void UpdateRanking()
{
	VERTEX_2D *pVtx;

	// カメラの注視点の回転
	RotMoveCameraV(0.005f, 0);

	// メッシュフィールドの更新処理
	UpdateMeshField();

	// メッシュウォールの更新処理
	UpdateMeshWall();

	// モデルの更新処理
	UpdateModel();

	g_nCntRankingText--;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffRanking->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += 4 * 6;

	// 点滅
	for (int nCnt = 0; nCnt < MAX_SCORE; nCnt++)
	{
		for (int nCntRanking = 0; nCntRanking < NUM_SCORE; nCntRanking++)
		{
			if (g_aFlashingData[nCnt] == 1)
			{
				// 頂点カラー
				if (g_nCntRankingText <= 0)
				{
					g_nCntRankingText = 30;
				}
				else if (g_nCntRankingText <= 15)
				{
					pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
					pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
					pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
					pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				}
				else if (g_nCntRankingText <= 30)
				{
					pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
					pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
					pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
					pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				}
			}
			else
			{
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
			pVtx += 4;
		}
	}

	// 頂点データをアンロック
	g_pVtxBuffRanking->Unlock();

	if (GetFade() != FADE_OUT)
	{
		g_nCntRanking--;
		if (g_nCntRanking <= 0)
		{
			//モードの設定
			SetFade(MODE_TITLE);
		}
		if (GetKeyboardTrigger(DIK_RETURN) || GetJoypadTrigger(0, JOYPADKEY_START) || GetJoypadTrigger(0, JOYPADKEY_A))
		{
			//モードの設定
			SetFade(MODE_TITLE);
		}
	}
}

//=============================================================================
// ランキング画面の描画処理
//=============================================================================
void DrawRanking()
{
	// カメラの設定
	SetCamera(0);

	// メッシュフィールドの描画処理
	DrawMeshField();

	// メッシュウォールの描画処理
	DrawMeshWall();

	// モデルの描画処理
	DrawModel();

	// デバイスを取得する
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nCntRanking;

	// 頂点バッファをデバイスのデータストリーミングにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffRanking, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ロゴ描画
	pDevice->SetTexture(0, g_pTextureRanking[0]);			// テクスチャの設定
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);		// ポリゴン描画

	// 順位描画
	for (nCntRanking = 0; nCntRanking < NUM_RANK; nCntRanking++)
	{
		pDevice->SetTexture(0, g_pTextureRanking[1]);
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (1 + nCntRanking) * 4, 2);
	}

	// スコア描画
	for (nCntRanking = 0; nCntRanking < NUM_SCORE*MAX_SCORE; nCntRanking++)
	{
		pDevice->SetTexture(0, g_pTextureRanking[2]);
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (6 + nCntRanking) * 4, 2);
	}
}
