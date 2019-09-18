//=============================================================================
//
// ボール処理 [ball.cpp]
// Author : KANAZAWA TUKIHO
//
//=============================================================================
#include "ball.h"
#include "player.h"
#include "game.h"
#include "shadow.h"
#include "fade.h"
#include "meshwall.h"
#include "meshfield.h"
#include "sound.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define BALL_MODEL_FILE "data/MODEL/ball.x"			//読み込みモデルファイル名[アヒル]
#define ATTACK_SPEED	(0.01)		// 攻撃判定がある速度
#define BALL_SHADOW	(5.0f)			//影の大きさ
#define BALL_SPEED	(30.0f)			//ボールを投げる速さ
#define BALL_INERTIA (0.03f)		//ボールの慣性

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void MoveLimitBall(D3DXVECTOR3 *pos, float fRadius);	//移動制限

//=============================================================================
// グローバル変数
//=============================================================================
LPD3DXMESH			g_pMeshBall;			//メッシュ情報へのポインタ
LPD3DXBUFFER		g_pBuffMatBall;		//マテリアル情報へのポインタ
DWORD				g_nNumMatBall;		//マテリアル情報の数

LPDIRECT3DTEXTURE9	g_pTextureBall = NULL;				//テクスチャへのポインタ

BALL				g_aBall;					//ボールの情報

//=============================================================================
// 初期化処理
//=============================================================================
void InitBall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 位置・向きの初期設定
	g_aBall.pos = D3DXVECTOR3(-100.0f, 50.0f, 0.0f);
	g_aBall.posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_aBall.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_aBall.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	g_aBall.fRadius = BALL_RADIUS;
	g_aBall.state = BALLSTATE_NORMAL;

	//影の作成
	g_aBall.nIdxShadow = SetShadow(D3DXVECTOR3(g_aBall.pos.x, g_aBall.pos.y, g_aBall.pos.z),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(100.0f, 0.0f, 100.0f));

	// Xファイルの読み込み
	D3DXLoadMeshFromX(BALL_MODEL_FILE,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&g_pBuffMatBall,
		NULL,
		&g_nNumMatBall,
		&g_pMeshBall);


	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		NULL,
		&g_pTextureBall);
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBall(void)
{
	//テクスチャの開放
	if (g_pTextureBall != NULL)
	{
		g_pTextureBall->Release();
		g_pTextureBall = NULL;
	}

	// メッシュの開放
	if (g_pMeshBall != NULL)
	{
		g_pMeshBall->Release();
		g_pMeshBall = NULL;
	}

	// マテリアルの開放
	if (g_pBuffMatBall != NULL)
	{
		g_pBuffMatBall->Release();
		g_pBuffMatBall = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBall(void)
{
	if (g_aBall.state == BALLSTATE_THROW)
	{
		if (g_aBall.move.x <= ATTACK_SPEED && g_aBall.move.z <= ATTACK_SPEED && 
			g_aBall.move.x >= -ATTACK_SPEED && g_aBall.move.z >= -ATTACK_SPEED)
		{
			g_aBall.state = BALLSTATE_NORMAL;
		}
	}

	// 慣性(減速)
	g_aBall.move.x += (0 - g_aBall.move.x)*BALL_INERTIA;
	g_aBall.move.y += (0 - g_aBall.move.y)*BALL_INERTIA;
	g_aBall.move.z += (0 - g_aBall.move.z)*BALL_INERTIA;

	g_aBall.pos += g_aBall.move;

	if (g_aBall.bHold)
	{	//持たれてる
		CatchBall(*GetPlayerPos(g_aBall.nParent), *GetPlayerMove(g_aBall.nParent), *GetPlayerRadius(g_aBall.nParent));
	}

	//影の位置を設定
	SetPositionShadow(g_aBall.nIdxShadow, g_aBall.pos);

	//重力
	//g_aBall.move.y -= 0.08f;

	MoveLimitBall(&g_aBall.pos, g_aBall.fRadius);

	if (g_aBall.pos.y < 0)
	{	//地面にめり込まないように
		g_aBall.pos.y = 0;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// スケールを反映
	D3DXMATRIX mtxRot, mtxTrans;			//計算用
	D3DXMATERIAL *pMat;						//現在のマテリアル保存用
	D3DMATERIAL9 matDef;					//マテリアルのポインタ

		// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_aBall.mtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aBall.rot.y, g_aBall.rot.x, g_aBall.rot.z);
	D3DXMatrixMultiply(&g_aBall.mtxWorld, &g_aBall.mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_aBall.pos.x, g_aBall.pos.y, g_aBall.pos.z);
	D3DXMatrixMultiply(&g_aBall.mtxWorld, &g_aBall.mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_aBall.mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアル情報に対するポインタを取得
	pMat = (D3DXMATERIAL*)g_pBuffMatBall->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)g_nNumMatBall; nCntMat++)
	{
		//テクスチャの設定
		pDevice->SetTexture(0, g_pTextureBall);

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// 描画
		g_pMeshBall->DrawSubset(nCntMat);
	}
	// マテリアルをデフォルトに戻す
	pDevice->SetMaterial(&matDef);
}

//=============================================================================
// ボール持つ
//=============================================================================
void CatchBall(D3DXVECTOR3 pos, D3DXVECTOR3 move, float fRadius)
{
	float fLength = (fRadius + g_aBall.fRadius);
	D3DXVECTOR3 vec = move;	//プレイヤーの移動方向ベクトル
	D3DXVec3Normalize(&vec, &vec);

	//ボールを体の前に
	if (vec.x >= 0.01f || vec.z >= 0.01f || vec.x <= -0.01f || vec.z <= -0.01f)
	{	//速度ある時だけ
		g_aBall.pos.x = pos.x + vec.x * fLength;
		g_aBall.pos.z = pos.z + vec.z * fLength;
	}

	//一緒に動く
	g_aBall.move = move;
}

//=============================================================================
// ボール投げる
//=============================================================================
void ThrowBall(D3DXVECTOR3 move)
{
	D3DXVECTOR3 vec = move;	//プレイヤーの移動方向ベクトル
	D3DXVec3Normalize(&vec, &vec);

	//向いてる方向に投げる
	g_aBall.move = vec * BALL_SPEED;
}

//=============================================================================
// 当たり判定
//=============================================================================
bool CollisionBall(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pPosOld, float fRadius)
{
	bool bLand = false;
	float fDistance = (pPos->x - g_aBall.pos.x)*(pPos->x - g_aBall.pos.x) + (pPos->z - g_aBall.pos.z)*(pPos->z - g_aBall.pos.z);	//2点間の距離
	float fLength = (fRadius + g_aBall.fRadius)*(fRadius + g_aBall.fRadius);	//半径
	float fDiff = (fRadius + g_aBall.fRadius) - (float)sqrt(fDistance);	//差
	D3DXVECTOR3 vec = D3DXVECTOR3((pPos->x - g_aBall.pos.x), 0.0f, (pPos->z - g_aBall.pos.z));	//ベクトル
	D3DXVec3Normalize(&vec, &vec);

	if (fDistance <= fLength)
	{
		bLand = true;
		pPos->x = pPosOld->x + vec.x * fDiff;
		pPos->z = pPosOld->z + vec.z * fDiff;

	}
	return bLand;
}

//=============================================================================
//	移動制限
//=============================================================================
void MoveLimitBall(D3DXVECTOR3 *pos, float fRadius)
{
	//左右
	if (pos->x > MESHFIELD_SIZE * 2 - fRadius)
	{
		pos->x = MESHFIELD_SIZE * 2 - fRadius;
		g_aBall.move = D3DXVECTOR3(0, 0, 0);
	}
	if (pos->x < -MESHFIELD_SIZE * 2 + fRadius)
	{
		pos->x = -MESHFIELD_SIZE * 2 + fRadius;
		g_aBall.move = D3DXVECTOR3(0, 0, 0);
	}

	//奥手前
	if (pos->z > MESHFIELD_SIZE * 2 - fRadius)
	{
		pos->z = MESHFIELD_SIZE * 2 - fRadius;
		g_aBall.move = D3DXVECTOR3(0, 0, 0);
	}
	if (pos->z < -MESHFIELD_SIZE * 2 + fRadius)
	{
		pos->z = -MESHFIELD_SIZE * 2 + fRadius;
		g_aBall.move = D3DXVECTOR3(0, 0, 0);
	}
}

//=============================================================================
// ボールの消去
//=============================================================================
void DeleteBall(int nCntBall)
{
}

//=============================================================================
//	ボールの取得
//=============================================================================
BALL *GetBall(void)
{
	return &g_aBall;
}