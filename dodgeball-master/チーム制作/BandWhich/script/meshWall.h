//=============================================================================
//
// メッシュウォール処理 [meshWall.h]
// Author : YUKI TAKAHASHI
//
//=============================================================================
#ifndef _MESHWALL_H_
#define _MESHWALL_H_

#define _CRT_SECURE_NO_WARNINGS

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuffer;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9	pIdxBuffer;	// インデックスバッファへのポインタ
	D3DXVECTOR3	pos;					// 位置
	D3DXVECTOR3	rot;					// 向き(回転)
	D3DXMATRIX	mtxWorld;				// ワールドマトリックス
	int nNumWidth;						// 横の分割数
	int nNumHeight;						// 縦の分割数
	int nNumPolygon;					// 総ポリゴン数
	int nNumVertex;						// 総頂点数
	int nNumIndex;						// 総インデックス数
	D3DXVECTOR3	sizePolygon;			// 1つあたりのポリゴンの大きさ
	D3DXVECTOR2 texPolygon;				// 1つあたりのテクスチャ座標
	bool bUse;							// 使用しているか
} MESHWALL;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void InitMeshWall(void);
void UninitMeshWall(void);
void UpdateMeshWall(void);
void DrawMeshWall(void);

#endif
