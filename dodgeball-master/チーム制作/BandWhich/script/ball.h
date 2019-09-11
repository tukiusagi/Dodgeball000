//=============================================================================
//
// ボール処理 [ball.h]
// Author : YONEYAMA TAKESHI
//
//=============================================================================
#ifndef _BALL_H_
#define _BALL_H_

#define _CRT_SECURE_NO_WARNINGS

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define BALL_RADIUS	(30)

//=============================================================================
// 構造体定義
//=============================================================================
typedef enum
{
	BALLSTATE_NORMAL = 0 ,
	BALLSTATE_HOLD,
	BALLSTATE_THROW,
	BALLSTATE_MAX
}BALLSTATE;

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	LPD3DXMESH		pMesh;
	LPD3DXBUFFER	pBuffMat;
	DWORD			nNumMat;
	D3DXMATRIX		mtxWorld;		//ワールドマトリックス
	D3DXVECTOR3		pos;			//位置
	D3DXVECTOR3		posOld;		
	D3DXVECTOR3		move;			//移動
	D3DXVECTOR3		rot;			//回転情報
	D3DXCOLOR		col;			//色
	int				nParent;		//親(持っているプレイヤー)
	int				nIdxShadow;		//影
	int				nDamage;		//与えるダメージ量
	float			fRadius;		//半径
	bool			bHold;			//持ってる
	BALLSTATE		state;			//ボールの状態
} BALL;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void InitBall(void);
void UninitBall(void);
void UpdateBall(void);
void DrawBall(void);

void ThrowBall(D3DXVECTOR3 move);
BALL *GetBall(void);

void CatchBall(D3DXVECTOR3 pos, D3DXVECTOR3 move, float fRadius);
bool CollisionBall(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pPosOld,float Radius);


#endif
