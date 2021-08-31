//ヘッダファイル読み込み
#include "game.h"		//ゲーム全体のヘッダファイル
#include "keyboard.h"	//キーボードの処理
#include "FPS.h"		//FPS処理

#include "mouse.h"		//マウス処理
#include "shape.h"		//図形の処理

#include <math.h>		//数学

//マクロ定義
#define TAMA_DIV_MAX	4	//弾の画像の最大数
#define TAMA_MAX		100	//弾の総数
#define TEKI_KIND		8	//敵の種類
#define TEKI_MAX		10	//敵の数

//画像の構造体
struct IMAGE
{
	int handle = -1;	//画像のハンドル(管理番号)
	char path[255];		//画像の場所(パス)
	
	int x;				//x位置
	int y;				//y位置
	int width;			//幅
	int height;			//高さ

	BOOL IsDraw = FALSE;//画像が描画できる?
};

//キャラクタの構造体
struct CHARACTOR
{
	IMAGE img;			//画像構造体
	int speed = 1;		//移動速度
	RECT coll;			//当たり判定の領域(四角)
};

//動画の構造体
struct MOVIE
{
	int handle = -1;	//動画のハンドル(管理番号)
	char path[255];		//動画の場所(パス)
	int x;				//x位置
	int y;				//y位置
	int width;			//幅
	int height;			//高さ

	int Volume = 255;	//ボリューム(最小)0～255(最大)
};

//音楽の構造体
struct AUDIO
{
	int handle = -1;	//音楽ハンドル
	char path[255];		//音楽パス

	int Volume = -1;	//ボリューム (MIN 0～255 MAX)
	int playType = -1;	//BGM or SE

};

//弾の構造体
struct TAMA
{
	int handle[TAMA_DIV_MAX];	//画像のハンドル
	char path[255];	//画像のパス

	int DivTate;	//分割数（縦）
	int DivYoko;	//分割数（横）
	int DivMAX;		//分割総数

	int AnimeCnt = 0;		//アニメーションカウンタ
	int AnimeCntMAX = 0;	//アニメーションカウンタMAX

	int NowIndex = 0;		//現在の画像の要素数

	int StartX;				//X最初の位置
	int StartY;				//Y最初の位置

	float radius;			//半径
	float degree;			//角度

	int x;					//x位置
	int y;					//y位置
	int width;				//幅
	int height;				//高さ

	int Speed;				//速度

	RECT coll;				//当たり判定（短形）

	BOOL IsDraw = FALSE;	//描画できる?
};


//グローバル変数
//シーンを管理する変数
GAME_SCENE GameScene;  //現在のゲームシーン
GAME_SCENE OldGameScene;  //前回のゲームシーン
GAME_SCENE NextGameScene;  //次のゲームシーン

//画像を読み込む
IMAGE TitleLogo;	//タイトルロゴ
IMAGE TitleEnter;	//エンターキーを押してね
IMAGE EndClear;		//クリアロゴ

//音楽
AUDIO TitleBGM;	//
AUDIO PlayBGM;	//
AUDIO EndBGM;	//

//画面の切り替え
BOOL IsFadeOut = FALSE;  //フェードアウト
BOOL IsFadeIn = FALSE;  //フェードイン

//シーン切り替え
int fadeTimeMill = 2000;						//切り替えミリ秒
int fadeTimeMAX = fadeTimeMill / 1000 * 60;	//ミリ秒をフレーム秒に変換

//フェードアウト
int fadeOutCntInit = 0;				//初期化
int fadeOutCnt = fadeOutCntInit;	//フェードアウトのカウンタ
int fadeOutCntMAX = fadeTimeMAX;	//フェードアウトのカウンタMAX

//フェードイン
int fadeInCntInit = fadeTimeMAX;	//初期化
int fadeInCnt = fadeInCntInit;		//フェードインのカウンタ
int fadeInCntMAX = fadeTimeMAX;				//フェードインのカウンタMAX

//PushEnterの点滅
int PushEnterCnt = 0;			//カウント
const int PushEnterCntMAX = 60;		//カウントMAX値
BOOL PushEnterBrink = FALSE;	//点滅しているか？

//弾の構造体
struct TAMA tama_moto;	//元
struct TAMA tama[TAMA_MAX];	//実際に使う

//弾の発射カウンタ
int tamaShotCnt = 0;
int tamaShotCntMAX = 5;

//プレイヤー
CHARACTOR player;

//背景画像
IMAGE back[2];	//背景は2つの画像

//敵データ(元)
CHARACTOR teki_moto[TEKI_KIND];

//実際の敵のデータ
CHARACTOR teki[TEKI_MAX];

//敵データのパス
char tekiPath[TEKI_KIND][255] =
{
	{".\\Image\\teki_blue.png"},
	{".\\Image\\teki_gray.png"},
	{".\\Image\\teki_green.png"},
	{".\\Image\\teki_mizu.png"},
	{".\\Image\\teki_purple.png"},
	{".\\Image\\teki_red.png"},
	{".\\Image\\teki_red_big.png"},
	{".\\Image\\teki_yellow.png"},
};

//敵が出てくるカウント
int TekiAddCnt = 0;
int TekiAddCntMAX = 60;	//60FPSで1回・・・1秒に1回

//ゲームのスコア
int Score = 0;

//プロトタイプ宣言
VOID Title(VOID);		//タイトル画面
VOID TitleProc(VOID);   //タイトル画面(処理)
VOID TitleDraw(VOID);   //タイトル画面(描画)

VOID Play(VOID);		//プレイ画面
VOID PlayProc(VOID);    //プレイ画面(処理)
VOID PlayDraw(VOID);    //プレイ画面(描画)

VOID End(VOID);			//エンド画面
VOID EndProc(VOID);     //エンド画面(処理)
VOID EndDraw(VOID);     //エンド画面(描画)

VOID Change(VOID);		//切り替え画面
VOID ChangeProc(VOID);  //切り替え画面(処理)
VOID ChangeDraw(VOID);  //切り替え画面(描画)

VOID ChangeScene(GAME_SCENE scnen);  //シーンの切り替え

VOID CollUpadatePlayer(CHARACTOR* chara);	//当たり判定の領域を更新
VOID CollUpadateGoll(CHARACTOR* chara);			//当たり判定の領域を更新
VOID CollUpadateTama(TAMA* tama);				//弾の当たり判定更新
VOID CollUpadateTEKI(CHARACTOR* chara);			//敵の当たり判定

BOOL OnCollision(RECT a, RECT b);				//短形と短形の当たり判定

BOOL GameLoad(VOID);		//ゲームのデータを読み込み

BOOL LoadImageMem(IMAGE* image, const char* path);										//ゲームの画像の読み込み
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType);				//ゲームの音楽を読み込み
BOOL LoadImageDivMem(int* handle, const char* path, int bunkatuYoko, int bunkatuTate);	//ゲームの画像の分割読み込み

VOID GameInit(VOID);		//ゲームのデータの初期化

VOID DrawTama(TAMA* tama);	//弾の描画
VOID ShotTama(TAMA* tama, float deg);	//弾の発射

// プログラムは WinMain から始まります
//Windowsのプログラミング方法 = (WinAPI)で動いている
//DxLibは、DirecttXという、ゲームプログラミングを簡単に使える仕組み
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);  //ウィンドウモードに設定
	SetMainWindowText(GAME_TITLE);  //ウィンドウのタイトルの文字
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);  //ウィンドウの解像度を設定
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);  //ウィンドウの大きさを設定
	SetBackgroundColor(255, 255, 255);  //デフォルトの背景の色
	SetWindowIconID(GAME_ICON_ID);  //アイコンファイルの読込
	SetWindowStyleMode(GAME_WINDOW_BAR);  //ウィンドウバーの状態
	SetWaitVSyncFlag(TRUE);  //ディスプレイの垂直同期を有効にする
	SetAlwaysRunFlag(TRUE);  //ウィンドウをずっとアクティブにする


	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}


	//ダブルバッファリング有効化
	SetDrawScreen(DX_SCREEN_BACK);



	//最初のシーンは、タイトル画面から
	GameScene = GAME_SCENE_TITLE;

	//ゲーム全体の初期化

	//ゲーム読み込み
	if(!GameLoad())
	{
		//データの読み込みに失敗したとき
		DxLib_End();	//DxLib終了
		return -1;		//異常終了
	}
	
	//ゲームの初期化
	GameInit();

	//無限ループ
	while (1)
	{
		if (ProcessMessage() != 0) { break; }  //メッセージを受け取り続ける
		if (ClearDrawScreen() != 0) { break; }  //画面を消去する


		//キーボード入力の更新
		AllKeyUpdate();

		//マウス入力の更新
		MouseUpdate();

		//FPS値の更新
		FPSUpdate();

		//ESCキーで強制終了
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//以前のシーンを取得
		if (GameScene != GAME_SCENE_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:  //タイトル画面
			Title();
			break;
		case GAME_SCENE_PLAY:  //プレイ画面
			Play();
			break;
		case GAME_SCENE_END:  //エンド画面
			End();
			break;
		case GAME_SCENE_CHANGE:  //切り替え画面
			Change();
			break;
		default:
			break;
		}

		//シーンを切り替える
		if (OldGameScene != GameScene)
		{
			//現在のシーンが切り替え場面でないとき
			if (GameScene != GAME_SCENE_CHANGE)
			{
				NextGameScene = GameScene;  //次のシーンを保存
				GameScene = GAME_SCENE_CHANGE;  //画面切り替えシーンに変える
			}
		}

		//FPS値を描画
		FPSDraw();

		//FPS値を待つ
		FPSWait();

		ScreenFlip();  //ダブルバッファリングした画面を描画

	}

	//読み込んだ画像を開放
	for (int i = 0; i < TAMA_DIV_MAX; i++) { DeleteGraph(tama_moto.handle[i]); }

	//プレイヤーの解放
	DeleteGraph(player.img.handle);

	//背景画像の解放
	DeleteGraph(back[0].handle);
	DeleteGraph(back[1].handle);

	//敵の画像の解放
	for (int i = 0; i < TEKI_KIND; i++)
	{
		DeleteGraph(teki_moto[i].img.handle);
	}

	DeleteGraph(TitleLogo.handle);		//画像をメモリ上から削除
	DeleteGraph(TitleEnter.handle);		//画像をメモリ上から削除
	DeleteGraph(EndClear.handle);		//画像をメモリ上から削除

	DeleteSoundMem(TitleBGM.handle);	//音楽をメモリ上から削除
	DeleteSoundMem(PlayBGM.handle);		//音楽をメモリ上から削除
	DeleteSoundMem(EndBGM.handle);		//音楽をメモリ上から削除

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}


/// <summary>
/// ゲームのデータを読み込み
/// </summary>
/// <returns>読み込めたらTRUE / 読み込めなかったらFALSE</returns>
BOOL GameLoad(VOID)
{
	//弾の分割数を設定
	tama_moto.DivYoko = 4;
	tama_moto.DivTate = 1;

	//弾のパス
	strcpyDx(tama_moto.path, ".\\Image\\dia_pink.png");

	//画像を分割して読み込み
	if (LoadImageDivMem(&tama_moto.handle[0], tama_moto.path, tama_moto.DivYoko, tama_moto.DivTate) == FALSE) { return FALSE; }

	//幅と高さを取得
	GetGraphSize(tama_moto.handle[0], &tama_moto.width, &tama_moto.height);

	//位置を設定
	tama_moto.x = GAME_WIDTH / 2 - tama_moto.width / 2;	//中央揃え
	tama_moto.y = GAME_HEIGHT - tama_moto.height;		//画面下

	//速度
	tama_moto.Speed = 5;

	//アニメを変える速度
	tama_moto.AnimeCntMAX = 10;

	//当たり判定の更新
	CollUpadateTama(&tama_moto);

	//画像の表示をしない
	tama_moto.IsDraw = FALSE;

	//全ての弾に情報をコピー
	for (int i = 0; i < TAMA_MAX; i++)
	{
		tama[i] = tama_moto;
	}

	//プレイヤーの画像を読み込み
	if (LoadImageMem(&player.img, ".\\Image\\player.png") == FALSE) { return FALSE; }
	player.img.x = GAME_WIDTH / 2 - player.img.width;
	player.img.y = GAME_HEIGHT / 2 - player.img.height;
	CollUpadatePlayer(&player);	//当たり判定の更新
	player.img.IsDraw = TRUE;	//描画する

	//背景の画像の読込①
	if (LoadImageMem(&back[0], ".\\Image\\hoshi.png") == FALSE) { return FALSE; }
	back[0].x = 0;
	back[0].y = -back[0].height;	//画像の高さ分、位置を上にあげる
	back[0].IsDraw = TRUE;	//描画する

		//背景の画像の読込②
	if (LoadImageMem(&back[1], ".\\Image\\hoshi_rev.png") == FALSE) { return FALSE; }
	back[1].x = 0;
	back[1].y = 0;
	back[1].IsDraw = TRUE;	//描画する

	//敵の画像を読み込む
	for (int i = 0; i < TEKI_KIND; i++)
	{
		if (LoadImageMem(&teki_moto[i].img, ".\\Image\\teki_blue.png") == FALSE) { return FALSE; }
		teki_moto[0].img.x = GAME_WIDTH / 2 - teki_moto[i].img.width;
		teki_moto[0].img.y = -teki_moto[i].img.height;
		CollUpadatePlayer(&teki_moto[i]);	//当たり判定の更新
		teki_moto[0].img.IsDraw = FALSE;	//描画する
	}

	//ロゴを読み込む
	if (!LoadImageMem(&TitleLogo, ".\\Image\\タイトルロゴ.png")) { return FALSE; }
	if (!LoadImageMem(&TitleEnter, ".\\Image\\PushEnter.png")) { return FALSE; }
	if (!LoadImageMem(&EndClear, ".\\Image\\Clear.png")) { return FALSE; }

	//音楽を読み込む
	if (!LoadAudio(&TitleBGM, ".\\Audio\\ARPCHAIN.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&PlayBGM, ".\\Audio\\Shooting_Stars_2.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&EndBGM, ".\\Audio\\Milky_Way.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }

	return TRUE;	//	全て読み込めた!
}

/// <summary>
/// 画像を分割してメモリに読み込み
/// </summary>
/// <param name="handle">ハンドル配列の先頭アドレス</param>
/// <param name="path">画像のパス</param>
/// <param name="bunkatuYoko">分割するときの横の数</param>
/// <param name="bunkatuTate">分割するときの縦の数</param>
/// <returns></returns>
BOOL LoadImageDivMem(int* handle, const char* path, int bunkatuYoko, int bunkatuTate)
{

	//弾の読み込み
	int IsTamaLoad = -1;	//画像が読み込めたか?

	//一時的に使うハンドル
	int TamaHandle = LoadGraph(path);

	//読み込みエラー
	if (TamaHandle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//ウィンドウハンドル
			path,				//本文
			"画像読み込みエラー！",	//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//画像の幅と高さを取得
	int TamaWidth = -1;
	int TamaHeight = -1;
	GetGraphSize(TamaHandle, &TamaWidth, &TamaHeight);

	//分割して読み込み
	IsTamaLoad = LoadDivGraph(
		".\\Image\\dia_pink.png",			//画像のパス	
		TAMA_DIV_MAX,					//分割総数
		bunkatuYoko, bunkatuTate,		//横の分割,縦の分割
		TamaWidth / 4, TamaHeight / 1,	//画像1つ分の幅,高さ
		handle							//連続で管理する配列の先頭アドレス
	);

	//分割エラー
	if (TamaHandle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//ウィンドウハンドル
			path,				//本文
			"画像分割エラー！",	//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}
	
	//一時的に読み込んだハンドルを解放
	DeleteGraph(TamaHandle);

	return TRUE;
}



/// <summary>
/// 画像をメモリに読み込み
/// </summary>
/// <param name="image">画像構造体のアドレス</param>
/// <param name="path">画像のパス</param>
/// <returns></returns>
BOOL LoadImageMem(IMAGE* image, const char* path)
{
	//ゴールの画像を読み込み
	strcpyDx(image->path, path);	//パスのコピー
	image->handle = LoadGraph(image->path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (image->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			image->path,			//メッセージ本文
			"画像読み込みエラー！",		//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//画像の幅と高さを取得
	GetGraphSize(image->handle, &image->width, &image->height);

	//読み込めた
	return TRUE;
}

/// <summary>
/// 音楽をメモリに読み込み
/// </summary>
/// <param name="audio">Audio構造体変数のアドレス</param>
/// <param name="path">Audioの音楽パス</param>
/// <param name="volume">ボリューム</param>
/// <param name="playType">DX_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType)
{
	//音楽の読み込み
	strcpyDx(audio->path, path);					//パスのコピー
	audio->handle = LoadSoundMem(audio->path);		//音楽の読み込み

	//音楽が読み込めなかったときは、エラー(-1)が入る
	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			audio->path,			//メッセージ本文
			"音楽読み込みエラー！",		//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//その他の設定
	audio->Volume = volume;
	audio->playType = playType;


	return TRUE;
}

/// <summary>
/// ゲームデータの初期化
/// </summary>
/// <param name=""></param>
VOID GameInit(VOID)
{
	//ゲームスコアの初期化
	Score = 0;

	player.img.x = GAME_WIDTH / 2 - player.img.width;
	player.img.y = GAME_HEIGHT / 2 - player.img.height;
	CollUpadatePlayer(&player);	//当たり判定の更新
	player.speed = 6;
	player.img.IsDraw = TRUE;	//描画する

		//背景の画像の読込①
	back[0].x = 0;
	back[0].y = -back[0].height;	//画像の高さ分、位置を上にあげる
	back[0].IsDraw = TRUE;	//描画する

		//背景の画像の読込②
	back[1].x = 0;
	back[1].y = 0;
	back[1].IsDraw = TRUE;	//描画する

	//敵の初期化
	for (int i = 0; i < TEKI_KIND; i++)
	{
		teki_moto[0].img.x = GAME_WIDTH / 2 - teki_moto[i].img.width;
		teki_moto[0].img.y = -teki_moto[i].img.height;
		CollUpadatePlayer(&teki_moto[i]);	//当たり判定の更新
		teki_moto[0].img.IsDraw = FALSE;	//描画する
	}

	//タイトルロゴの位置を決める
	TitleLogo.x = GAME_WIDTH / 2 - TitleLogo.width / 2; //中央揃え
	TitleLogo.x = 100;

	//PusHEnterの位置を決める
	TitleEnter.x = GAME_WIDTH / 2 - TitleEnter.width / 2;	//中央揃え
	TitleEnter.y = GAME_HEIGHT - TitleEnter.height - 100;

	//PushEnterの点滅
	int PushEnterCnt = 0;			//カウント
	//int PushEnterCntMAX = 60;		初期化しなくていい?
	BOOL PushEnterBrink = FALSE;	//点滅しているか？

	//クリアロゴの位置を決める
	EndClear.x = GAME_WIDTH / 2 - EndClear.width / 2;	//中央揃え
	EndClear.y = GAME_HEIGHT / 2 - EndClear.height / 2;		//中央揃え

}

VOID ChangeScene(GAME_SCENE scene)
{
	GameScene = scene;				//シーンを切り替え
	IsFadeIn = FALSE;				//フェードインしない
	IsFadeOut = TRUE;				//フェードアウトする

	return;
}

/// <summary>
/// タイトル画面
/// </summary>
VOID Title(VOID)
{
	TitleProc();  //処理
	TitleDraw();  //描画

	return;
}

/// <summary>
/// タイトル画面の処理
/// </summary>
VOID TitleProc(VOID)
{
	//プレイシーンへ切り替える
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGMを止める
		StopSoundMem(TitleBGM.handle);

		//ゲームの初期化
		GameInit();

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);

		//マウスを描画しない
		SetMouseDispFlag(FALSE);

		return;
	}
	//BGMが流れていないとき
	if (CheckSoundMem(TitleBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(TitleBGM.handle, TitleBGM.playType);
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	//タイトルロゴの描画
	DrawGraph(TitleLogo.x, TitleLogo.y, TitleLogo.handle, TRUE);

	//MAX値までを持つ
	if (PushEnterCnt < PushEnterCntMAX) { PushEnterCnt++; }
	else
	{
		if (PushEnterBrink == TRUE)PushEnterBrink = FALSE;
		else if (PushEnterBrink == FALSE)PushEnterBrink = TRUE;

		PushEnterCnt = 0;	//カウントを初期化
	}

	//PushEnterを描画
	if (PushEnterBrink == TRUE)
	{
		//半透明にする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)PushEnterCnt / PushEnterCntMAX) * 255);

		//PushEnterの描画
		DrawGraph(TitleEnter.x, TitleEnter.y, TitleEnter.handle, TRUE);

		//半透明終了
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (PushEnterBrink == FALSE)
	{
		//半透明にする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)(PushEnterCntMAX - PushEnterCnt) / PushEnterCntMAX) * 255);

		//PushEnterの描画
		DrawGraph(TitleEnter.x, TitleEnter.y, TitleEnter.handle, TRUE);

		//半透明終了
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 弾の描画
/// </summary>
/// <param name="tama">弾の構造体</param>
VOID DrawTama(TAMA* tama)
{
	//弾の描画ができるとき
	if (tama->IsDraw == TRUE)
	{
		//弾の描画
		DrawGraph(tama->x, tama->y, tama->handle[tama->NowIndex], TRUE);

		if (tama->AnimeCnt < tama->AnimeCntMAX)
		{
			tama->AnimeCnt++;
		}
		else
		{
			//弾の添字が弾の分割数の最大よりも小さいとき
			if (tama->NowIndex < TAMA_DIV_MAX - 1)
			{
				tama->NowIndex++;	//次の画像へ
			}
			else
			{
				tama->NowIndex = 0;	//最初に戻す
			}

			tama->AnimeCnt = 0;
		}
	}
}


/// <summary>
/// プレイ画面
/// </summary>
VOID Play(VOID)
{
	PlayProc();  //処理
	PlayDraw();  //描画

	return;
}

/// <summary>
/// プレイ画面の処理
/// </summary>
VOID PlayProc(VOID)
{
	//プレイシーンへ切り替える
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGMを止める
		StopSoundMem(PlayBGM.handle);

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);

		return;
	}

	/*
	//プレイヤーを操作する
	if (KeyDown(KEY_INPUT_LEFT) == TRUE)
	{
		if (player.img.x - player.speed >= 0)
		{
			player.img.x -= player.speed;
			}
	}
	if (KeyDown(KEY_INPUT_RIGHT) == TRUE)
	{
		if (player.img.x + player.img.width + player.speed <= GAME_WIDTH)
		{
			player.img.x += player.speed;
		}
	}
	if (KeyDown(KEY_INPUT_UP) == TRUE)
	{
		if (player.img.y - player.speed >= 0)
		{
			player.img.y -= player.speed;
		}
	}
	if (KeyDown(KEY_INPUT_DOWN) == TRUE)
	{
		if (player.img.y + player.img.height + player.speed <= GAME_HEIGHT)
		{
			player.img.y += player.speed;
		}
	}
	*/

	//マウスの位置にプレイヤーを置く
	player.img.x = mouse.Point.x - player.img.width / 2;	//マウスの位置を画像の中心にする
	player.img.y = mouse.Point.y - player.img.height / 2;	//マウスの位置を画像の中心にする

	//プレイヤーの当たり判定
	CollUpadatePlayer(&player);

	//スペースキーを押しているとき
	/*
	if (KeyDown(KEY_INPUT_SPACE) == TRUE)
	*/
	
	//マウスの左ボタンを押しているとき
	if (MouseDown(MOUSE_INPUT_LEFT) == TRUE)
	{
		if (tamaShotCnt == 0)
		{
			//全体
			for (int deg = 240; deg <= 300; deg += 30)
			{
				//弾を発射する（弾を描画する）
				for (int i = 0; i < TAMA_MAX; i++)
				{
					if (tama[i].IsDraw == FALSE)
					{
						ShotTama(&tama[i], deg);
							
						//弾を1発出したら、ループを抜ける
						break;
					}


				}
			}
		}

		//弾の発射待ち
		if (tamaShotCnt < tamaShotCntMAX)
		{
			tamaShotCnt++;
		}
		else
		{
			tamaShotCnt = 0;
		}
	}




	//弾を飛ばす
	for (int i = 0; i < TAMA_MAX; i++)
	{
		//描画されているとき
		if (tama[i].IsDraw == TRUE)
		{
			//弾の位置を修正
			//中心位置　＋　飛ばす角度→飛ばす距離を計算　*　距離
			tama[i].x = tama[i].StartX + cos(tama[i].degree * DX_PI / 180.0f) * tama[i].radius;
			tama[i].y = tama[i].StartY + sin(tama[i].degree * DX_PI / 180.0f) * tama[i].radius;

			//半径を足す
			tama[i].radius += tama[i].Speed;

				//弾の当たり判定を更新
				CollUpadateTama(&tama[i]);

				//画面買いに出たら、描画しない
				if (tama[i].y + tama[i].height < 0 ||	//画面外（上）
					tama[i].y > GAME_HEIGHT ||			//画面外（下）
					tama[i].x + tama[i].width < 0 ||	//画面外（左）
					tama[i].x > GAME_WIDTH)				//画面外（右）
				{
					tama[i].IsDraw = FALSE;
				}

			}
		}

		if (TekiAddCnt < TekiAddCntMAX)
		{
			TekiAddCnt++;
		}
		else
		{
			TekiAddCnt = 0;

			//敵を生成
			for (int i = 0; i < TEKI_MAX; i++)
			{
				//描画されていない敵を探す
				if (teki[i].img.IsDraw == FALSE)
				{
					teki[i] = teki_moto[0];

					teki[i].img.x = GetRand(3) * GAME_WIDTH / 4;
					teki[i].img.y = -teki[i].img.height;

					teki[i].img.IsDraw = TRUE;	//描画する
					break;

					int Bunkatu = 10;	//画面の横分割数

					if (Score < 1000)
					{
						teki[i] = teki_moto[0];
					}
					else if (Score < 2000)
					{
						teki[i] = teki_moto[1];
					}
					else
					{
						teki[i] = teki_moto[GetRand(TEKI_KIND - 1)];
					}

					teki[i].img.x = GetRand(Bunkatu - 1) * GAME_WIDTH / Bunkatu;
					teki[i].img.y = -teki[i].img.height;
				}
			}
		}

		//敵の処理
		for (int i = 0; i < TEKI_MAX; i++)
		{
			//描画している敵
			if (teki[i].img.IsDraw == TRUE)
			{
				teki[i].img.y += 1;	//とりあえず下に移動

				//敵の当たり判定更新
				CollUpadateTEKI(&teki[i]);

				//敵が下まで行ったら表示しない
				if (teki[i].img.y > GAME_HEIGHT)
				{
					teki[i].img.IsDraw = FALSE;
				}

				//弾を飛ばす
				for (int cnt = 0; cnt < TAMA_MAX; cnt++)
				{
					//描画されているとき
					if (tama[cnt].IsDraw == TRUE)
					{
						//当たり判定
						if (OnCollision(teki[i].coll, tama[cnt].coll) == TRUE)
						{
							tama[cnt].IsDraw = FALSE;		//弾の描画はしない
							teki[i].img.IsDraw = FALSE;	//敵の描画もしない

							Score += 100;	//スコア加算
						}
					}
				}
			}

			
		}
		//BGMが流れていないとき
		if (CheckSoundMem(PlayBGM.handle) == 0)
		{
			//BGMを流す
			PlaySoundMem(PlayBGM.handle, PlayBGM.playType);
		}

	return;
}

/// <summary>
/// 弾を飛ばす
/// </summary>
/// <param name="tama"></param>
VOID ShotTama(TAMA* tama, float deg)
{
		//弾を発射する（描画する）
		tama->IsDraw = TRUE;

		//弾の位置を決める
		tama->StartX = player.img.x + player.img.width / 2 - tama->width / 2;
		tama->StartY = player.img.y;

		//弾の速度を変える
		tama->Speed = 6;

		//弾の角度
		tama->degree = deg;

		//弾の半径
		tama->radius = 0.0f;

		//弾の当たり判定を更新
		CollUpadateTama(tama);
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	//背景の描画
	for  (int i = 0; i < 2; i++)
	{
		//描画
		DrawGraph(back[i].x, back[i].y, back[i].handle, TRUE);

		//画像が下まで行ったとき
		if (back[i].y > GAME_HEIGHT)
		{
			back[i].y = -back[i].height + 1;	//高さ分、上に戻す
		}

		//画像を下に動かす
		back[i].y++;
	}

	//敵の描画
	for (int i = 0; i < TEKI_MAX; i++)
	{
		/*
		if (teki[i].img.IsDraw == TRUE)
		{
			DrawGraph(teki[i].img.x, teki[i].img.y, teki[i].img.handle, TRUE);
		}

		//当たり判定
		if (GAME_DEBUG == TRUE)
		{
			DrawBox(
				teki[i].coll.left, teki[i].coll.top, teki[i].coll.right, teki[i].coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
		*/

		//敵が描画できるときは
		if (teki[i].img.IsDraw == TRUE)
		{
			DrawGraph(teki[i].img.x, teki[i].img.y, teki[i].img.handle, TRUE);

			//当たり判定
			if (GAME_DEBUG == TRUE)
			{
				DrawBox(
					teki[i].coll.left, teki[i].coll.top, teki[i].coll.right, teki[i].coll.bottom,
					GetColor(255, 0, 0), FALSE);
			}
		}
	}

	//プレイヤーの描画
	if (player.img.IsDraw == TRUE)
	{
		DrawGraph(player.img.x, player.img.y, player.img.handle, TRUE);

		//当たり判定
		if (GAME_DEBUG == TRUE)
		{
			DrawBox(
				player.coll.left, player.coll.top, player.coll.right, player.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}

	//弾の描画
	for (int i = 0; i < TAMA_MAX; i++)
	{
		//描画されているとき
		if (tama[i].IsDraw == TRUE)
		{
			DrawTama(&tama[i]);

			//当たり判定
			if (GAME_DEBUG == TRUE)
			{
				DrawBox(
					tama[i].coll.left, tama[i].coll.top, tama[i].coll.right, tama[i].coll.bottom,
					GetColor(255, 0, 0), FALSE);
			}
		}
	}

	//スコアの描画
	int old = GetFontSize();	//以前のフォントサイズを取得
	SetFontSize(40);			//フォントを大きくする
	DrawFormatString(0, 100, GetColor(255, 255, 255), "SCORE:%05d", Score);
	SetFontSize(old);			//フォントをもとに戻す

	//マウスの位置を描画
	MouseDraw();


	DrawString(0, 0, "プレイ画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// エンド画面
/// </summary>
VOID End(VOID)
{
	EndProc();  //処理
	EndDraw();  //描画

	return;
}

/// <summary>
/// エンド画面の処理
/// </summary>
VOID EndProc(VOID)
{
	//プレイシーンへ切り替える
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGMを止める
		StopSoundMem(EndBGM.handle);

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}
	//BGMが流れていないとき
	if (CheckSoundMem(EndBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(EndBGM.handle, EndBGM.playType);
	}

	return;
}

/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	//EndClearの描画
	DrawGraph(EndClear.x, EndClear.y, EndClear.handle, TRUE);


	DrawString(0, 0, "エンド画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 切り替え画面
/// </summary>
VOID Change(VOID)
{
	ChangeProc();  //処理
	ChangeDraw();  //描画

	return;
}

/// <summary>
/// 切り替え画面の処理
/// </summary>
VOID ChangeProc(VOID)
{
	//フェードイン
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMAX)
		{
			fadeInCnt--;	//カウンタを減らす
		}
		else
		{
			//フェードイン処理が終わった

			fadeInCnt = fadeInCntInit;	//カウンタ初期化
			IsFadeIn = FALSE;			//フェードイン処理終了
		}

	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMAX)
		{
			fadeOutCnt++;	//カウンタを増やす
		}
		else
		{
			//フェードアウト処理が終わった

			fadeOutCnt = fadeOutCntInit;	//カウンタ初期化
			IsFadeOut = FALSE;				//フェードアウト処理終了
		}

	}

	//切り替え処理終了?
	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//フェードインしていない、フェードアウトもしていないとき
		GameScene = NextGameScene;	//次のシーンに切り替え
		OldGameScene = GameScene;	//以前のゲームシーン更新
	}


	return;
}

/// <summary>
/// 切り替え画面の描画
/// </summary>
VOID ChangeDraw(VOID)
{
	//以前のシーンを描画
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();	//タイトル画面の描画
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();		//プレイ画面の描画
		break;
	case GAME_SCENE_END:
		EndDraw();		//エンド画面の描画
		break;
	default:
		break;
	}

	//フェードイン
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMAX) * 255);
	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMAX) * 255);
	}

	//四角を描画
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//半透明終了
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(0, 16, "切り替え画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="coll">当たり判定の領域</param>
VOID CollUpadatePlayer(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x + 10;					//当たり判定微調整
	chara->coll.top = chara->img.y + 10;						//当たり判定微調整

	chara->coll.right = chara->img.x + chara->img.width - 10;	//当たり判定微調整
	chara->coll.bottom = chara->img.y + chara->img.height - 10;	//当たり判定微調整

	return;
}

/// <summary>
/// 当たり判定の領域を更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpadateTEKI(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;					//当たり判定微調整
	chara->coll.top = chara->img.y;						//当たり判定微調整

	chara->coll.right = chara->img.x + chara->img.width;	//当たり判定微調整
	chara->coll.bottom = chara->img.y + chara->img.height;	//当たり判定微調整

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="tama">当たり判定の領域</param>
VOID CollUpadateTama(TAMA* tama)
{
	tama->coll.left = tama->x;					//当たり判定微調整
	tama->coll.top = tama->y;						//当たり判定微調整

	tama->coll.right = tama->x + tama->width;	//当たり判定微調整
	tama->coll.bottom = tama->y + tama->height;	//当たり判定微調整

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="coll">当たり判定の領域</param>
VOID CollUpadateGoll(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;					//当たり判定微調整
	chara->coll.top = chara->img.y;						//当たり判定微調整

	chara->coll.right = chara->img.x + chara->img.width - 40;	//当たり判定微調整
	chara->coll.bottom = chara->img.y + chara->img.height;	//当たり判定微調整

	return;
}

BOOL OnCollision(RECT a, RECT b)
{
	if (
		a.left < b.right &&	//　短形Aの左辺x座標 < 短形Bの右辺x座標　かつ
		a.top < b.bottom &&	//　短形Aの上辺y座標 < 短形Bの下辺y座標　かつ
		a.right > b.left &&	//　短形Aの右辺x座標 > 短形Bの左辺x座標　かつ
		a.bottom > b.top	//　短形Aの下辺y座標 > 短形Bの上辺y座標
		)
	{
		//当たっているとき

		return TRUE;
	}
	else
	{
		//当たっていないとき
		return FALSE;
	}
}

