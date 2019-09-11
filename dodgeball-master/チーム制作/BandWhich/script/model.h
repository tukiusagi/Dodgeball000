//=============================================================================
//
// モデル処理 [model.h]
// Author : YUKI TAKAHASHI
//
//=============================================================================
#ifndef _MODEL_H_
#define _MODEL_H_

#define _CRT_SECURE_NO_WARNINGS

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// 構造体定義
//=============================================================================

//=============================================================================
// モデルの種類
//=============================================================================
typedef enum
{
	MODELTYPE_BILL0 = 0,		// ビル0			[0]
	MODELTYPE_BILL1,			// ビル1			[1]
	MODELTYPE_BILL2,			// ビル2			[2]
	MODELTYPE_BILL3,			// ビル3			[3]
	MODELTYPE_BILL4,			// ビル4			[4]
	MODELTYPE_10_BOARD,			// 10万円看板		[5]
	MODELTYPE_30_BOARD,			// 30万円看板		[6]
	MODELTYPE_50_BOARD,			// 50万円看板		[7]
	MODELTYPE_100_BOARD,		// 100万円看板		[8]
	MODELTYPE_CLIFF,			// 崖				[9]
	MODELTYPE_CONVENI,			// コンビニ			[10]
	MODELTYPE_WAC,				// ワック			[11]
	MODELTYPE_MAX				// 種類の最大数
} MODELTYPE;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void InitModel(void);
void UninitModel(void);
void UpdateModel(void);
void DrawModel(void);

void SetModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, MODELTYPE type);
bool CollisionModel(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pPosOld, D3DXVECTOR3 size);
//bool CollisionModelCollide(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pPosOld, D3DXVECTOR3 size);

D3DXVECTOR3 *GetModelPos(void);

#endif
