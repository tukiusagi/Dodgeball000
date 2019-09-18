//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : YUKI TAKAHASHI
//
//=============================================================================

//=============================================================================
// インクルードファイル
//=============================================================================
#include "player.h"
#include "camera.h"
#include "fade.h"
#include "game.h"
#include "joypad.h"
#include "keyboard.h"
#include "meshField.h"
#include "model.h"
#include "sound.h"
#include "shadow.h"
#include "ball.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define PLAYER_FILENAME		"data/LOAD_TEXT/motion_boy.txt"		//モデルファイル名
#define TEXTUREFILE0		"data/TEXTURE/red.jpg"				// 読み込むテクスチャ
#define TEXTUREFILE1		"data/TEXTURE/blue.jpg"				// 読み込むテクスチャ
#define TEXTUREFILE2		"data/TEXTURE/yellow.jpg"			// 読み込むテクスチャ
#define TEXTUREFILE3		"data/TEXTURE/green.jpg"			// 読み込むテクスチャ
#define MAX_TEXTURE			(5)									// テクスチャ数
#define MAX_USETEXTURE		(4)									// 使用テクスチャ数
#define MAX_MODEL			(15)									// モデルの数
#define MAX_KEY				(4)									// モデルの数
#define PLAYER_MOVE			(0.4f)								// プレイヤーの移動量
#define PLAYER_REST			(1475)								// プレイヤーの移動範囲
#define PLAYER_GRAVITY		(5)									// 重力
#define PLAYER_JUMP_MOVE	(11.25)								// ジャンプ量

//=============================================================================
// モーションの種類
//=============================================================================
typedef enum
{
	MOTIONTYPE_NEUTRAL = 0,	// ニュートラル
	MOTIONTYPE_RUNNING,		// ランニング
	MOTIONTYPE_ACTION,		// アクション
	MOTIONTYPE_JUMP,		// ジャンプ
	MOTIONTYPE_LAND,		// ランド
	MOTIONTYPE_BLOWAWAY,	// 吹き飛ぶ
	MOTIONTYPE_MAX			// モーションの最大数
} MOTIONTYPE;

//=============================================================================
// 構造体定義
////=============================================================================
typedef struct
{// モデル
	LPD3DXMESH		pMesh;						// メッシュ情報へのポインタ
	LPD3DXBUFFER	pBuffMat;					// マテリアル情報へのポインタ
	DWORD			nNumMat;					// マテリアル情報の数
	D3DXMATRIX		mtxWorld;					// ワールドマトリックス
	char			cPass[32];					// モデルファイルのパス
	D3DXVECTOR3		pos;						// 位置
	D3DXVECTOR3		posSet;						// 初期位置
	D3DXVECTOR3		rot;						// 向き(回転)
	D3DXVECTOR3		rotDest;					// 到達する向き(回転)
	int				nIdxModelParent;			// 親の番号
	int				nIdxShadow;					// 影のID
} MODEL;

typedef struct
{// キー要素
	D3DXVECTOR3		pos;						// 現在の位置
	D3DXVECTOR3		rot;						// 向き(回転)
} KEY;

typedef struct
{// キー情報
	int				nNumKyeFrame;				// キーフレーム数
	KEY				aKey[MAX_MODEL];			// キー要素
	bool			bUse;						// キーを使用しているか
} KEY_INFO;

typedef struct
{// モーション情報
	int				nCntFrame;					// 現在のフレーム数
	int				nNumKey;					// 現在のキー
	int				nMaxKey;					// 最大キー数
	bool			bLoop;						// ループするか
	KEY_INFO		aKeyInfo[MAX_KEY];			// キー情報
} MOTION_INFO;

typedef struct
{// プレイヤー
	D3DXMATRIX		mtxWorld;					// ワールドマトリックス
	D3DXVECTOR3		pos;						// 現在の位置
	D3DXVECTOR3		posOld;						// 前回の位置
	D3DXVECTOR3		move;						// 移動量
	D3DXVECTOR3		rot;						// 向き(回転)
	D3DXVECTOR3		rotDest;					// 到達する向き(回転)
	D3DXVECTOR3		rotDiff;					// 到達する向きと現在の向きの差
	int				nIdxShadow;					// 影のID
	int				nNumParts;					// パーツ数
	int				nLife;						// 体力
	bool			bJump;						// ジャンプしているか
	bool			bMotion;					// モーション再生中か
	MODEL			aModel[MAX_MODEL];			// モデル情報
	MOTION_INFO		aMotionInfo[MOTIONTYPE_MAX];// モーション情報
	MOTIONTYPE		motionType;					// 現在のモーション
	bool			bCollAtk;					// 攻撃が当たっているか
	int				nRaidTrout;					// 乗っているマスの番号
	int				nNextTrout;					// 到達するマスの番号
	float			fRadius;					//当たり判定用半径
	bool			bHold;						//ボール持ってるか否か
	bool			bUse;
} PLAYER;

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void LoadPlayerMotion(void);
void ControlPlayer(int nCnt);
void Move(CAMERA *pCamera);
void MoveLimit(D3DXVECTOR3 *pos, float fRadius, bool bHold);	//移動制限
void MotionPlayer(int nCnt);									// モーションの再生
void CollisionPlayer(int nCnt);									// 接触判定
void MotionChangePlayer(MOTIONTYPE motionType, int nCnt);		// モーションの切り替え

//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9	g_paTexturePlayer[MAX_TEXTURE] = {};			// テクスチャへのポインタ

PLAYER				g_player[NUM_JOYPAD_MAX];						// プレイヤーの情報

// 陣取りの色
D3DXCOLOR g_aPlayer[NUM_JOYPAD_MAX] = {
	D3DXCOLOR(1.0f,0.0f,0.0f,1.0f),	// 1P
	D3DXCOLOR(0.0f,0.0f,1.0f,1.0f),	// 2P
	D3DXCOLOR(1.0f,1.0f,0.0f,1.0f),	// 3P
	D3DXCOLOR(0.0f,1.0f,0.0f,1.0f)	// 4P
};

//=============================================================================
// 初期化処理
//=============================================================================
void InitPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ファイルの読み込み
	LoadPlayerMotion();

	for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++)
	{
		// Xファイルの読み込み
		D3DXLoadMeshFromX((LPCSTR)&g_player[0].aModel[nCntModel].cPass[0], D3DXMESH_SYSTEMMEM, pDevice, NULL,
			&g_player[0].aModel[nCntModel].pBuffMat, NULL,
			&g_player[0].aModel[nCntModel].nNumMat,
			&g_player[0].aModel[nCntModel].pMesh);
	}

	for (int nCnt = 1; nCnt < NUM_JOYPAD_MAX; nCnt++)
	{// 同じデータを格納
		g_player[nCnt] = g_player[0];
	}

	// 初期化（遊ぶプレイヤー数で最大回数を変更する）
	for (int nCnt = 0; nCnt < GetNumPad(); nCnt++)
	{
		// 初期設定
		g_player[nCnt].posOld = g_player[nCnt].pos;
		g_player[nCnt].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_player[nCnt].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_player[nCnt].rotDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_player[nCnt].rotDiff = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_player[nCnt].bJump = false;
		g_player[nCnt].bMotion = true;
		g_player[nCnt].motionType = MOTIONTYPE_NEUTRAL;
		g_player[nCnt].bCollAtk = false;
		g_player[nCnt].fRadius = 30;
		g_player[nCnt].bHold = false;
		g_player[nCnt].bUse = true;

		// 影の作成
		g_player[nCnt].nIdxShadow = SetShadow(g_player[nCnt].pos, g_player[nCnt].rot, D3DXVECTOR3(100.0f, 0.0f, 100.0f));

		for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++)
		{
			g_player[nCnt].aModel[nCntModel].posSet = g_player[nCnt].aModel[nCntModel].pos;
		}
	}

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE0, &g_paTexturePlayer[0]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE1, &g_paTexturePlayer[1]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE2, &g_paTexturePlayer[2]);
	D3DXCreateTextureFromFile(pDevice, TEXTUREFILE3, &g_paTexturePlayer[3]);
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	for (int nCnt = 0; nCnt < NUM_JOYPAD_MAX; nCnt++)
	{
		// メッシュの開放
		for (int nCntMesh = 0; nCntMesh < MAX_MODEL; nCntMesh++)
		{
			if (g_player[nCnt].aModel[nCntMesh].pMesh != NULL)
			{
				g_player[nCnt].aModel[nCntMesh].pMesh->Release();
				g_player[nCnt].aModel[nCntMesh].pMesh = NULL;
			}
		}

		// マテリアルの開放
		for (int nCntMat = 0; nCntMat < MAX_MODEL; nCntMat++)
		{
			if (g_player[nCnt].aModel[nCntMat].pBuffMat != NULL)
			{
				g_player[nCnt].aModel[nCntMat].pBuffMat->Release();
				g_player[nCnt].aModel[nCntMat].pBuffMat = NULL;
			}
		}
	}

	// テクスチャの開放
	for (int nCntTex = 0; nCntTex < NUM_JOYPAD_MAX; nCntTex++)
	{
		if (g_paTexturePlayer[nCntTex] != NULL)
		{
			g_paTexturePlayer[nCntTex]->Release();
			g_paTexturePlayer[nCntTex] = NULL;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	int nNumPlayer = GetNumPad();	// プレイヤーの人数

	if (*GetMode() == MODE_RESULT)
	{
		for (int nCnt = 0; nCnt < nNumPlayer; nCnt++)
		{
			g_player[nCnt].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_player[nCnt].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			// モーションの再生
			MotionPlayer(nCnt);
		}
		return;
	}

	for (int nCnt = 0; nCnt < nNumPlayer; nCnt++)
	{
		if (g_player[nCnt].bUse)
		{
			// 前回の位置を保存
			g_player[nCnt].posOld = g_player[nCnt].pos;

			// 重力の設定
			g_player[nCnt].pos.y -= PLAYER_GRAVITY;

			// コントロール
			ControlPlayer(nCnt);

			// 角度が一定値を超えた場合の数値制限
			if (g_player[nCnt].rot.y > D3DX_PI)
			{
				g_player[nCnt].rot.y -= D3DX_PI * 2;
			}

			if (g_player[nCnt].rot.y < -D3DX_PI)
			{
				g_player[nCnt].rot.y += D3DX_PI * 2;
			}

			// 角度が一定値を超えた場合の数値制限
			if (g_player[nCnt].rotDest.y > D3DX_PI)
			{
				g_player[nCnt].rotDest.y -= D3DX_PI * 2;
			}

			if (g_player[nCnt].rotDest.y < -D3DX_PI)
			{
				g_player[nCnt].rotDest.y += D3DX_PI * 2;
			}

			// 向きの差を求める
			g_player[nCnt].rotDiff.y = g_player[nCnt].rotDest.y - g_player[nCnt].rot.y;

			// 角度が一定値を超えた場合の数値制限
			if (g_player[nCnt].rotDiff.y > D3DX_PI)
			{
				g_player[nCnt].rotDiff.y -= D3DX_PI * 2;
			}

			if (g_player[nCnt].rotDiff.y < -D3DX_PI)
			{
				g_player[nCnt].rotDiff.y += D3DX_PI * 2;
			}

			// 向きの更新
			g_player[nCnt].rot.y += (g_player[nCnt].rotDiff.y)*0.2f;

			// 位置の更新
			g_player[nCnt].pos.x += g_player[nCnt].move.x;
			g_player[nCnt].pos.y += g_player[nCnt].move.y;
			g_player[nCnt].pos.z += g_player[nCnt].move.z;

			// 接触判定
			CollisionPlayer(nCnt);

			if (g_player[nCnt].bUse)
			{
				// 影の位置を設定
				SetPositionShadow(g_player[nCnt].nIdxShadow, g_player[nCnt].pos);
			}

			// 慣性(減速)
			g_player[nCnt].move.x += (0 - g_player[nCnt].move.x) * 0.125f;
			g_player[nCnt].move.y += (0 - g_player[nCnt].move.y) * 0.125f;
			g_player[nCnt].move.z += (0 - g_player[nCnt].move.z) * 0.125f;

			if (g_player[nCnt].motionType != MOTIONTYPE_JUMP && !g_player[nCnt].bMotion)
			{//  再生状態か
				g_player[nCnt].bMotion = true;
			}

			// モーションの再生
			MotionPlayer(nCnt);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;		// 計算用マトリックス
	D3DXMATERIAL *pMat;							// 現在のマテリアル保存用
	D3DMATERIAL9 matDef;						// マテリアルデータへのポインタ

	for (int nCnt = 0; nCnt < GetNumPad(); nCnt++)
	{
		// 本体
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_player[nCnt].mtxWorld);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_player[nCnt].rot.y, g_player[nCnt].rot.x, g_player[nCnt].rot.z);
		D3DXMatrixMultiply(&g_player[nCnt].mtxWorld, &g_player[nCnt].mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_player[nCnt].pos.x, g_player[nCnt].pos.y, g_player[nCnt].pos.z);
		D3DXMatrixMultiply(&g_player[nCnt].mtxWorld, &g_player[nCnt].mtxWorld, &mtxTrans);

		MODEL *pModel = &g_player[nCnt].aModel[0];

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_player[nCnt].mtxWorld);

		for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++, pModel++)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pModel->mtxWorld);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pModel->rot.y, pModel->rot.x, pModel->rot.z);
			D3DXMatrixMultiply(&pModel->mtxWorld, &pModel->mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pModel->pos.x,
				pModel->pos.y, pModel->pos.z);
			D3DXMatrixMultiply(&pModel->mtxWorld, &pModel->mtxWorld, &mtxTrans);

			if (pModel->nIdxModelParent == -1)
			{
				D3DXMatrixMultiply(&pModel->mtxWorld, &pModel->mtxWorld, &g_player[nCnt].mtxWorld);
			}
			else
			{
				D3DXMatrixMultiply(&pModel->mtxWorld, &pModel->mtxWorld, &g_player[nCnt].aModel[pModel->nIdxModelParent].mtxWorld);
			}

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pModel->mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアル情報に対するポインタを取得
			pMat = (D3DXMATERIAL*)pModel->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_player[nCnt].aModel[nCntModel].nNumMat; nCntMat++)
			{
				if (g_player[nCnt].bUse)
				{
					// テクスチャの設定
					pDevice->SetTexture(0, g_paTexturePlayer[nCnt]);

					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

					// 描画
					pModel->pMesh->DrawSubset(nCntMat);
				}
			}
			// マテリアルをデフォルトに戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//=============================================================================
// コントロール
//=============================================================================
void ControlPlayer(int nCnt)
{
	int nStickH, nStickV;			// アナログスティック左の入力情報
	CAMERA *pCamera = GetCamera(0);	// カメラ情報の取得

	// アナログスティック左の取得
	GetJoypadStickLeft(nCnt, &nStickH, &nStickV);

	// パッド
	if (nStickH != 0 || nStickV != 0)
	{
		// 移動
		g_player[nCnt].move.x += sinf(D3DX_PI * (float)atan2(nStickH, nStickV) / D3DX_PI + pCamera->rot.y) * 1.0f;
		g_player[nCnt].move.z += cosf(D3DX_PI * (float)atan2(nStickH, nStickV) / D3DX_PI + pCamera->rot.y) * 1.0f;

		// 向き
		g_player[nCnt].rotDest.y = D3DX_PI * -(float)atan2(nStickH, -nStickV) / D3DX_PI + pCamera->rot.y;

		if (g_player[nCnt].motionType != MOTIONTYPE_RUNNING&&g_player[nCnt].motionType != MOTIONTYPE_JUMP)
		{
			// モーションの切り替え
			MotionChangePlayer(MOTIONTYPE_RUNNING, nCnt);
		}
	}
	else if (g_player[nCnt].motionType == MOTIONTYPE_RUNNING)
	{// 移動をやめた場合
		// モーションの切り替え
		MotionChangePlayer(MOTIONTYPE_NEUTRAL, nCnt);
	}

	{// 多分あとで消す
		//キーボード移動
		Move(pCamera);
	}

	// ジャンプ
	if (!g_player[nCnt].bJump)// ジャンプしていない場合
	{
		if (GetKeyboardTrigger(DIK_J) || GetJoypadTrigger(nCnt, JOYPADKEY_A))
		{
			g_player[nCnt].move.y += PLAYER_JUMP_MOVE;

			// モーションの切り替え
			MotionChangePlayer(MOTIONTYPE_JUMP, nCnt);
		}
	}

	//ボール投げる＆つかむ
	float fDistX, fDistZ;
	float fDistans = 100000;
	BALL *pBall = GetBall();
	fDistX = pBall->pos.x - g_player[nCnt].pos.x;
	fDistZ = pBall->pos.z - g_player[nCnt].pos.z;
	fDistans = (float)sqrt(fDistX * fDistX + fDistZ * fDistZ);

	if (GetJoypadTrigger(nCnt, JOYPADKEY_B))
	{
		if (pBall->nParent == nCnt && pBall->bHold)
		{	//投げる
			pBall->state = BALLSTATE_THROW;
			pBall->bHold = false;
			g_player[nCnt].bHold = false;
			ThrowBall(g_player[nCnt].move);

			// モーションの切り替え
			MotionChangePlayer(MOTIONTYPE_ACTION, nCnt);
		}
		else if (g_player[nCnt].fRadius + PLAYER_RANGE > fDistans && !pBall->bHold)
		{	//つかむ
			pBall->state = BALLSTATE_HOLD;
			pBall->bHold = true;
			g_player[nCnt].bHold = true;
			pBall->nParent = nCnt;
		}
	}
}

//=============================================================================
//	移動制限
//=============================================================================
void MoveLimit(D3DXVECTOR3 *pos, float fRadius, bool bHold)
{
	//if (bHold)
	//{
	//	//左右
	//	if (pos->x > MESHFIELD_SIZE * 2 - (fRadius + BALL_RADIUS*2))
	//	{
	//		pos->x = MESHFIELD_SIZE * 2 - (fRadius + BALL_RADIUS*2);
	//	}
	//	if (pos->x < -MESHFIELD_SIZE * 2 + (fRadius + BALL_RADIUS*2))
	//	{
	//		pos->x = -MESHFIELD_SIZE * 2 + (fRadius + BALL_RADIUS*2);
	//	}

	//	//奥手前
	//	if (pos->z > MESHFIELD_SIZE * 2 - (fRadius + BALL_RADIUS*2))
	//	{
	//		pos->z = MESHFIELD_SIZE * 2 - (fRadius + BALL_RADIUS*2);
	//	}
	//	if (pos->z < -MESHFIELD_SIZE * 2 + (fRadius + BALL_RADIUS*2))
	//	{
	//		pos->z = -MESHFIELD_SIZE * 2 + (fRadius + BALL_RADIUS*2);
	//	}
	//	return;
	//}
	//左右
	if (pos->x > MESHFIELD_SIZE * 2 - fRadius)
	{
		pos->x = MESHFIELD_SIZE * 2 - fRadius;
	}
	if (pos->x < -MESHFIELD_SIZE * 2 + fRadius)
	{
		pos->x = -MESHFIELD_SIZE * 2 + fRadius;
	}

	//奥手前
	if (pos->z > MESHFIELD_SIZE * 2 - fRadius)
	{
		pos->z = MESHFIELD_SIZE * 2 - fRadius;
	}
	if (pos->z < -MESHFIELD_SIZE * 2 + fRadius)
	{
		pos->z = -MESHFIELD_SIZE * 2 + fRadius;
	}
}

//=============================================================================
//	移動処理
//=============================================================================
void Move(CAMERA *pCamera)
{
	// キーボード
	if (GetKeyboardPress(DIK_A))
	{
		if (GetKeyboardPress(DIK_W))
		{// 左上
			g_player[1].move.x += sinf(-D3DX_PI*0.75f - pCamera->rot.y) * PLAYER_MOVE;
			g_player[1].move.z -= cosf(-D3DX_PI*0.75f - pCamera->rot.y) * PLAYER_MOVE;

			g_player[1].rotDest.y = D3DX_PI*0.75f + pCamera->rot.y;
		}
		else if (GetKeyboardPress(DIK_S))
		{// 左下
			g_player[1].move.x += sinf(-D3DX_PI*0.25f - pCamera->rot.y) * PLAYER_MOVE;
			g_player[1].move.z -= cosf(-D3DX_PI*0.25f - pCamera->rot.y) * PLAYER_MOVE;

			g_player[1].rotDest.y = D3DX_PI*0.25f + pCamera->rot.y;
		}
		else
		{// 左
			g_player[1].move.x += sinf(-D3DX_PI*0.5f - pCamera->rot.y) * PLAYER_MOVE;
			g_player[1].move.z -= cosf(-D3DX_PI*0.5f - pCamera->rot.y) * PLAYER_MOVE;

			g_player[1].rotDest.y = D3DX_PI*0.5f + pCamera->rot.y;
		}
	}
	else if (GetKeyboardPress(DIK_D))
	{
		if (GetKeyboardPress(DIK_W))
		{// 右上
			g_player[1].move.x += sinf(D3DX_PI*0.75f - pCamera->rot.y) * PLAYER_MOVE;
			g_player[1].move.z -= cosf(D3DX_PI*0.75f - pCamera->rot.y) * PLAYER_MOVE;

			g_player[1].rotDest.y = -D3DX_PI*0.75f + pCamera->rot.y;
		}
		else if (GetKeyboardPress(DIK_S))
		{// 右下
			g_player[1].move.x += sinf(D3DX_PI*0.25f - pCamera->rot.y) * PLAYER_MOVE;
			g_player[1].move.z -= cosf(D3DX_PI*0.25f - pCamera->rot.y) * PLAYER_MOVE;

			g_player[1].rotDest.y = -D3DX_PI*0.25f + pCamera->rot.y;
		}
		else
		{// 右
			g_player[1].move.x += sinf(D3DX_PI*0.5f - pCamera->rot.y) * PLAYER_MOVE;
			g_player[1].move.z -= cosf(D3DX_PI*0.5f - pCamera->rot.y) * PLAYER_MOVE;

			g_player[1].rotDest.y = -D3DX_PI*0.5f + pCamera->rot.y;
		}
	}
	else if (GetKeyboardPress(DIK_W))
	{// 上
		g_player[1].move.x += sinf(D3DX_PI - pCamera->rot.y) * PLAYER_MOVE;
		g_player[1].move.z -= cosf(D3DX_PI - pCamera->rot.y) * PLAYER_MOVE;

		g_player[1].rotDest.y = D3DX_PI + pCamera->rot.y;
	}
	else if (GetKeyboardPress(DIK_S))
	{// 下
		g_player[1].move.x += sinf(0 - pCamera->rot.y) * PLAYER_MOVE;
		g_player[1].move.z -= cosf(0 - pCamera->rot.y) * PLAYER_MOVE;

		g_player[1].rotDest.y = 0 + pCamera->rot.y;
	}

	if (GetKeyboardTrigger(DIK_SPACE))
	{
		BALL *pBall = GetBall();
		float fDistX, fDistZ;
		float fDistans = 100000;
		fDistX = pBall->pos.x - g_player[1].pos.x;
		fDistZ = pBall->pos.z - g_player[1].pos.z;
		fDistans = (float)sqrt(fDistX * fDistX + fDistZ * fDistZ);

		if (g_player[1].bHold)
		{	//投げる
			pBall->state = BALLSTATE_THROW;
			pBall->bHold = false;
			g_player[1].bHold = false;
			ThrowBall(g_player[1].move);
		}
		else if (g_player[1].fRadius + PLAYER_RANGE > fDistans && !pBall->bHold)
		{	//つかむ
			pBall->state = BALLSTATE_HOLD;
			pBall->bHold = true;
			g_player[1].bHold = true;
			pBall->nParent = 1;
		}
	}
}

//=============================================================================
// モーションの再生
//=============================================================================
void MotionPlayer(int nCnt)
{
	if (g_player[nCnt].bMotion)
	{
		MOTION_INFO* pInfo = &g_player[nCnt].aMotionInfo[g_player[nCnt].motionType];
		// モーション
		for (int nCntModel = 0; nCntModel < g_player[nCnt].nNumParts; nCntModel++)
		{
			KEY *pKeyInfo, *pNextKey;

			// 現在のキー
			pKeyInfo = &pInfo->aKeyInfo[pInfo->nNumKey].aKey[nCntModel];

			// 次のキー
			if (pInfo->nNumKey + 1 == pInfo->nMaxKey)
			{
				pNextKey = &pInfo->aKeyInfo[0].aKey[nCntModel];
			}
			else
			{
				pNextKey = &pInfo->aKeyInfo[pInfo->nNumKey + 1].aKey[nCntModel];
			}

			// パーツの位置設定
			g_player[nCnt].aModel[nCntModel].pos = g_player[nCnt].aModel[nCntModel].posSet + pKeyInfo->pos + (pNextKey->pos - pKeyInfo->pos)*
				pInfo->nCntFrame / pInfo->aKeyInfo[pInfo->nNumKey].nNumKyeFrame;

			// パーツの向き設定
			g_player[nCnt].aModel[nCntModel].rot = pKeyInfo->rot + (pNextKey->rot - pKeyInfo->rot)*
				pInfo->nCntFrame / pInfo->aKeyInfo[pInfo->nNumKey].nNumKyeFrame;
		}

		pInfo->nCntFrame++;

		if (pInfo->nCntFrame == pInfo->aKeyInfo[pInfo->nNumKey].nNumKyeFrame)
		{
			pInfo->nNumKey++;
			pInfo->nCntFrame = 0;
		}

		if (!pInfo->bLoop && pInfo->nNumKey + 1 == pInfo->nMaxKey)
		{// モーションがループしない場合
			if (g_player[nCnt].motionType != MOTIONTYPE_JUMP)
			{// ジャンプ以外
				pInfo->nNumKey = 0;
				pInfo->nCntFrame = 0;
				g_player[nCnt].motionType = MOTIONTYPE_NEUTRAL;// 待機状態に変更
			}
			else
			{// ジャンプのみ
				g_player[nCnt].bMotion = false;
			}
		}
		else if (pInfo->nNumKey == pInfo->nMaxKey && g_player[nCnt].motionType != MOTIONTYPE_JUMP)
		{// モーションが終了した場合
			pInfo->nNumKey = 0;
		}
	}
}

//=============================================================================
// 接触判定
//=============================================================================
void CollisionPlayer(int nCnt)
{
	MODE *pMode = GetMode();		// 現在のモード
	bool bRand = false;				// 着地判定

	// 移動制限
	MoveLimit(&g_player[nCnt].pos, g_player[nCnt].fRadius, g_player[nCnt].bHold);

	if (g_player[nCnt].pos.y < 0)
	{
		g_player[nCnt].pos.y = 0;
		bRand = true;
	}

	if (bRand)
	{// 着地した場合
		g_player[nCnt].bJump = false;	// ジャンプできるようにする

		if (g_player[nCnt].motionType == MOTIONTYPE_JUMP)
		{
			// モーションの切り替え
			MotionChangePlayer(MOTIONTYPE_LAND, nCnt);
		}
	}
	else if (!g_player[nCnt].bJump)
	{
		g_player[nCnt].bJump = true;	// ジャンプできないようにする
	}

	// 当たり判定
	if (*pMode == MODE_GAME)
	{
		BALL *pBall = GetBall();

		if (!g_player[nCnt].bHold && pBall->nParent != nCnt)// ボールを持っているか
		{
			if (CollisionBall(&g_player[nCnt].pos, &g_player[nCnt].posOld, g_player[nCnt].fRadius))
			{
				if (pBall->state == BALLSTATE_THROW)
				{
					g_player[nCnt].bUse = false;
					DeleteShadow(g_player[nCnt].nIdxShadow);
				}
			}
		}
	}
}

//=============================================================================
// モーションの切り替え
//=============================================================================
void MotionChangePlayer(MOTIONTYPE motionType, int nCnt)
{
	g_player[nCnt].aMotionInfo[g_player[nCnt].motionType].nNumKey = 0;
	g_player[nCnt].aMotionInfo[g_player[nCnt].motionType].nCntFrame = 0;
	g_player[nCnt].motionType = motionType;
}

//=============================================================================
// モーションのロード
//=============================================================================
void LoadPlayerMotion(void)
{
	FILE *pFile = NULL;		// ファイルポインタ

	// ファイルオープン
	pFile = fopen(PLAYER_FILENAME, "r");

	if (pFile != NULL)
	{// ファイルが無事に開けた場合
		char cReedText[128];	// 読み取り用
		char cHeadText[128];	// 文字格納用
		int nLoop = 0;;			// ループ格納用
		MOTION_INFO *pMotion = &g_player[0].aMotionInfo[0];
		MODEL *pModel = &g_player[0].aModel[0];

		while (strcmp(cHeadText, "SCRIPT") != 0)
		{// SCRIPTが読み込まれるまでループ
			fgets(cReedText, sizeof(cReedText), pFile);
			sscanf(cReedText, "%s", &cHeadText);
		}

		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReedText, sizeof(cReedText), pFile);
				sscanf(cReedText, "%s", &cHeadText);

				if (strcmp(cHeadText, "MODEL_FILENAME") == 0)
				{// ファイル名
					sscanf(cReedText, "%*s %*s %s", &pModel->cPass[0]);
					pModel++;
				}

				if (strcmp(cHeadText, "CHARACTERSET") == 0)
				{
					pModel = &g_player[0].aModel[0];

					while (strcmp(cHeadText, "END_CHARACTERSET") != 0)
					{
						fgets(cReedText, sizeof(cReedText), pFile);
						sscanf(cReedText, "%s", &cHeadText);

						if (strcmp(cHeadText, "MOVE") == 0)
						{// 移動量
							//sscanf(cReedText, "%s %s %f", &cDie, &cDie, &g_player[0].fAddMove);
						}

						//if (strcmp(cHeadText, "JUMP") == 0)
						//{// ジャンプ量
						//	sscanf(cReedText, "%s %s %d", &cDie, &cDie, &g_player[0].nNumParts);
						//}

						//if (strcmp(cHeadText, "RADIUS") == 0)
						//{// 半径
						//	sscanf(cReedText, "%s %s %d", &cDie, &cDie, &g_player[0].nNumParts);
						//}

						if (strcmp(cHeadText, "NUM_PARTS") == 0)
						{// パーツ数
							sscanf(cReedText, "%*s %*s %d", &g_player[0].nNumParts);
						}
						else if (strcmp(cHeadText, "PARTSSET") == 0)
						{
							while (strcmp(cHeadText, "END_PARTSSET") != 0)
							{
								fgets(cReedText, sizeof(cReedText), pFile);
								sscanf(cReedText, "%s", &cHeadText);

								if (strcmp(cHeadText, "INDEX") == 0)
								{// 本体の番号
								 //sscanf(cReedText, "%s %s %d", &cDie, &cDie, &g_player[0].nNumParts);
								}

								if (strcmp(cHeadText, "PARENT") == 0)
								{// 親の番号
									sscanf(cReedText, "%*s %*s %d", &pModel->nIdxModelParent);
								}

								if (strcmp(cHeadText, "POS") == 0)
								{// 位置
									sscanf(cReedText, "%*s %*s %f %f %f", &pModel->pos.x, &pModel->pos.y, &pModel->pos.z);
								}

								if (strcmp(cHeadText, "ROT") == 0)
								{// 向き
									sscanf(cReedText, "%*s %*s %f %f %f", &pModel->rot.x, &pModel->rot.y, &pModel->rot.z);
								}
							}
							pModel++;
						}
					}
				}
				else if (strcmp(cHeadText, "MOTIONSET") == 0)
				{
					KEY_INFO *pKeyInfo = &pMotion->aKeyInfo[0];
					while (strcmp(cHeadText, "END_MOTIONSET") != 0)
					{
						fgets(cReedText, sizeof(cReedText), pFile);
						sscanf(cReedText, "%s", &cHeadText);

						if (strcmp(cHeadText, "LOOP") == 0)
						{// ループするかどうか
							sscanf(cReedText, "%*s %*s %d", &nLoop);

							// 1の場合ループする
							pMotion->bLoop = nLoop == 1 ? true : false;
						}
						else if (strcmp(cHeadText, "NUM_KEY") == 0)
						{// 最大キー数
							sscanf(cReedText, "%*s %*s %d", &pMotion->nMaxKey);
						}
						else if (strcmp(cHeadText, "KEYSET") == 0)
						{
							KEY *pKey = &pKeyInfo->aKey[0];
							while (strcmp(cHeadText, "END_KEYSET") != 0)
							{
								fgets(cReedText, sizeof(cReedText), pFile);
								sscanf(cReedText, "%s", &cHeadText);

								if (strcmp(cHeadText, "FRAME") == 0)
								{// キーフレーム数
									sscanf(cReedText, "%*s %*s %d", &pKeyInfo->nNumKyeFrame);
								}
								else if (strcmp(cHeadText, "KEY") == 0)
								{
									while (strcmp(cHeadText, "END_KEY") != 0)
									{
										fgets(cReedText, sizeof(cReedText), pFile);
										sscanf(cReedText, "%s", &cHeadText);

										if (strcmp(cHeadText, "POS") == 0)
										{// 位置
											sscanf(cReedText, "%*s %*s %f %f %f",
												&pKey->pos.x, &pKey->pos.y, &pKey->pos.z);
										}
										else if (strcmp(cHeadText, "ROT") == 0)
										{// 向き
											sscanf(cReedText, "%*s %*s %f %f %f",
												&pKey->rot.x, &pKey->rot.y, &pKey->rot.z);
										}
									}
									pKey++;
								}
							}
							pKeyInfo++;
						}
					}
					pMotion++;
				}
			}
		}
		fclose(pFile);	// ファイルを閉じる
	}
	else
	{// 開けなかった場合
		// エラーメッセージ
		//MessageBox(hWnd, "ファイルの読み込みに失敗しました。", "警告", MB_ICONWARNING);
	}
}

//=============================================================================
// 場所の設定
//=============================================================================
void SetPlayerPos(D3DXVECTOR3 pos, int nNum)
{
	g_player[nNum].pos = pos;
}

//=============================================================================
// 向きの設定
//=============================================================================
void SetPlayerRot(D3DXVECTOR3 rot, int nNum)
{
	g_player[nNum].rot = rot;
	g_player[nNum].rotDest = g_player[nNum].rot;
}

//=============================================================================
// 移動量の設定
//=============================================================================
void SetPlayerMove(D3DXVECTOR3 move, int nNum)
{
	g_player[nNum].move = move;
}

//=============================================================================
// 場所情報の取得処理
//=============================================================================
D3DXVECTOR3 *GetPlayerPos(int nNum)
{
	return&g_player[nNum].pos;
}

//=============================================================================
// 場所情報の取得処理
//=============================================================================
D3DXVECTOR3 *GetPlayerPosOld(int nNum)
{
	return&g_player[nNum].posOld;
}

//=============================================================================
// 向き情報の取得処理
//=============================================================================
D3DXVECTOR3 *GetPlayerRot(int nNum)
{
	return&g_player[nNum].rot;
}

//=============================================================================
// 移動量情報の取得処理
//=============================================================================
D3DXVECTOR3 *GetPlayerMove(int nNum)
{
	return&g_player[nNum].move;
}

//=============================================================================
// 半径情報の取得処理
//=============================================================================
float *GetPlayerRadius(int nNum)
{
	return&g_player[nNum].fRadius;
}

//=============================================================================
// プレイヤーの生死状態の取得処理
//=============================================================================
bool GetPlayerUse(int nNum)
{
	return g_player[nNum].bUse;
}