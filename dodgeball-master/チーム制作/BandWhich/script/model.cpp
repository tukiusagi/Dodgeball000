//=============================================================================
//
// モデル処理 [model.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "model.h"
#include "camera.h"
#include "shadow.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MODELFILE0		"data/MODEL/bill_00.x"					// 読み込むモデル
#define MODELFILE1		"data/MODEL/bill_01.x"					// 読み込むモデル
#define MODELFILE2		"data/MODEL/bill_02.x"					// 読み込むモデル
#define MODELFILE3		"data/MODEL/bill_03.x"					// 読み込むモデル
#define MODELFILE4		"data/MODEL/bill_04.x"					// 読み込むモデル

#define MODELFILE5		"data/MODEL/10万円.x"					// 読み込むモデル
#define MODELFILE6		"data/MODEL/30万円.x"					// 読み込むモデル
#define MODELFILE7		"data/MODEL/50万円.x"					// 読み込むモデル
#define MODELFILE8		"data/MODEL/100万円.x"					// 読み込むモデル

#define MODELFILE9		"data/MODEL/tikinnresu.x"				// 読み込むモデル

#define MODELFILE10		"data/MODEL/mart.x"						// 読み込むモデル
#define MODELFILE11		"data/MODEL/Wac.x"						// 読み込むモデル

#define TEXTUREFILE0	"data/TEXTURE/bill_window.png"			// 読み込むテクスチャ
#define TEXTUREFILE1	"data/TEXTURE/auto_door.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE2	"data/TEXTURE/bill_rooftop.png"			// 読み込むテクスチャ
#define TEXTUREFILE3	"data/TEXTURE/bill_window1.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE4	"data/TEXTURE/bill_window2.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE5	"data/TEXTURE/SH028_L.jpg"				// 読み込むテクスチャ
#define TEXTUREFILE6	"data/TEXTURE/SH033_L.jpg"				// 読み込むテクスチャ
#define TEXTUREFILE7	"data/TEXTURE/bill_window3.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE8	"data/TEXTURE/bill_window4.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE9	"data/TEXTURE/bill_wall.png"			// 読み込むテクスチャ
#define TEXTUREFILE10	"data/TEXTURE/Signboard.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE11	"data/TEXTURE/Signboard10.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE12	"data/TEXTURE/Signboard30.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE13	"data/TEXTURE/Signboard50.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE14	"data/TEXTURE/Signboard100.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE15	"data/TEXTURE/chicken_race0.jpg"		// 読み込むテクスチャ
#define TEXTUREFILE16	"data/TEXTURE/chicken_race1.jpg"		// 読み込むテクスチャ
#define TEXTUREFILE17	"data/TEXTURE/convenience0.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE18	"data/TEXTURE/convenience1.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE19	"data/TEXTURE/conveni_signboard.jpg"	// 読み込むテクスチャ
#define MAX_TEXTURE		(21)									// テクスチャ数
#define MAX_SET_TEXTURE	(20)									// 設置テクスチャ数
#define MAX_MODEL		(128)									// モデルの数

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	D3DXVECTOR3		pos;						// 位置
	D3DXVECTOR3		move;						// 移動量
	D3DXVECTOR3		rot;						// 向き(回転)
	D3DXMATRIX		mtxWorld;					// ワールドマトリックス
	bool			bUse;						// 使用しているか
	int				nIdxShadow;					// 影のID
	MODELTYPE		type;						// 種類
} MODEL;

typedef struct
{
	DWORD			nNumMat;					// マテリアル情報の数
	LPD3DXMESH		pMesh;						// メッシュ情報へのポインタ
	LPD3DXBUFFER	pBuffMat;					// マテリアル情報へのポインタ
	D3DXVECTOR3		vtxMin;						// 最大値
	D3DXVECTOR3		vtxMax;						// 最小値
} MODEL_MODEL;

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9	g_pTextureModel[MAX_TEXTURE] = {};			// テクスチャへのポインタ

MODEL				g_aModel[MAX_MODEL];						// プレイヤーの情報			
MODEL_MODEL			g_aModelModel[MODELTYPE_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
void InitModel(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 初期設定
	for (int nCntEnemy = 0; nCntEnemy < MAX_MODEL; nCntEnemy++)
	{
		g_aModel[nCntEnemy].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aModel[nCntEnemy].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aModel[nCntEnemy].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aModel[nCntEnemy].bUse = false;
		g_aModel[nCntEnemy].type = MODELTYPE_BILL0;
	}

	// Xファイルの読み込み
	D3DXLoadMeshFromX(MODELFILE0, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[0].pBuffMat, NULL,
		&g_aModelModel[0].nNumMat,
		&g_aModelModel[0].pMesh);

	D3DXLoadMeshFromX(MODELFILE1, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[1].pBuffMat, NULL,
		&g_aModelModel[1].nNumMat,
		&g_aModelModel[1].pMesh);

	D3DXLoadMeshFromX(MODELFILE2, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[2].pBuffMat, NULL,
		&g_aModelModel[2].nNumMat,
		&g_aModelModel[2].pMesh);

	D3DXLoadMeshFromX(MODELFILE3, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[3].pBuffMat, NULL,
		&g_aModelModel[3].nNumMat,
		&g_aModelModel[3].pMesh);

	D3DXLoadMeshFromX(MODELFILE4, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[4].pBuffMat, NULL,
		&g_aModelModel[4].nNumMat,
		&g_aModelModel[4].pMesh);

	D3DXLoadMeshFromX(MODELFILE5, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[5].pBuffMat, NULL,
		&g_aModelModel[5].nNumMat,
		&g_aModelModel[5].pMesh);

	D3DXLoadMeshFromX(MODELFILE6, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[6].pBuffMat, NULL,
		&g_aModelModel[6].nNumMat,
		&g_aModelModel[6].pMesh);

	D3DXLoadMeshFromX(MODELFILE7, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[7].pBuffMat, NULL,
		&g_aModelModel[7].nNumMat,
		&g_aModelModel[7].pMesh);

	D3DXLoadMeshFromX(MODELFILE8, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[8].pBuffMat, NULL,
		&g_aModelModel[8].nNumMat,
		&g_aModelModel[8].pMesh);
	
	D3DXLoadMeshFromX(MODELFILE9, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[9].pBuffMat, NULL,
		&g_aModelModel[9].nNumMat,
		&g_aModelModel[9].pMesh);

	D3DXLoadMeshFromX(MODELFILE10, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[10].pBuffMat, NULL,
		&g_aModelModel[10].nNumMat,
		&g_aModelModel[10].pMesh);

	D3DXLoadMeshFromX(MODELFILE11, D3DXMESH_SYSTEMMEM, pDevice, NULL,
		&g_aModelModel[11].pBuffMat, NULL,
		&g_aModelModel[11].nNumMat,
		&g_aModelModel[11].pMesh);

	// モデルの頂点数の最大値・最小値の設定
	int		nNumVertices;							// 頂点数
	DWORD	sizeFVF;								// 頂点フォーマットのサイズ
	BYTE	*pVertexBuffer;

	for (int nCntModel = 0; nCntModel < MODELTYPE_MAX; nCntModel++)
	{
		g_aModelModel[nCntModel].vtxMax= D3DXVECTOR3(-1000.0f, -1000.0f, -1000.0f);
		g_aModelModel[nCntModel].vtxMin= D3DXVECTOR3(1000.0f, 1000.0f, 1000.0f);

		// 頂点数を取得
		nNumVertices = g_aModelModel[nCntModel].pMesh->GetNumVertices();

		// 頂点フォーマットのサイズを取得
		sizeFVF = D3DXGetFVFVertexSize(g_aModelModel[nCntModel].pMesh->GetFVF());

		// 頂点バッファをロック
		g_aModelModel[nCntModel].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertexBuffer);

		for (int nCntVtx = 0; nCntVtx < nNumVertices; nCntVtx++)
		{
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVertexBuffer;

			// 全ての頂点情報を比較して最大値・最小値を抜き出す
			if (g_aModelModel[nCntModel].vtxMax.x < vtx.x)
			{
				g_aModelModel[nCntModel].vtxMax.x = vtx.x;
			}
			if (g_aModelModel[nCntModel].vtxMin.x > vtx.x)
			{
				g_aModelModel[nCntModel].vtxMin.x = vtx.x;
			}

			if (g_aModelModel[nCntModel].vtxMax.y < vtx.y)
			{
				g_aModelModel[nCntModel].vtxMax.y = vtx.y;
			}
			if (g_aModelModel[nCntModel].vtxMin.y > vtx.y)
			{
				g_aModelModel[nCntModel].vtxMin.y = vtx.y;
			}

			if (g_aModelModel[nCntModel].vtxMax.z < vtx.z)
			{
				g_aModelModel[nCntModel].vtxMax.z = vtx.z;
			}
			if (g_aModelModel[nCntModel].vtxMin.z > vtx.z)
			{
				g_aModelModel[nCntModel].vtxMin.z = vtx.z;
			}
			pVertexBuffer += sizeFVF;//	サイズ分ポインタを進める
		}
		// アンロック
		g_aModelModel[nCntModel].pMesh->UnlockVertexBuffer();
	}

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE0, &g_pTextureModel[0]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE1, &g_pTextureModel[1]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE2, &g_pTextureModel[2]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE3, &g_pTextureModel[3]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE4, &g_pTextureModel[4]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE5, &g_pTextureModel[5]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE6, &g_pTextureModel[6]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE7, &g_pTextureModel[7]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE8, &g_pTextureModel[8]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE9, &g_pTextureModel[9]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE10, &g_pTextureModel[10]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE11, &g_pTextureModel[11]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE12, &g_pTextureModel[12]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE13, &g_pTextureModel[13]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE14, &g_pTextureModel[14]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE15, &g_pTextureModel[15]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE16, &g_pTextureModel[16]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE17, &g_pTextureModel[17]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE18, &g_pTextureModel[18]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE19, &g_pTextureModel[19]);
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitModel(void)
{
	// メッシュの開放
	for (int nCntMesh = 0; nCntMesh < MODELTYPE_MAX; nCntMesh++)
	{
		if (g_aModelModel[nCntMesh].pMesh != NULL)
		{
			g_aModelModel[nCntMesh].pMesh->Release();
			g_aModelModel[nCntMesh].pMesh = NULL;
		}
	}
	
	// マテリアルの開放
	for (int nCntMat = 0; nCntMat < MODELTYPE_MAX; nCntMat++)
	{
		if (g_aModelModel[nCntMat].pBuffMat != NULL)
		{
			g_aModelModel[nCntMat].pBuffMat->Release();
			g_aModelModel[nCntMat].pBuffMat = NULL;
		}
	}
	
	// テクスチャの開放
	for (int nCntTex = 0; nCntTex < MAX_SET_TEXTURE; nCntTex++)
	{
		if (g_pTextureModel[nCntTex] != NULL)
		{
			g_pTextureModel[nCntTex]->Release();
			g_pTextureModel[nCntTex] = NULL;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateModel(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawModel(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス
	D3DXMATERIAL *pMat;							// 現在のマテリアル保存用
	D3DMATERIAL9 matDef;						// マテリアルデータへのポインタ
	MODEL *pModel;

	pModel = &g_aModel[0];
	for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++, pModel++)
	{
		if (pModel->bUse)// 使用されている場合
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aModel[nCntModel].mtxWorld);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aModel[nCntModel].rot.y, g_aModel[nCntModel].rot.x, g_aModel[nCntModel].rot.z);
			D3DXMatrixMultiply(&g_aModel[nCntModel].mtxWorld, &g_aModel[nCntModel].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aModel[nCntModel].pos.x, g_aModel[nCntModel].pos.y, g_aModel[nCntModel].pos.z);
			D3DXMatrixMultiply(&g_aModel[nCntModel].mtxWorld, &g_aModel[nCntModel].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aModel[nCntModel].mtxWorld);
	
			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアル情報に対するポインタを取得
			pMat = (D3DXMATERIAL*)g_aModelModel[g_aModel[nCntModel].type].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aModelModel[g_aModel[nCntModel].type].nNumMat; nCntMat++)
			{// テクスチャの設定
				switch (g_aModel[nCntModel].type)
				{
				case MODELTYPE_BILL0:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[0]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[1]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[2]);
					}
					break;

				case MODELTYPE_BILL1:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[3]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[1]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[2]);
					}
					break;

				case MODELTYPE_BILL2:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[4]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[5]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[6]);
					}
					break;

				case MODELTYPE_BILL3:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[7]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[1]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[2]);
					}
					break;

				case MODELTYPE_BILL4:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[8]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[9]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[2]);
					}
					break;

				case MODELTYPE_10_BOARD:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[17]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[10]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[11]);
					}
					break;

				case MODELTYPE_30_BOARD:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[17]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[10]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[12]);
					}
					break;

				case MODELTYPE_50_BOARD:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[17]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[10]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[13]);
					}
					break;

				case MODELTYPE_100_BOARD:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[17]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[10]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[14]);
					}
					break;

				case MODELTYPE_CLIFF:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[15]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[16]);
					}
					break;

				case MODELTYPE_CONVENI:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[1]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[17]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[18]);
					}
					else if (nCntMat == 3)
					{
						pDevice->SetTexture(0, g_pTextureModel[19]);
					}
					else if (nCntMat == 4)
					{
						// テクスチャの設定(
						pDevice->SetTexture(0, g_pTextureModel[20]);
					}
					break;

				case MODELTYPE_WAC:
					if (nCntMat == 0)
					{
						pDevice->SetTexture(0, g_pTextureModel[0]);
					}
					else if (nCntMat == 1)
					{
						pDevice->SetTexture(0, g_pTextureModel[1]);
					}
					else if (nCntMat == 2)
					{
						pDevice->SetTexture(0, g_pTextureModel[18]);
					}
					else if (nCntMat == 3)
					{
						pDevice->SetTexture(0, g_pTextureModel[2]);
					}
					else if (nCntMat == 4)
					{
						// テクスチャの設定(
						pDevice->SetTexture(0, g_pTextureModel[20]);
					}
					break;
				}
				
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// 描画
				g_aModelModel[g_aModel[nCntModel].type].pMesh->DrawSubset(nCntMat);
			}
			// マテリアルをデフォルトに戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//=============================================================================
// モデルの設定
//=============================================================================
void SetModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, MODELTYPE type)
{
	for (int nCntEnemy = 0; nCntEnemy < MAX_MODEL; nCntEnemy++)
	{
		if (!g_aModel[nCntEnemy].bUse)// 敵が使用されていない場合
		{
			g_aModel[nCntEnemy].pos = pos;
			g_aModel[nCntEnemy].rot = rot;
			g_aModel[nCntEnemy].type = type;
			g_aModel[nCntEnemy].bUse = true;

			//// 影の作成
			//g_aModel[nCntEnemy].nIdxShadow = SetShadow(g_aModel[nCntEnemy].pos, g_aModel[nCntEnemy].rot,
			//	D3DXVECTOR3(g_aModelModel[g_aModel[nCntEnemy].type].vtxMax.x- g_aModelModel[g_aModel[nCntEnemy].type].vtxMin.x,
			//		0.0f, g_aModelModel[g_aModel[nCntEnemy].type].vtxMax.z - g_aModelModel[g_aModel[nCntEnemy].type].vtxMin.z));

			break;
		}
	}
}

//=============================================================================
// モデルとの当たり判定
//=============================================================================
bool CollisionModel(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pPosOld, D3DXVECTOR3 size)
{
	bool bRand = false;// 着地判定

	// 当たり判定
	for (int nCountBlock = 0; nCountBlock < MAX_MODEL; nCountBlock++)
	{
		if (g_aModel[nCountBlock].bUse)// ブロックが使用されている場合
		{
			if (pPos->y <= g_aModel[nCountBlock].pos.y + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.y&&
				pPosOld->y >= g_aModel[nCountBlock].pos.y + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.y)
			{// 乗っているか
				if (pPos->x + size.x > g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.x&&
					pPos->x - size.x < g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.x&&
					pPos->z + size.z > g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.z&&
					pPos->z - size.z < g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.z)
				{// X,Z座標が範囲内の場合
					pPos->y = g_aModel[nCountBlock].pos.y + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.y;
					bRand = true;
				}
			}
			else if (pPos->y + size.y > g_aModel[nCountBlock].pos.y + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.y &&
				pPos->y < g_aModel[nCountBlock].pos.y + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.y)
			{// Y座標が範囲内の場合
				if (pPos->x + size.x > g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.x&&
					pPos->x - size.x < g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.x)
				{// 上下
					if (pPos->z - size.z <= g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.z&&
						pPosOld->z - size.z >= g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.z)
					{// プレイヤーがブロックに上からぶつかって
						pPos->z = size.z + g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.z;
					}
					else if (pPos->z + size.z >= g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.z&&
						pPosOld->z + size.z <= g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.z)
					{// プレイヤーがブロックに下からぶつかって
						pPos->z = g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.z - size.z;
					}
				}

				if (pPos->z + size.z > g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.z&&
					pPos->z - size.z < g_aModel[nCountBlock].pos.z + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.z)
				{// 左右
					if (pPos->x - size.x <= g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.x&&
						pPosOld->x - size.x >= g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.x)
					{// プレイヤーがブロックに右からぶつかって
						pPos->x = size.x + g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMax.x;
					}
					else if (pPos->x + size.x >= g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.x&&
						pPosOld->x + size.x <= g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.x)
					{// プレイヤーがブロックに左からぶつかって
						pPos->x = g_aModel[nCountBlock].pos.x + g_aModelModel[g_aModel[nCountBlock].type].vtxMin.x - size.x;
					}
				}
			}
		}
	}

	return bRand;
}

//=============================================================================
// モデルの場所情報の取得処理
//=============================================================================
D3DXVECTOR3 *GetModelPos(void)
{
	return&g_aModel[0].pos;
}