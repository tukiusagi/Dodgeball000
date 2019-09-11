//=============================================================================
//
// カメラ処理 [camera.h]
// Author : YUKI TAKAHASHI
//
//=============================================================================
#ifndef _CAMERA_H_
#define _CAMERA_H_

#define _CRT_SECURE_NO_WARNINGS

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// カメラの構造体
//=============================================================================
typedef struct
{
	D3DXVECTOR3 posV;			// 現在の視点
	D3DXVECTOR3 posVDest;		// 目的の視点
	D3DXVECTOR3 posVOld;		// 前回の視点
	D3DXVECTOR3 posR;			// 現在の注視点(参照点)
	D3DXVECTOR3 posRDest;		// 目的の注視点(参照点)
	D3DXVECTOR3 posMove;		// 場所の移動量
	D3DXVECTOR3 vecU;			// 上方向ベクトル
	D3DXVECTOR3 rot;			// 現在の角度
	D3DXVECTOR3 rotOld;			// 前回の角度
	D3DXVECTOR3 rotDest;		// 目的の角度
	D3DXVECTOR3 rotDiff;		// 角度の差
	D3DXVECTOR3 rotMove;		// 向きの移動量
	float		fDistance;		// 距離
	bool		bWraparound;	// 回り込みしているか
	D3DXMATRIX	mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX	mtxView;		// ビューマトリックス
} CAMERA;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);

void SetCamera(int nCount);
void SetCameraPosR(D3DXVECTOR3 pos, int nCnt);
void SetCameraPosV(D3DXVECTOR3 pos, int nCnt);
void SetCameraRot(D3DXVECTOR3 rot, int nCnt);
void RotMoveCameraR(float rotY, int nCnt);
void RotMoveCameraV(float rotY, int nCnt);

CAMERA *GetCamera(int nNum);

#endif
