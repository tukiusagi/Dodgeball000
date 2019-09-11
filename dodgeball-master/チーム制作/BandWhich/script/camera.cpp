//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "camera.h"
#include "joypad.h"
#include "keyboard.h"
#include "player.h"
#include "title.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define CAMERA_MOVE	(0.25)	// カメラの移動量
#define MAX_CAMERA	(4)

//=============================================================================
// グローバル変数
//=============================================================================
CAMERA g_camera[MAX_CAMERA];	// カメラ情報

//=============================================================================
// カメラの初期化処理
//=============================================================================
void InitCamera(void)
{
	if (*GetMode() != *GetModeOld())
	{
		for (int nCnt = 0; nCnt < MAX_CAMERA; nCnt++)
		{
			g_camera[nCnt].posV = D3DXVECTOR3(0.0f, 300.0f, -800.0f);
			g_camera[nCnt].posVDest = g_camera[nCnt].posV;
			g_camera[nCnt].posVOld = g_camera[nCnt].posV;
			g_camera[nCnt].posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_camera[nCnt].posRDest = g_camera[nCnt].posR;
			g_camera[nCnt].posMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_camera[nCnt].vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			g_camera[nCnt].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_camera[nCnt].rotDest = g_camera[nCnt].rot;
			g_camera[nCnt].rotDiff = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_camera[nCnt].rotMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_camera[nCnt].fDistance = g_camera[nCnt].posR.z - g_camera[nCnt].posV.z;
			g_camera[nCnt].bWraparound = false;
		}
	}
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{
}

//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	int nStickH, nStickV;

	for (int nCnt = 0; nCnt < MAX_CAMERA; nCnt++)
	{
		// 前回の視点の位置保存
		g_camera[0].posVOld = g_camera[0].posV;

		// アナログスティック右の取得
		GetJoypadStickRight(0, &nStickH, &nStickV);

		g_camera[0].posRDest.x = -25;
		g_camera[0].posRDest.y = 90;
		g_camera[0].posRDest.z = 25;

		// 角度が一定値を超えた場合の数値制限
		if (g_camera[0].rotDest.y > D3DX_PI)
		{
			g_camera[0].rotDest.y -= D3DX_PI * 2;
		}

		if (g_camera[0].rotDest.y < -D3DX_PI)
		{
			g_camera[0].rotDest.y += D3DX_PI * 2;
		}

		if (g_camera[0].bWraparound == true)
		{
			if (g_camera[0].rotDest.y - g_camera[0].rot.y < 0.01 && g_camera[0].rotDest.y - g_camera[0].rot.y>-0.01)
			{
				g_camera[0].bWraparound = false;
			}
		}

		// 向きの差を求める
		g_camera[0].rotDiff.y = g_camera[0].rotDest.y - g_camera[0].rot.y;

		// 角度が一定値を超えた場合の数値制限
		if (g_camera[0].rotDiff.y > D3DX_PI)
		{
			g_camera[0].rotDiff.y -= D3DX_PI * 2;
		}

		if (g_camera[0].rotDiff.y < -D3DX_PI)
		{
			g_camera[0].rotDiff.y += D3DX_PI * 2;
		}

		// 回り込み中の場合
		if (g_camera[0].bWraparound == true)
		{
			g_camera[0].rot.y += (g_camera[0].rotDiff.y)*0.2f;
		}

		// 角度が一定値を超えた場合の数値制限
		if (g_camera[0].rotDest.y > D3DX_PI)
		{
			g_camera[0].rotDest.y -= D3DX_PI * 2;
		}

		if (g_camera[0].rotDest.y < -D3DX_PI)
		{
			g_camera[0].rotDest.y += D3DX_PI * 2;
		}

		// 角度が一定値を超えた場合の数値制限
		if (g_camera[0].rot.y > D3DX_PI)
		{
			g_camera[0].rot.y -= D3DX_PI * 2;
		}

		if (g_camera[0].rot.y < -D3DX_PI)
		{
			g_camera[0].rot.y += D3DX_PI * 2;
		}

		g_camera[0].posVDest.x = sinf(-g_camera[0].rot.y) * g_camera[0].fDistance;
		g_camera[0].posVDest.y = 120;
		g_camera[0].posVDest.z = -cosf(-g_camera[0].rot.y) * g_camera[0].fDistance;

		g_camera[0].posR += (g_camera[0].posRDest - g_camera[0].posR)*0.1f;

		g_camera[0].posV.x += (g_camera[0].posVDest.x - g_camera[0].posV.x)*0.1f;
		g_camera[0].posV.z += (g_camera[0].posVDest.z - g_camera[0].posV.z)*0.1f;

		// 注視点の回転
		if (GetKeyboardPress(DIK_RIGHT) || nStickH > 0)
		{// 左旋回
			g_camera[0].rotMove.y -= 0.005f;
		}

		if (GetKeyboardPress(DIK_LEFT) || nStickH < 0)
		{// 右旋回
			g_camera[0].rotMove.y += 0.005f;
		}

		// 向きの更新
		g_camera[0].rot += g_camera[0].rotMove;

		// 慣性
		g_camera[0].rotMove.y += (0 - g_camera[0].rotMove.y)*0.125f;

		// 角度が一定値を超えた場合の数値制限
		if (g_camera[0].rot.y > D3DX_PI)
		{
			g_camera[0].rot.y -= D3DX_PI * 2;
		}

		if (g_camera[0].rot.y < -D3DX_PI)
		{
			g_camera[0].rot.y += D3DX_PI * 2;
		}

		if (GetKeyboardPress(DIK_UP) || nStickV > 0)
		{// 上
			g_camera[0].posMove.y += CAMERA_MOVE;
		}
		if (GetKeyboardPress(DIK_DOWN) || nStickV < 0)
		{// 下
			g_camera[0].posMove.y -= CAMERA_MOVE;
		}

		// 視点の場所の更新
		g_camera[0].posV.y += g_camera[0].posMove.y;

		if (g_camera[0].posV.y <= 10)
		{
			g_camera[0].posV.y = g_camera[0].posVOld.y;
		}

		// 慣性
		g_camera[0].posMove.y += (0 - g_camera[0].posMove.y)*0.1f;
	}
}

//=============================================================================
// カメラの設定処理
//=============================================================================
void SetCamera(int nCount)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&g_camera[nCount].mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(
		&g_camera[nCount].mtxProjection,
		D3DXToRadian(45.0f),						// 視野角
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,	// アスペクト比
		10.0f,										// NearZ値/見える範囲/ここから
		4500.0f);									// Far Z値/見える範囲/ここまで

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &g_camera[nCount].mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_camera[nCount].mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH(
		&g_camera[nCount].mtxView,
		&g_camera[nCount].posV,		// カメラの視点
		&g_camera[nCount].posR,		// カメラの注視点
		&g_camera[nCount].vecU);		// カメラの上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &g_camera[nCount].mtxView);
}

//=============================================================================
// 注視点の回転
//=============================================================================
void RotMoveCameraR(float rotY, int nCnt)
{
	g_camera[nCnt].rot.y += rotY;

	// 角度が一定値を超えた場合の数値制限
	if (g_camera[nCnt].rot.y > D3DX_PI)
	{
		g_camera[nCnt].rot.y -= D3DX_PI * 2;
	}

	g_camera[nCnt].posV.x = g_camera[nCnt].posR.x + sinf(g_camera[nCnt].rot.y) * -g_camera[nCnt].fDistance;
	g_camera[nCnt].posV.z = g_camera[nCnt].posR.z + cosf(g_camera[nCnt].rot.y) * -g_camera[nCnt].fDistance;
}

//=============================================================================
// 注視点の回転
//=============================================================================
void RotMoveCameraV(float rotY, int nCnt)
{
	g_camera[nCnt].rot.y += rotY;

	// 角度が一定値を超えた場合の数値制限
	if (g_camera[nCnt].rot.y > D3DX_PI)
	{
		g_camera[nCnt].rot.y -= D3DX_PI * 2;
	}

	g_camera[nCnt].posR.x = g_camera[nCnt].posV.x + sinf(g_camera[nCnt].rot.y) * g_camera[nCnt].fDistance;
	g_camera[nCnt].posR.z = g_camera[nCnt].posV.z + cosf(g_camera[nCnt].rot.y) * g_camera[nCnt].fDistance;
}

//=============================================================================
// カメラの注視点の設定
//=============================================================================
void SetCameraPosR(D3DXVECTOR3 pos, int nCnt)
{
	g_camera[nCnt].posR = pos;
	g_camera[nCnt].posRDest = g_camera[nCnt].posR;
}

//=============================================================================
// カメラの視点の設定
//=============================================================================
void SetCameraPosV(D3DXVECTOR3 pos, int nCnt)
{
	g_camera[nCnt].posV = pos;
	g_camera[nCnt].posVDest = g_camera[nCnt].posV;
}

//=============================================================================
// カメラの視点の設定
//=============================================================================
void SetCameraRot(D3DXVECTOR3 rot, int nCnt)
{
	g_camera[nCnt].rot = rot;
	g_camera[nCnt].rotDest = g_camera[nCnt].rot;
}

//=============================================================================
// カメラ情報の取得
//=============================================================================
CAMERA *GetCamera(int nNum)
{
	return &g_camera[nNum];
}