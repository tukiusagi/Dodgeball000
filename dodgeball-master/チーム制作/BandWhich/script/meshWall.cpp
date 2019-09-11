//=============================================================================
//
// メッシュウォール処理 [meshWall.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "meshWall.h"
#include "camera.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define TEXTUREFILE0	"data/TEXTURE/field000.jpg"				// 読み込むテクスチャ
#define MAX_TEXTURE		(1)										// テクスチャ数
#define POLYFON_WIDTH	(1900)									// 壁の幅
#define POLYGON_HEIGHT	(100)									// 壁の高さ
#define MAX_WALL		(4)										// 壁の数

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void MakeVertexMeshWall(LPDIRECT3DDEVICE9 pDevice);
void SetMeshWall(D3DXVECTOR3 pos, D3DXVECTOR3 rot);

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9	g_apTextureMeshWall[MAX_TEXTURE] = {};	// テクスチャへのポインタ

MESHWALL			g_MeshWall[MAX_WALL];					// メッシュウォールの情報

//=============================================================================
// 初期化処理
//=============================================================================
void InitMeshWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE0, &g_apTextureMeshWall[0]);

	// 初期設定
	g_MeshWall[0].pos = D3DXVECTOR3(0.0f, 0.0f, 950.0f);
	g_MeshWall[0].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_MeshWall[0].nNumWidth = 40;
	g_MeshWall[0].nNumHeight = 40;

	g_MeshWall[1].pos = D3DXVECTOR3(950.0f, 0.0f, 0.0f);
	g_MeshWall[1].rot = D3DXVECTOR3(0.0f, D3DX_PI / 2, 0.0f);
	g_MeshWall[1].nNumWidth = 40;
	g_MeshWall[1].nNumHeight = 40;

	g_MeshWall[2].pos = D3DXVECTOR3(-950.0f, 0.0f, 0.0f);
	g_MeshWall[2].rot = D3DXVECTOR3(0.0f, -D3DX_PI / 2, 0.0f);
	g_MeshWall[2].nNumWidth = 40;
	g_MeshWall[2].nNumHeight = 40;

	g_MeshWall[3].pos = D3DXVECTOR3(0.0f, 0.0f, -950.0f);
	g_MeshWall[3].rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);
	g_MeshWall[3].nNumWidth = 40;
	g_MeshWall[3].nNumHeight = 40;

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		// 総頂点数
		g_MeshWall[nCntWall].nNumVertex = (g_MeshWall[nCntWall].nNumWidth + 1)*(g_MeshWall[nCntWall].nNumHeight + 1);

		// 総ポリゴン数
		g_MeshWall[nCntWall].nNumPolygon = (g_MeshWall[nCntWall].nNumWidth*g_MeshWall[nCntWall].nNumHeight * 2) + ((g_MeshWall[nCntWall].nNumHeight - 1) * 4);

		// 総インデックス数
		g_MeshWall[nCntWall].nNumIndex = g_MeshWall[nCntWall].nNumPolygon + 2;

		// 1つあたりのポリゴンの大きさ
		g_MeshWall[nCntWall].sizePolygon = D3DXVECTOR3(1.0f*POLYFON_WIDTH / g_MeshWall[nCntWall].nNumWidth, 0.0f,
			1.0f*POLYGON_HEIGHT / g_MeshWall[nCntWall].nNumHeight);

		// 1つあたりのテクスチャ座標
		g_MeshWall[nCntWall].texPolygon = D3DXVECTOR2(10.0f / g_MeshWall[nCntWall].nNumWidth, 1.0f / g_MeshWall[nCntWall].nNumHeight);
	}
	// 頂点情報の作成
	MakeVertexMeshWall(pDevice);

	// 壁の設置
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		SetMeshWall(g_MeshWall[nCntWall].pos, g_MeshWall[nCntWall].rot);
	}
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshWall(void)
{
	// テクスチャの開放
	if (g_apTextureMeshWall[0] != NULL)
	{
		g_apTextureMeshWall[0]->Release();
		g_apTextureMeshWall[0] = NULL;
	}

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		// インデックスバッファの解放
		if (g_MeshWall[nCntWall].pIdxBuffer)
		{
			g_MeshWall[nCntWall].pIdxBuffer->Release();
			g_MeshWall[nCntWall].pIdxBuffer = NULL;
		}

		// 頂点バッファの開放
		if (g_MeshWall[nCntWall].pVtxBuffer != NULL)
		{
			g_MeshWall[nCntWall].pVtxBuffer->Release();
			g_MeshWall[nCntWall].pVtxBuffer = NULL;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshWall(void)
{
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス
	MESHWALL *pMeshWall;						// メッシュウォール情報へのポインタ

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);			// 両面カリングする

	pMeshWall = &g_MeshWall[0];
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++,pMeshWall++)
	{
		if (pMeshWall->bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_MeshWall[nCntWall].mtxWorld);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_MeshWall[nCntWall].rot.y, g_MeshWall[nCntWall].rot.x, g_MeshWall[nCntWall].rot.z);
			D3DXMatrixMultiply(&g_MeshWall[nCntWall].mtxWorld, &g_MeshWall[nCntWall].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_MeshWall[nCntWall].pos.x, g_MeshWall[nCntWall].pos.y, g_MeshWall[nCntWall].pos.z);
			D3DXMatrixMultiply(&g_MeshWall[nCntWall].mtxWorld, &g_MeshWall[nCntWall].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_MeshWall[nCntWall].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_MeshWall[nCntWall].pVtxBuffer, 0, sizeof(VERTEX_3D));

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(g_MeshWall[nCntWall].pIdxBuffer);

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureMeshWall[0]);

			// ポリゴンの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_MeshWall[nCntWall].nNumVertex, 0, g_MeshWall[nCntWall].nNumPolygon);
		}
	}

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);			// カリングを元に戻す
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void MakeVertexMeshWall(LPDIRECT3DDEVICE9 pDevice)
{
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		// オブジェクトの頂点バッファを生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * g_MeshWall[nCntWall].nNumVertex,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_MeshWall[nCntWall].pVtxBuffer,
			NULL);

		// インデックスバッファを生成
		pDevice->CreateIndexBuffer(sizeof(WORD) * g_MeshWall[nCntWall].nNumIndex,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&g_MeshWall[nCntWall].pIdxBuffer,
			NULL);

		VERTEX_3D *pVtx;

		// 頂点情報の設定
		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_MeshWall[nCntWall].pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntV = 0; nCntV < g_MeshWall[nCntWall].nNumHeight + 1; nCntV++)
		{
			for (int nCntH = 0; nCntH < g_MeshWall[nCntWall].nNumWidth + 1; nCntH++)
			{
				// 頂点座標の設定
				pVtx[nCntH + nCntV * (g_MeshWall[nCntWall].nNumWidth + 1)].pos =
					D3DXVECTOR3(-POLYFON_WIDTH / 2 + g_MeshWall[nCntWall].sizePolygon.x*nCntH,
					POLYGON_HEIGHT - g_MeshWall[nCntWall].sizePolygon.z*nCntV, 0.0f);

				// 法線ベクトルの設定
				pVtx[nCntH + nCntV * (g_MeshWall[nCntWall].nNumWidth + 1)].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

				// 頂点カラーの設定
				pVtx[nCntH + nCntV * (g_MeshWall[nCntWall].nNumWidth + 1)].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ座標の設定
				pVtx[nCntH + nCntV * (g_MeshWall[nCntWall].nNumWidth + 1)].tex = D3DXVECTOR2(g_MeshWall[nCntWall].texPolygon.x*nCntH,
					g_MeshWall[nCntWall].texPolygon.y*nCntV);
			}
		}
		// 頂点データをアンロックする
		g_MeshWall[nCntWall].pVtxBuffer->Unlock();

		WORD *pIdx;// インデックスデータへのポインタ

		// インデックスの設定
		// インデックスバッファをロックし、インデックスデータへのポインタを取得
		g_MeshWall[nCntWall].pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

		int nNum = 0;
		int nNumCnt = 0;

		// インデックス
		for (int nCntV = 0; nCntV < g_MeshWall[nCntWall].nNumHeight; nCntV++)
		{
			if (nCntV != 0)
			{
				pIdx[nNum] = nNumCnt - 1 + g_MeshWall[nCntWall].nNumWidth - 1 + 3;
				nNum++;
			}

			for (int nCntH = 0; nCntH < g_MeshWall[nCntWall].nNumWidth + 1; nCntH++, nNumCnt++, nNum += 2)
			{
				pIdx[nNum] = nNumCnt + g_MeshWall[nCntWall].nNumWidth + 1;
				pIdx[nNum + 1] = nNumCnt;
			}

			if (nCntV != g_MeshWall[nCntWall].nNumHeight - 1)
			{
				pIdx[nNum] = nNumCnt - 1;
				nNum++;
			}
		}
		// インデックスバッファをアンロックする
		g_MeshWall[nCntWall].pIdxBuffer->Unlock();
	}
}

//=============================================================================
// メッシュウォールの設定
//=============================================================================
void SetMeshWall(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		if (!g_MeshWall[nCntWall].bUse)
		{
			g_MeshWall[nCntWall].pos = pos;
			g_MeshWall[nCntWall].rot = rot;
			g_MeshWall[nCntWall].bUse = true;
			break;
		}
	}
}