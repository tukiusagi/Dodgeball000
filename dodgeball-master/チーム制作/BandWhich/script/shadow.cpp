//=============================================================================
//
// 影処理 [shadow.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "shadow.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define TEXTUREFILE0	"data/TEXTURE/shadow000.jpg"			// 読み込むテクスチャ
#define MAX_TEXTURE		(1)										// テクスチャ数
#define MAX_SHADOW		(256)									// 影の最大数

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	D3DXVECTOR3	pos;						// 位置
	D3DXVECTOR3	rot;						// 回転
	D3DXMATRIX	mtxWorld;					// ワールドマトリックス
	bool		bUse;						// 使用しているか
} SHADOW;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void MakeVertexShadow(LPDIRECT3DDEVICE9 pDevice);

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9		g_apTextureShadow = NULL;				// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffShadow = NULL;				// 頂点バッファへのポインタ

SHADOW					g_aShadow[MAX_SHADOW];					// 影の情報

//=============================================================================
// 初期化処理
//=============================================================================
void InitShadow(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得

	// 影情報の初期設定
	for (int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		g_aShadow[nCntShadow].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aShadow[nCntShadow].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aShadow[nCntShadow].bUse = false;
	}
	// 頂点情報の作成
	MakeVertexShadow(pDevice);
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitShadow(void)
{
	// テクスチャの解放
	if (g_apTextureShadow != NULL)
	{
		g_apTextureShadow->Release();
		g_apTextureShadow = NULL;
	}

	// 頂点バッファの開放
	if (g_pVtxBuffShadow != NULL)
	{
		g_pVtxBuffShadow->Release();
		g_pVtxBuffShadow = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateShadow(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawShadow(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス
	SHADOW *pShadow;

	// レンダーステート(減算合成処理)
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);// Zファンクションの設定

	pShadow = &g_aShadow[0];
	for (int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++, pShadow++)
	{
		if (pShadow->bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aShadow[nCntShadow].mtxWorld);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aShadow[nCntShadow].rot.y,
				g_aShadow[nCntShadow].rot.x, g_aShadow[nCntShadow].rot.z);
			D3DXMatrixMultiply(&g_aShadow[nCntShadow].mtxWorld, &g_aShadow[nCntShadow].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aShadow[nCntShadow].pos.x,
				g_aShadow[nCntShadow].pos.y, g_aShadow[nCntShadow].pos.z);
			D3DXMatrixMultiply(&g_aShadow[nCntShadow].mtxWorld, &g_aShadow[nCntShadow].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aShadow[nCntShadow].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffShadow, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureShadow);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4 * nCntShadow, 2);
		}
	}
	// レンダーステート(通常ブレンド処理)
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);// Zファンクションの設定
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void MakeVertexShadow(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_SHADOW,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffShadow,
		NULL);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE0, &g_apTextureShadow);

	VERTEX_3D *pVtx;

	// 頂点情報の設定
	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffShadow->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		// 座標の設定
		pVtx[0].pos = pVtx[1].pos = pVtx[2].pos = pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 法線ベクトルの設定
		pVtx[0].nor = pVtx[1].nor = pVtx[2].nor = pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラーの設定
		pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;// 頂点データのポインタを4つ分進める
	}
	// 頂点データをアンロックする
	g_pVtxBuffShadow->Unlock();
}

//=============================================================================
// 影の作成
//=============================================================================
int SetShadow(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size)
{
	VERTEX_3D *pVtx;
	int nCntShadow;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffShadow->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		if (!g_aShadow[nCntShadow].bUse)
		{
			g_aShadow[nCntShadow].pos.x = pos.x;
			g_aShadow[nCntShadow].pos.z = pos.z;
			g_aShadow[nCntShadow].rot = rot;
			g_aShadow[nCntShadow].bUse = true;

			// 座標の設定＿上にあるほど影が大きくなる
			pVtx[0].pos = D3DXVECTOR3(-size.x - pos.y / 25, 0.1f, +size.z + pos.y / 25);
			pVtx[1].pos = D3DXVECTOR3(+size.x + pos.y / 25, 0.1f, +size.z + pos.y / 25);
			pVtx[2].pos = D3DXVECTOR3(-size.x - pos.y / 25, 0.1f, -size.z - pos.y / 25);
			pVtx[3].pos = D3DXVECTOR3(+size.x + pos.y / 25, 0.1f, -size.z - pos.y / 25);

			// 頂点カラーの設定＿上にあるほど影が薄くなる
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f - pos.y / 1000);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f - pos.y / 1000);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f - pos.y / 1000);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f - pos.y / 1000);

			break;
		}
		pVtx += 4;// 頂点データのポインタを4つ分進める
	}
	// 頂点データをアンロックする
	g_pVtxBuffShadow->Unlock();

	return nCntShadow;
}

//=============================================================================
// 影の位置を設定
//=============================================================================
void SetPositionShadow(int nIdxShadow, D3DXVECTOR3 pos)
{
	if (g_aShadow[nIdxShadow].bUse == false)
	{
		g_aShadow[nIdxShadow].bUse = true;
	}
	g_aShadow[nIdxShadow].pos.x = pos.x;
	g_aShadow[nIdxShadow].pos.y = pos.y + 0.01f;
	g_aShadow[nIdxShadow].pos.z = pos.z;
}

//=============================================================================
// 影を削除
//=============================================================================
void DeleteShadow(int nIdxShadow)
{
	g_aShadow[nIdxShadow].bUse = false;
}
