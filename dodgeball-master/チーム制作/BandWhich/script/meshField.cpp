//=============================================================================
//
// ポリゴン処理 [meshField1.cpp]
// Author:RYO KANDA
//
//=============================================================================
#include "meshField.h"
#include "joypad.h"

//=============================================================================
// 構造体
//=============================================================================
typedef struct
{
	D3DXVECTOR3		pos;						// 四角形の位置

} POS;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void MakeVertexMeshField(LPDIRECT3DDEVICE9 pDevice);

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9	g_pTextureMeshField1 = NULL;
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffMeshField1 = NULL;
LPDIRECT3DINDEXBUFFER9	g_pIdxBuffMeshField1 = NULL;

D3DXVECTOR3	g_posMeshField1;			// ポリゴンの位置
D3DXVECTOR3	g_rotMeshField1;			// ポリゴンの向き(回転)
D3DXMATRIX	g_mtxWorldMeshField1;		// ワールドマトリックス

int	g_nNumVertexMeshField1;				// 総頂点
int g_nNumIndexMeshField1;				// 総インデックス
int g_nNumPolygonMeshField1;			// 総ポリゴン

POS g_aPos[MESHFIELD_NUMLENGTH * MESHFIELD_NUMSIDE];

//=============================================================================
// 初期化処理
//=============================================================================
void InitMeshField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "data/TEXTURE/yukiField.jpg", &g_pTextureMeshField1);

	for (int nCount = 0; nCount < MESHFIELD_NUMLENGTH; nCount++)
	{
		for (int nCnt = 0; nCnt < MESHFIELD_NUMSIDE; nCnt++)
		{
			g_aPos[MESHFIELD_NUMSIDE * nCount + nCnt].pos = D3DXVECTOR3(MESHFIELD_SIZE * (nCnt - MESHFIELD_NUMSIDE / 2.0f), 0.0f, MESHFIELD_SIZE * (MESHFIELD_NUMLENGTH / 2.0f - nCount));
		}
	}

	// 位置・回転の初期設定
	g_posMeshField1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_rotMeshField1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	g_nNumVertexMeshField1 = (MESHFIELD_NUMLENGTH + 1) * (MESHFIELD_NUMSIDE + 1);
	g_nNumIndexMeshField1 = (MESHFIELD_NUMLENGTH - 1) * 2 + (MESHFIELD_NUMSIDE + 1) * 2 * MESHFIELD_NUMLENGTH;
	g_nNumPolygonMeshField1 = (MESHFIELD_NUMLENGTH - 1) * 4 + 2 * MESHFIELD_NUMLENGTH * MESHFIELD_NUMSIDE;

	// 頂点座標
	MakeVertexMeshField(pDevice);
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	// 頂点バッファの開放
	if (g_pVtxBuffMeshField1 != NULL)
	{
		g_pVtxBuffMeshField1->Release();
		g_pVtxBuffMeshField1 = NULL;
	}

	// インデックスの開放
	if (g_pIdxBuffMeshField1 != NULL)
	{
		g_pIdxBuffMeshField1->Release();
		g_pIdxBuffMeshField1 = NULL;
	}

	// テクスチャの開放
	if (g_pTextureMeshField1 != NULL)
	{
		g_pTextureMeshField1->Release();
		g_pTextureMeshField1 = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得

	D3DXMATRIX	mtxRot, mtxTrans;		// 計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldMeshField1);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotMeshField1.y, g_rotMeshField1.x, g_rotMeshField1.z);
	D3DXMatrixMultiply(&g_mtxWorldMeshField1, &g_mtxWorldMeshField1, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_posMeshField1.x, g_posMeshField1.y, g_posMeshField1.z);
	D3DXMatrixMultiply(&g_mtxWorldMeshField1, &g_mtxWorldMeshField1, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldMeshField1);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffMeshField1, 0, sizeof(VERTEX_3D));

	//インデックスバッファをデータストリームに設定
	pDevice->SetIndices(g_pIdxBuffMeshField1);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pTextureMeshField1);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_nNumVertexMeshField1, 0, g_nNumPolygonMeshField1);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void MakeVertexMeshField(LPDIRECT3DDEVICE9 pDevice)
{
	// 頂点情報の設定
	VERTEX_3D *pVtx;

	// 頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * g_nNumVertexMeshField1, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &g_pVtxBuffMeshField1, NULL);

	// 頂点データの範囲をロックし、頂点バッファのポインタを取得
	g_pVtxBuffMeshField1->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点の設定
	for (int nCount = 0; nCount < MESHFIELD_NUMLENGTH + 1; nCount++)
	{
		for (int nCnt = 0; nCnt < MESHFIELD_NUMSIDE + 1; nCnt++, pVtx++)
		{
			pVtx[0].pos = D3DXVECTOR3(MESHFIELD_SIZE * (nCnt - MESHFIELD_NUMSIDE / 2.0f), 0.0f, MESHFIELD_SIZE * (MESHFIELD_NUMLENGTH / 2.0f - nCount));
		
			//法線ベクトルの設定
			pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			//頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 赤、緑、青、アルファ値

			// テクスチャ(UV)座標
			pVtx[0].tex = D3DXVECTOR2(1.0f * nCnt, 1.0f * nCount);
		}
	}

	// 頂点データをアンロック
	g_pVtxBuffMeshField1->Unlock();


	// インデックスバッファを生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * g_nNumIndexMeshField1, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pIdxBuffMeshField1, NULL);

	// インデックスデータへのポインタ
	WORD *pIdx;

	// インデックスバッファをロックし、インデックスバッファのポインタを取得
	g_pIdxBuffMeshField1->Lock(0, 0, (void**)&pIdx, 0);

	// インデックスの設定
	for (int nCount = 0, nCnt = 0; nCount < g_nNumIndexMeshField1; nCount += 2)
	{
		if (nCount != 0 && (nCount / 2 - (nCnt / (MESHFIELD_NUMSIDE + 1) - 1)) % (MESHFIELD_NUMSIDE + 1) == 0 && nCnt % (MESHFIELD_NUMSIDE + 1) == 0)
		{// 縮退分
			pIdx[nCount] = nCnt - 1;
			pIdx[nCount + 1] = nCnt + 1 + MESHFIELD_NUMSIDE;
		}
		else
		{
			pIdx[nCount] = (MESHFIELD_NUMSIDE + 1) + nCnt;
			pIdx[nCount + 1] = nCnt;

			nCnt++;
		}
	}

	//頂点データをアンロック
	g_pIdxBuffMeshField1->Unlock();
}

//=============================================================================
// 床の色の変更
//=============================================================================
void ChangeColor(D3DXCOLOR col, int nNumber)
{
	VERTEX_3D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファのポインタを取得
	g_pVtxBuffMeshField1->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += nNumber + nNumber / MESHFIELD_NUMSIDE;

	pVtx[0].col = col;	// 赤、緑、青、アルファ値
	pVtx[1].col = col;	// 赤、緑、青、アルファ値

	pVtx += MESHFIELD_NUMSIDE + 1;

	pVtx[0].col = col;	// 赤、緑、青、アルファ値
	pVtx[1].col = col;	// 赤、緑、青、アルファ値

	// 頂点データをアンロック
	g_pVtxBuffMeshField1->Unlock();
}
