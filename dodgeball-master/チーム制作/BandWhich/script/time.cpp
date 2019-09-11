//=============================================================================
//
// カウントダウン処理 [time.cpp]
// Author : YONEYAMA TAKESHI
//
//=============================================================================
#include "time.h"
#include "sound.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define	TEXTURE_TIME	"data/TEXTURE/銭湯number001.png"	// 読み込むテクスチャファイル名

#define TIME_POS_X		(1280.0f)							// ポリゴンの表示位置(左上X)
#define TIME_POS_Y		(300.0f)							//ポリゴンの表示位置(左上Y)
#define TIME_SIZE_X		(300)								// ポリゴンのサイズ(横幅)
#define TIME_SIZE_Y		(300)								// ポリゴンのサイズ(縦幅)

#define MAX_TIME (1)			//タイムの最大数
#define FPS_TIME (60)			//1秒カウント

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void MakeVertexTime(LPDIRECT3DDEVICE9 pDevice);

//=============================================================================
// グローバル変数宣言
//=============================================================================
LPDIRECT3DTEXTURE9		g_pTextureTime = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTime = NULL;	// 頂点バッファへのポインタ

int						g_nTime;				// タイムカウント

TIME					g_anTime[MAX_TIME];

//=============================================================================
// タイム初期化処理
//=============================================================================
void InitTime(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	g_nTime = FPS_TIME * 4;

	int nCntTime;
	for (nCntTime = 0; nCntTime < MAX_TIME; nCntTime++)
	{
		g_anTime[nCntTime].bUse = true;
	}

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,				// デバイスへのポインタ
								TEXTURE_TIME,		// ファイルの名前
								&g_pTextureTime);	// 読み込むメモリー
	
	// 頂点情報の作成
	MakeVertexTime(pDevice);
}

//=============================================================================
// タイム終了処理
//=============================================================================
void UninitTime(void)
{
	//テクスチャの開放
	if (g_pTextureTime != NULL)
	{
		g_pTextureTime->Release();
		g_pTextureTime = NULL;
	}

	//頂点バッファの開放
	if (g_pVtxBuffTime != NULL)
	{
		g_pVtxBuffTime->Release();
		g_pVtxBuffTime = NULL;
	}
}

//=============================================================================
// タイム更新処理
//=============================================================================
void UpdateTime(void)
{
	AddTime(-1);
	int nCntTime;
	for (nCntTime = 0; nCntTime < MAX_TIME; nCntTime++)
	{
		if (g_nTime <= FPS_TIME)
		{//1秒以下になったら消える
			g_anTime[nCntTime].bUse = false;
		}
	}
}

//=============================================================================
// タイム描画処理
//=============================================================================
void DrawTime(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nCntTime;

	// アルファテスト
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 200);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	for (nCntTime = 0; nCntTime < MAX_TIME; nCntTime++)
	{
		if (g_anTime[nCntTime].bUse == true)			//使用中なら描画
		{
			// 頂点バッファをデバイスのデータストリームにバインド
			pDevice->SetStreamSource(0, g_pVtxBuffTime, 0, sizeof(VERTEX_2D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_2D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureTime);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTime * 4, 2);
		}
	}

	// アルファテスト戻す
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

}

//=============================================================================
// 頂点の作成
//=============================================================================
void MakeVertexTime(LPDIRECT3DDEVICE9 pDevice)
{
	VERTEX_2D *pVtx;

	int nCntTime;

	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TIME,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffTime,
								NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffTime->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (nCntTime = 0; nCntTime < MAX_TIME; nCntTime++)
	{
		g_anTime[nCntTime].pos = D3DXVECTOR3((TIME_POS_X / 2) + TIME_SIZE_X * nCntTime, TIME_POS_Y, 0.0f);
		g_anTime[nCntTime].nNm = 0;

		pVtx[0].pos = D3DXVECTOR3(g_anTime[nCntTime].pos.x - (TIME_SIZE_X / 2.0f), g_anTime[nCntTime].pos.y - (TIME_SIZE_Y / 2.0f), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_anTime[nCntTime].pos.x + (TIME_SIZE_X / 2.0f), g_anTime[nCntTime].pos.y - (TIME_SIZE_Y / 2.0f), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_anTime[nCntTime].pos.x - (TIME_SIZE_X / 2.0f), g_anTime[nCntTime].pos.y + (TIME_SIZE_Y / 2.0f), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_anTime[nCntTime].pos.x + (TIME_SIZE_X / 2.0f), g_anTime[nCntTime].pos.y + (TIME_SIZE_Y / 2.0f), 0.0f);

		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		 
		pVtx[0].tex = D3DXVECTOR2(0.0f + 0.1f * g_anTime[nCntTime].nNm, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * g_anTime[nCntTime].nNm, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f + 0.1f * g_anTime[nCntTime].nNm, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * g_anTime[nCntTime].nNm, 1.0f);

		pVtx += 4;
	}
	// 頂点データをアンロックする
	g_pVtxBuffTime->Unlock();
}

//=============================================================================
// タイムの変更
//=============================================================================
void AddTime(int nValue)
{
	VERTEX_2D *pVtx;

	int nCntTime;

	g_nTime += nValue;
	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffTime->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntTime = 0; nCntTime < MAX_TIME; nCntTime++)
	{
		g_anTime[nCntTime].nNm = (g_nTime / 60) % (int)powf(10.0f, 1.0f - nCntTime * 1.0f) / (int)powf(10.0f, 1.0f - 1.0f - nCntTime * 1.0f);

		pVtx[0].col = D3DXCOLOR(0.0f, 0.2f * g_anTime[nCntTime].nNm, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(0.0f, 0.2f * g_anTime[nCntTime].nNm, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(0.0f, 0.2f * g_anTime[nCntTime].nNm, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(0.0f, 0.2f * g_anTime[nCntTime].nNm, 1.0f, 1.0f);

		pVtx[0].tex = D3DXVECTOR2(0.0f + 0.1f * g_anTime[nCntTime].nNm, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * g_anTime[nCntTime].nNm, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f + 0.1f * g_anTime[nCntTime].nNm, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * g_anTime[nCntTime].nNm, 1.0f);

		pVtx += 4;
	}
	// 頂点データをアンロックする
	g_pVtxBuffTime->Unlock();

	if (g_nTime % 60 == 0 && g_nTime > 60)
	{
		//PlaySound(SOUND_LABEL_SE_COUNT);
	}
}