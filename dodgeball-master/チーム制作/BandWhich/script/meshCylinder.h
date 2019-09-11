//=============================================================================
//
// メッシュコライダー処理 [meshCylinder.h]
// Author : YUKI TAKAHASHI
//
//=============================================================================
#ifndef _MESHCYLINDER_H_
#define _MESHCYLINDER_H_

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
	D3DXVECTOR3	pos;		// 位置
	D3DXVECTOR3	rot;		// 向き(回転)
	D3DXMATRIX	mtxWorld;	// ワールドマトリックス
	float fRadius;			// 半径
	int nNumWidth;			// 横の分割数
	int nNumHeight;			// 縦の分割数
	int nNumPolygon;		// 総ポリゴン数
	int nNumVertex;			// 総頂点数
	int nNumIndex;			// 総インデックス数
	//D3DXVECTOR3	sizePolygon;// 1つあたりのポリゴンの大きさ
	D3DXVECTOR2 texPolygon;	// 1つあたりのテクスチャ座標
	float fAngle;			// 1頂点当たりの角度
} MESHCYLINDER;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void InitMeshCylinder(void);
void UninitMeshCylinder(void);
void UpdateMeshCylinder(void);
void DrawMeshCylinder(void);

#endif
