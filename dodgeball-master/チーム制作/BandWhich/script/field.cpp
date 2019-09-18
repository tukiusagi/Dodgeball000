//=============================================================================
//
// 壁処理 [field.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "field.h"

#include "keyboard.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define TEXTUREFILE0	"data/TEXTURE/.jpg"					// 読み込むテクスチャ1
#define MAX_TEXTURE		(1)										// テクスチャ数
#define MAX_FIELD		(128)									// 壁の数
#define FIELD_WIDTH		(500)									// 壁の幅
#define FIELD_HEIGHT	(500)									// 壁の高さ

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	D3DXVECTOR3	pos;		// 位置
	D3DXVECTOR3	rot;		// 向き(回転)
	D3DXMATRIX	mtxWorld;	// ワールドマトリックス
	bool bUse;				// 壁を使用しているか
} FIELD;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void MakeVertexField(LPDIRECT3DDEVICE9 pDevice);
void SetField(D3DXVECTOR3 pos, D3DXVECTOR3 rot);

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9		g_apTextureField[MAX_TEXTURE] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffField = NULL;				// 頂点バッファへのポインタ

FIELD					g_aField[MAX_FIELD];				// 壁の情報
typedef enum
{
	RED,
	BLUE,
	YELLOW,
	GREEN,
	COLOR_MAX
}COLOR;
COLOR g_aColor[4];

//=============================================================================
// 初期化処理
//=============================================================================
void InitField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得

	// 初期設定
	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++)
	{
		g_aField[nCntField].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aField[nCntField].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aField[nCntField].bUse = false;
	}

	// 頂点情報の作成
	MakeVertexField(pDevice);

	// 壁の設置
	SetField(D3DXVECTOR3(-250.0f, 0.0f, 250.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	SetField(D3DXVECTOR3(250.0f, 0.0f, 250.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	SetField(D3DXVECTOR3(-250.0f, 0.0f, -250.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	SetField(D3DXVECTOR3(250.0f, 0.0f, -250.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitField(void)
{
	// テクスチャの開放
	for (int nCntTex = 0; nCntTex < MAX_TEXTURE; nCntTex++)
	{
		if (g_apTextureField[nCntTex] != NULL)
		{
			g_apTextureField[nCntTex]->Release();
			g_apTextureField[nCntTex] = NULL;
		}
	}

	// 頂点バッファの開放
	if (g_pVtxBuffField != NULL)
	{
		g_pVtxBuffField->Release();
		g_pVtxBuffField = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateField(void)
{
	// 色の変更処理
	// 赤
	if (GetKeyboardTrigger(DIK_R))
	{
		ChangeFieldColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 0);
	}
	if (GetKeyboardRelease(DIK_R))
	{
		ChangeFieldColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 0);
	}
	// 青
	if (GetKeyboardTrigger(DIK_B))
	{
		ChangeFieldColor(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f), 1);
	}
	if (GetKeyboardRelease(DIK_B))
	{
		ChangeFieldColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1);
	}
	// 黄色
	if (GetKeyboardTrigger(DIK_Y))
	{
		ChangeFieldColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f), 2);
	}
	if (GetKeyboardRelease(DIK_Y))
	{
		ChangeFieldColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 2);
	}
	// 緑
	if (GetKeyboardTrigger(DIK_G))
	{
		ChangeFieldColor(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), 3);
	}
	if (GetKeyboardRelease(DIK_G))
	{
		ChangeFieldColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 3);
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス
	FIELD *pField = &g_aField[0];

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++, pField++)
	{
		if (!pField->bUse) { continue; }
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&pField->mtxWorld);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, pField->rot.y, pField->rot.x, pField->rot.z);
		D3DXMatrixMultiply(&pField->mtxWorld, &pField->mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, pField->pos.x, pField->pos.y, pField->pos.z);
		D3DXMatrixMultiply(&pField->mtxWorld, &pField->mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pField->mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffField, 0, sizeof(VERTEX_3D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureField[0]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntField * 4, 2);
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void MakeVertexField(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_FIELD,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffField,
		NULL);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE0, &g_apTextureField[0]);

	VERTEX_3D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++, pVtx += 4)
	{
		// 座標の設定
		pVtx[0].pos = pVtx[1].pos = pVtx[2].pos = pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 法線ベクトルの設定
		pVtx[0].nor = pVtx[1].nor = pVtx[2].nor = pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// 頂点カラーの設定
		pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	// 頂点データをアンロックする
	g_pVtxBuffField->Unlock();
}

//=============================================================================
// 壁の設定
//=============================================================================
void SetField(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	VERTEX_3D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++, pVtx += 4)
	{
		if (g_aField[nCntField].bUse) { continue; }
		g_aField[nCntField].pos = pos;
		g_aField[nCntField].rot = rot;
		g_aField[nCntField].bUse = true;

		// 座標の設定
		pVtx[0].pos = D3DXVECTOR3(-FIELD_WIDTH / 2, 0.0f, +FIELD_HEIGHT/2);
		pVtx[1].pos = D3DXVECTOR3(+FIELD_WIDTH / 2, 0.0f, +FIELD_HEIGHT/2);
		pVtx[2].pos = D3DXVECTOR3(-FIELD_WIDTH / 2, 0.0f, -FIELD_HEIGHT/2);
		pVtx[3].pos = D3DXVECTOR3(+FIELD_WIDTH / 2, 0.0f, -FIELD_HEIGHT/2);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(3.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(3.0f, 1.0f);
		break;
	}
	// 頂点データをアンロックする
	g_pVtxBuffField->Unlock();
}

//=============================================================================
// 床の色の変更
//=============================================================================
void ChangeFieldColor(D3DXCOLOR col, int nNumber)
{
	VERTEX_3D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファのポインタを取得
	g_pVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += nNumber * 4;

	pVtx[0].col = pVtx[1].col = pVtx[2].col = pVtx[3].col = col;

	// 頂点データをアンロック
	g_pVtxBuffField->Unlock();
}
