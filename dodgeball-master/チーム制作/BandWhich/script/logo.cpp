//=============================================================================
//
// ロゴ処理[logo.cpp]
// Author : KANAZAWA TUKIHO
//
//=============================================================================
#include"logo.h"
#include "keyboard.h"	
#include "joypad.h"
#include "fade.h"
#include "main.h"
#include"game.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define LOGO_PLAYER1		"data/TEXTURE/player1.png"		//1PWin
#define LOGO_PLAYER2		"data/TEXTURE/player2.png"		//2PWin
#define LOGO_PLAYER3		"data/TEXTURE/player3.png"		//3PWin
#define LOGO_PLAYER4		"data/TEXTURE/player4.png"		//4PWin

#define LOGO_POS_X		(0)		//ポリゴン位置X
#define LOGO_POS_Y		(0)		//ポリゴン位置Y
#define LOGO_SIZE_X		(600)	//ポリゴンサイズX
#define LOGO_SIZE_Y		(200)	//ポリゴンサイズY


//=============================================================================
// ロゴの構造体
//=============================================================================
typedef struct
{
	D3DXVECTOR3 pos;
	D3DXCOLOR	col;
	LOGOTYPE	type;
	bool		bUse;
}LOGO;
//=============================================================================
// グローバル変数
//=============================================================================
LPDIRECT3DTEXTURE9		g_pTextureLogo[WIN_MAX] = {};	//テクスチャのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffLogo = NULL;	//頂点バッファへのポインタ

LOGO	g_Logo;

//=============================================================================
// 初期化処理
//=============================================================================
void InitLogo(void)
{
	LPDIRECT3DDEVICE9 pDevice;
	VERTEX_2D *pVtx;	//頂点情報へのポインタ

	pDevice = GetDevice();	//デバイスの取得

	g_Logo.pos = D3DXVECTOR3(640.0f, 150.0f, 0.0f);
	g_Logo.col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_Logo.type = WIN_1P;
	g_Logo.bUse = false;

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		LOGO_PLAYER1,
		&g_pTextureLogo[WIN_1P]);

	D3DXCreateTextureFromFile(pDevice,
		LOGO_PLAYER2,
		&g_pTextureLogo[WIN_2P]);

	D3DXCreateTextureFromFile(pDevice,
		LOGO_PLAYER2,
		&g_pTextureLogo[WIN_3P]);

	D3DXCreateTextureFromFile(pDevice,
		LOGO_PLAYER3,
		&g_pTextureLogo[WIN_4P]);


	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,	//確保するバッファサイズ(サイズ*頂点数)
		D3DUSAGE_WRITEONLY,		//固定
		FVF_VERTEX_2D,			//頂点フォーマット
		D3DPOOL_MANAGED,		//固定
		&g_pVtxBuffLogo,
		NULL);


	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffLogo->Lock(0, 0, (void**)&pVtx, 0);	//いじるときはロックをかけてから！

	//頂点の設定
	pVtx[0].pos = D3DXVECTOR3(g_Logo.pos.x - (LOGO_SIZE_X / 2), g_Logo.pos.y - (LOGO_SIZE_Y / 2), 0.0f);	//ｚは0.0ｆで固定
	pVtx[1].pos = D3DXVECTOR3(g_Logo.pos.x + (LOGO_SIZE_X / 2), g_Logo.pos.y - (LOGO_SIZE_Y / 2), 0.0f);
	pVtx[2].pos = D3DXVECTOR3(g_Logo.pos.x - (LOGO_SIZE_X / 2), g_Logo.pos.y + (LOGO_SIZE_Y / 2), 0.0f);
	pVtx[3].pos = D3DXVECTOR3(g_Logo.pos.x + (LOGO_SIZE_X / 2), g_Logo.pos.y + (LOGO_SIZE_Y / 2), 0.0f);

	pVtx[0].rhw = 1.0f;	//1.0fで固定
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//頂点カラーの設定
	pVtx[0].col = g_Logo.col;
	pVtx[1].col = g_Logo.col;
	pVtx[2].col = g_Logo.col;
	pVtx[3].col = g_Logo.col;

	//テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点データをアンロック
	g_pVtxBuffLogo->Unlock();
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitLogo(void)
{
	for (int nCntlogo = 0; nCntlogo < WIN_MAX; nCntlogo++)
	{
		//テクスチャの開放
		if (g_pTextureLogo[nCntlogo] != NULL)
		{				  
			g_pTextureLogo[nCntlogo]->Release();
			g_pTextureLogo[nCntlogo] = NULL;
		}
	}
	//頂点バッファの開放
	if (g_pVtxBuffLogo != NULL)
	{
		g_pVtxBuffLogo->Release();
		g_pVtxBuffLogo = NULL;
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateLogo(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawLogo(void)
{
	LPDIRECT3DDEVICE9 pDevice;

	pDevice = GetDevice();	//デバイスの取得

	//頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffLogo, 0, sizeof(VERTEX_2D));

	pDevice->SetFVF(FVF_VERTEX_2D);				//頂点フォーマットの設定
	
	if (g_Logo.bUse)
	{
		pDevice->SetTexture(0, g_pTextureLogo[g_Logo.type]);		//テクスチャの設定

			//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,	//プリミティブの種類
			0,					//開始するインデックス
			2);					//プリミティブの数(ポリゴンの数)
	}
}

//=============================================================================
// ロゴのセット
//=============================================================================
void SetLogo(LOGOTYPE nType)
{
	g_Logo.type = nType;
	g_Logo.bUse = true;
}

//=============================================================================
// ロゴの消去
//=============================================================================
void DeleteLogo(void)
{
	g_Logo.bUse = false;
	g_Logo.pos = D3DXVECTOR3(640.0f, 150.0f, 0.0f);
	g_Logo.col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
}
