

//�w�b�_�t�@�C���ǂݍ���
#include "game.h"		//�Q�[���S�̂̃w�b�_�t�@�C��
#include "keyboard.h"	//�L�[�{�[�h�̏���
#include "FPS.h"		//FPS����

//�摜�̍\����
struct IMAGE
{
	int handle = -1;	//�摜�̃n���h��(�Ǘ��ԍ�)
	char path[255];		//�摜�̏ꏊ(�p�X)
	
	int x;				//x�ʒu
	int y;				//y�ʒu
	int width;			//��
	int height;			//����

	BOOL IsDraw = FALSE;//�摜���`��ł���?
};

//�L�����N�^�̍\����
struct CHARACTOR
{
	IMAGE img;			//�摜�\����
	int speed = 1;		//�ړ����x
	RECT coll;			//�����蔻��̗̈�(�l�p)
};

//����̍\����
struct MOVIE
{
	int handle = -1;	//����̃n���h��(�Ǘ��ԍ�)
	char path[255];		//����̏ꏊ(�p�X)
	int x;				//x�ʒu
	int y;				//y�ʒu
	int width;			//��
	int height;			//����

	int Volume = 255;	//�{�����[��(�ŏ�)0�`255(�ő�)
};

//���y�̍\����
struct AUDIO
{
	int handle = -1;	//���y�n���h��
	char path[255];		//���y�p�X

	int Volume = -1;	//�{�����[�� (MIN 0�`255 MAX)
	int playType = -1;	//BGM or SE

};




//�O���[�o���ϐ�
//�V�[�����Ǘ�����ϐ�
GAME_SCENE GameScene;  //���݂̃Q�[���V�[��
GAME_SCENE OldGameScene;  //�O��̃Q�[���V�[��
GAME_SCENE NextGameScene;  //���̃Q�[���V�[��

//��ʂ̐؂�ւ�
BOOL IsFadeOut = FALSE;  //�t�F�[�h�A�E�g
BOOL IsFadeIn = FALSE;  //�t�F�[�h�C��

//�V�[���؂�ւ�
int fadeTimeMill = 2000;						//�؂�ւ��~���b
int fadeTimeMAX = fadeTimeMill / 1000 * 60;	//�~���b���t���[���b�ɕϊ�

//�t�F�[�h�A�E�g
int fadeOutCntInit = 0;				//������
int fadeOutCnt = fadeOutCntInit;	//�t�F�[�h�A�E�g�̃J�E���^
int fadeOutCntMAX = fadeTimeMAX;	//�t�F�[�h�A�E�g�̃J�E���^MAX

//�t�F�[�h�C��
int fadeInCntInit = fadeTimeMAX;	//������
int fadeInCnt = fadeInCntInit;		//�t�F�[�h�C���̃J�E���^
int fadeInCntMAX = fadeTimeMAX;				//�t�F�[�h�C���̃J�E���^MAX


//�v���g�^�C�v�錾
VOID Title(VOID);		//�^�C�g�����
VOID TitleProc(VOID);   //�^�C�g�����(����)
VOID TitleDraw(VOID);   //�^�C�g�����(�`��)

VOID Play(VOID);		//�v���C���
VOID PlayProc(VOID);    //�v���C���(����)
VOID PlayDraw(VOID);    //�v���C���(�`��)

VOID End(VOID);			//�G���h���
VOID EndProc(VOID);     //�G���h���(����)
VOID EndDraw(VOID);     //�G���h���(�`��)

VOID Change(VOID);		//�؂�ւ����
VOID ChangeProc(VOID);  //�؂�ւ����(����)
VOID ChangeDraw(VOID);  //�؂�ւ����(�`��)

VOID ChangeScene(GAME_SCENE scnen);  //�V�[���̐؂�ւ�

VOID CollUpadateGollPlayer(CHARACTOR* chara);	//�����蔻��̗̈���X�V
VOID CollUpadateGoll(CHARACTOR* chara);			//�����蔻��̗̈���X�V

BOOL OnCollision(RECT a, RECT b);				//�Z�`�ƒZ�`�̓����蔻��

BOOL GameLoad(VOID);		//�Q�[���̃f�[�^��ǂݍ���

BOOL LoadImageMem(IMAGE* image, const char* path);
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType);

VOID GameInit(VOID);		//�Q�[���̃f�[�^�̏�����



// �v���O������ WinMain ����n�܂�܂�
//Windows�̃v���O���~���O���@ = (WinAPI)�œ����Ă���
//DxLib�́ADirecttX�Ƃ����A�Q�[���v���O���~���O���ȒP�Ɏg����d�g��
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);  //�E�B���h�E���[�h�ɐݒ�
	SetMainWindowText(GAME_TITLE);  //�E�B���h�E�̃^�C�g���̕���
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);  //�E�B���h�E�̉𑜓x��ݒ�
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);  //�E�B���h�E�̑傫����ݒ�
	SetBackgroundColor(255, 255, 255);  //�f�t�H���g�̔w�i�̐F
	SetWindowIconID(GAME_ICON_ID);  //�A�C�R���t�@�C���̓Ǎ�
	SetWindowStyleMode(GAME_WINDOW_BAR);  //�E�B���h�E�o�[�̏��
	SetWaitVSyncFlag(TRUE);  //�f�B�X�v���C�̐���������L���ɂ���
	SetAlwaysRunFlag(TRUE);  //�E�B���h�E�������ƃA�N�e�B�u�ɂ���


	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}


	//�_�u���o�b�t�@�����O�L����
	SetDrawScreen(DX_SCREEN_BACK);



	//�ŏ��̃V�[���́A�^�C�g����ʂ���
	GameScene = GAME_SCENE_TITLE;

	//�Q�[���S�̂̏�����

	//�Q�[���ǂݍ���
	if(!GameLoad())
	{
		//�f�[�^�̓ǂݍ��݂Ɏ��s�����Ƃ�
		DxLib_End();	//DxLib�I��
		return -1;		//�ُ�I��
	}
	
	//�Q�[���̏�����
	GameInit();

	//�������[�v
	while (1)
	{


		if (ProcessMessage() != 0) { break; }  //���b�Z�[�W���󂯎�葱����
		if (ClearDrawScreen() != 0) { break; }  //��ʂ���������



		//�L�[�{�[�h���͂̍X�V
		AllKeyUpdate();

		//FPS�l�̍X�V
		FPSUpdate();

		//ESC�L�[�ŋ����I��
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//�ȑO�̃V�[�����擾
		if (GameScene != GAME_SCENE_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//�V�[�����Ƃɏ������s��
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:  //�^�C�g�����
			Title();
			break;
		case GAME_SCENE_PLAY:  //�v���C���
			Play();
			break;
		case GAME_SCENE_END:  //�G���h���
			End();
			break;
		case GAME_SCENE_CHANGE:  //�؂�ւ����
			Change();
			break;
		default:
			break;
		}

		//�V�[����؂�ւ���
		if (OldGameScene != GameScene)
		{
			//���݂̃V�[�����؂�ւ���ʂłȂ��Ƃ�
			if (GameScene != GAME_SCENE_CHANGE)
			{
				NextGameScene = GameScene;  //���̃V�[����ۑ�
				GameScene = GAME_SCENE_CHANGE;  //��ʐ؂�ւ��V�[���ɕς���
			}
		}





		//FPS�l��`��
		FPSDraw();

		//FPS�l��҂�
		FPSWait();

		ScreenFlip();  //�_�u���o�b�t�@�����O������ʂ�`��

	}

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}


/// <summary>
/// �Q�[���̃f�[�^��ǂݍ���
/// </summary>
/// <returns>�ǂݍ��߂���TRUE / �ǂݍ��߂Ȃ�������FALSE</returns>
BOOL GameLoad(VOID)
{

	return TRUE;	//	�S�ēǂݍ��߂�!
}

/// <summary>
/// �摜���������ɓǂݍ���
/// </summary>
/// <param name="image">�摜�\���̂̃A�h���X</param>
/// <param name="path">�摜�̃p�X</param>
/// <returns></returns>
BOOL LoadImageMem(IMAGE* image, const char* path)
{
	//�S�[���̉摜��ǂݍ���
	strcpyDx(image->path, path);	//�p�X�̃R�s�[
	image->handle = LoadGraph(image->path);	//�摜�̓ǂݍ���

	//�摜���ǂݍ��߂Ȃ������Ƃ��́A�G���[(-1)������
	if (image->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//���C���̃E�B���h�E�n���h��
			image->path,			//���b�Z�[�W�{��
			"�摜�ǂݍ��݃G���[�I",		//���b�Z�[�W�^�C�g��
			MB_OK					//�{�^��
		);

		return FALSE;	//�ǂݍ��ݎ��s
	}

	//�摜�̕��ƍ������擾
	GetGraphSize(image->handle, &image->width, &image->height);

	//�ǂݍ��߂�
	return TRUE;
}

/// <summary>
/// ���y���������ɓǂݍ���
/// </summary>
/// <param name="audio">Audio�\���̕ϐ��̃A�h���X</param>
/// <param name="path">Audio�̉��y�p�X</param>
/// <param name="volume">�{�����[��</param>
/// <param name="playType">DX_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType)
{
	//���y�̓ǂݍ���
	strcpyDx(audio->path, path);					//�p�X�̃R�s�[
	audio->handle = LoadSoundMem(audio->path);		//���y�̓ǂݍ���

	//���y���ǂݍ��߂Ȃ������Ƃ��́A�G���[(-1)������
	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//���C���̃E�B���h�E�n���h��
			audio->path,			//���b�Z�[�W�{��
			"���y�ǂݍ��݃G���[�I",		//���b�Z�[�W�^�C�g��
			MB_OK					//�{�^��
		);

		return FALSE;	//�ǂݍ��ݎ��s
	}

	//���̑��̐ݒ�
	audio->Volume = volume;
	audio->playType = playType;


	return TRUE;
}

/// <summary>
/// �Q�[���f�[�^�̏�����
/// </summary>
/// <param name=""></param>
VOID GameInit(VOID)
{

}

VOID ChangeScene(GAME_SCENE scene)
{
	GameScene = scene;				//�V�[����؂�ւ�
	IsFadeIn = FALSE;				//�t�F�[�h�C�����Ȃ�
	IsFadeOut = TRUE;				//�t�F�[�h�A�E�g����

	return;
}

/// <summary>
/// �^�C�g�����
/// </summary>
VOID Title(VOID)
{
	TitleProc();  //����
	TitleDraw();  //�`��

	return;
}

/// <summary>
/// �^�C�g����ʂ̏���
/// </summary>
VOID TitleProc(VOID)
{
	//�v���C�V�[���֐؂�ւ���
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{


		//�Q�[���̏�����
		GameInit();

		//�v���C��ʂɐ؂�ւ�
		ChangeScene(GAME_SCENE_PLAY);

		return;
	}


	return;
}

/// <summary>
/// �^�C�g����ʂ̕`��
/// </summary>
VOID TitleDraw(VOID)
{
	DrawString(0, 0, "�^�C�g�����", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// �v���C���
/// </summary>
VOID Play(VOID)
{
	PlayProc();  //����
	PlayDraw();  //�`��

	return;
}

/// <summary>
/// �v���C��ʂ̏���
/// </summary>
VOID PlayProc(VOID)
{
		//�v���C�V�[���֐؂�ւ���
		if (KeyClick(KEY_INPUT_RETURN) == TRUE)
		{
			//�G���h��ʂɐ؂�ւ�
			ChangeScene(GAME_SCENE_END);

			return;
		}
	return;
}

/// <summary>
/// �v���C��ʂ̕`��
/// </summary>
VOID PlayDraw(VOID)
{
	DrawString(0, 0, "�v���C���", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// �G���h���
/// </summary>
VOID End(VOID)
{
	EndProc();  //����
	EndDraw();  //�`��

	return;
}

/// <summary>
/// �G���h��ʂ̏���
/// </summary>
VOID EndProc(VOID)
{
	//�v���C�V�[���֐؂�ւ���
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//�v���C��ʂɐ؂�ւ�
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}


	return;
}

/// <summary>
/// �G���h��ʂ̕`��
/// </summary>
VOID EndDraw(VOID)
{
	DrawString(0, 0, "�G���h���", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// �؂�ւ����
/// </summary>
VOID Change(VOID)
{
	ChangeProc();  //����
	ChangeDraw();  //�`��

	return;
}

/// <summary>
/// �؂�ւ���ʂ̏���
/// </summary>
VOID ChangeProc(VOID)
{
	//�t�F�[�h�C��
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMAX)
		{
			fadeInCnt--;	//�J�E���^�����炷
		}
		else
		{
			//�t�F�[�h�C���������I�����

			fadeInCnt = fadeInCntInit;	//�J�E���^������
			IsFadeIn = FALSE;			//�t�F�[�h�C�������I��
		}

	}

	//�t�F�[�h�A�E�g
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMAX)
		{
			fadeOutCnt++;	//�J�E���^�𑝂₷
		}
		else
		{
			//�t�F�[�h�A�E�g�������I�����

			fadeOutCnt = fadeOutCntInit;	//�J�E���^������
			IsFadeOut = FALSE;				//�t�F�[�h�A�E�g�����I��
		}

	}

	//�؂�ւ������I��?
	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//�t�F�[�h�C�����Ă��Ȃ��A�t�F�[�h�A�E�g�����Ă��Ȃ��Ƃ�
		GameScene = NextGameScene;	//���̃V�[���ɐ؂�ւ�
		OldGameScene = GameScene;	//�ȑO�̃Q�[���V�[���X�V
	}


	return;
}

/// <summary>
/// �؂�ւ���ʂ̕`��
/// </summary>
VOID ChangeDraw(VOID)
{
	//�ȑO�̃V�[����`��
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();	//�^�C�g����ʂ̕`��
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();		//�v���C��ʂ̕`��
		break;
	case GAME_SCENE_END:
		EndDraw();		//�G���h��ʂ̕`��
		break;
	default:
		break;
	}

	//�t�F�[�h�C��
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMAX) * 255);
	}

	//�t�F�[�h�A�E�g
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMAX) * 255);
	}

	//�l�p��`��
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//�������I��
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(0, 16, "�؂�ւ����", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// �����蔻��̗̈�X�V
/// </summary>
/// <param name="coll">�����蔻��̗̈�</param>
VOID CollUpadateGollPlayer(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;					//�����蔻�������
	chara->coll.top = chara->img.y;						//�����蔻�������

	chara->coll.right = chara->img.x + chara->img.width -40;	//�����蔻�������
	chara->coll.bottom = chara->img.y + chara->img.height;	//�����蔻�������

	return;
}

/// <summary>
/// �����蔻��̗̈�X�V
/// </summary>
/// <param name="coll">�����蔻��̗̈�</param>
VOID CollUpadateGoll(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;					//�����蔻�������
	chara->coll.top = chara->img.y;						//�����蔻�������

	chara->coll.right = chara->img.x + chara->img.width - 40;	//�����蔻�������
	chara->coll.bottom = chara->img.y + chara->img.height;	//�����蔻�������

	return;
}

BOOL OnCollision(RECT a, RECT b)
{
	if (
		a.left < b.right &&	//�@�Z�`A�̍���x���W < �Z�`B�̉E��x���W�@����
		a.top < b.bottom &&	//�@�Z�`A�̏��y���W < �Z�`B�̉���y���W�@����
		a.right > b.left &&	//�@�Z�`A�̉E��x���W > �Z�`B�̍���x���W�@����
		a.bottom > b.top	//�@�Z�`A�̉���y���W > �Z�`B�̏��y���W
		)
	{
		//�������Ă���Ƃ�

		return TRUE;
	}
	else
	{
		//�������Ă��Ȃ��Ƃ�
		return FALSE;
	}
}
