#include"DirectX.h"
const string APPTITLE = "接苹果";
const int SCREENW = 1024;
const int SCREENH = 768;
int speed=6;
int life = 2;
POINT pos;
enum GAME_STATES
{
	MENU = 0,
	GAME1 = 1,
	GAME2 = 2
};
GAME_STATES game_state = MENU;
DWORD refresh = 0;
DWORD screentime = 0;
double screenfps = 0.0;
double screencount = 0.0;
DWORD coretime = 0;
double corefps = 0.0;
double corecount = 0.0;
DWORD currenttime;
int bcx, bcy,lcx,lcy,pause_state,level;
int gametime = 0;
LPDIRECT3DTEXTURE9 apple_surf = NULL;
LPDIRECT3DTEXTURE9 bucket_surf = NULL;
LPDIRECT3DSURFACE9 background1 = NULL;
LPDIRECT3DSURFACE9 background2 = NULL;
LPDIRECT3DTEXTURE9 textback = NULL;
LPDIRECT3DSURFACE9 sele_surf = NULL;
LPDIRECT3DTEXTURE9 life_surf = NULL;
D3DCOLOR white = D3DCOLOR_XRGB(255, 255, 255);
D3DCOLOR black = D3DCOLOR_XRGB(0, 0, 0);
LPD3DXFONT text = NULL;
LPD3DXFONT bigtext = NULL;
CSound *sound_get = NULL;
struct FALLING
{
	int x, y;
	void reset()
	{
		x = (rand() % (SCREENW - 128));
		y = 0;
	}
};
FALLING apple,flife;
struct BUCKET
{
	int x, y;
};
BUCKET bucket,sele;
int score = 0;
void LoadResource()
{	
	text = MakeFont("等线", 36);
	bigtext = MakeFont("黑体", 72);
	background1 = LoadSurface("tree.jpg");
	background2 = LoadSurface("Background.jpg");
	apple_surf = LoadTexture("Apple.png");
	bucket_surf = LoadTexture("Bucket.png");
	textback = LoadTexture("leaves.png");
	sound_get = LoadSound("get.wav");
	sele_surf = LoadSurface("rect.png");
	life_surf = LoadTexture("Life.png");
}
void UpdatePause(int &pause_state,int &esc_state)
{
	if (key_down(DIK_P) && !pause_state)
		pause_state = 1;
	else if (mouse_button(0))
		pause_state = 0;
	if (key_down(DIK_ESCAPE)&&pause_state)
	{
		esc_state = 1;
		if (esc_state)
		{
			game_state = GAME_STATES::MENU;
			esc_state = 0;
			pause_state = 0;
		}
	}
}
void PauseMenu(int pause_state)
{
	
	if (pause_state)FontPrint(text, 0, 732, "游戏暂停，点击继续，按下ESC返回主菜单。",D3DCOLOR_XRGB(0,0,0));
	else
		FontPrint(text, 0, 732, "方向键移动，按P键暂停游戏。", D3DCOLOR_XRGB(0, 0, 0));
}
bool Game_init(HWND window)
{
	Direct3D_init(window, SCREENW, SCREENH, false);
	Directsound_init(window);
	DirectInput_init(window);
	LoadResource();
	srand((unsigned int)time(NULL));
	bucket.x = 500;
	bucket.y = 640;
	sele.x = 100;
	sele.y = 200;
	pause_state = 0;
	return true;
}
void Game_run(HWND window)
{
	static int return_state = 0, esc_state = 0, up_state = 0, down_state = 0, life_state = 0,time_state=0;
	if (!d3ddev)return;
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);
	currenttime = timeGetTime();//当前时间获取
	corecount ++;//累计帧数
	if (time_state)gametime--;
	if (currenttime >= coretime + 1000)//每过1秒
	{
		corefps = corecount;//传递帧数
		corecount = 0;//清0
		coretime = currenttime;//
	}
	GetCursorPos(&pos);
	ScreenToClient(window, &pos);
	if (currenttime > refresh)
	{
		refresh = currenttime;
		DirectInput_update();
		switch (game_state)
		{
		case GAME_STATES::GAME1:
		{
			UpdatePause(pause_state, esc_state);
			if (!pause_state)
			{
				time_state = 1;
				if(gametime==0)
				{
					MessageBox(0, "时间到，游戏结束", "信息", 0);
					game_state = GAME_STATES::MENU;
				}
				else
				{
					switch (level)
					{
					case 1:
						speed = 5;
						if (score >= 1000)
						{
							level = 2;
							gametime = 4001;
							score = 0;
							MessageBox(0, "恭喜过关。", "提示", 0);
							pause_state = 1;
						}
						break;
					case 2:
						speed = 8;
						if (score >= 1500)
						{
							level = 3;
							gametime = 5001;
							score = 0;
							MessageBox(0, "恭喜过关。", "提示", 0);
							pause_state = 1;
						}
						break;
					case 3:
						speed = 8;
						if (score >= 3000)
						{
							level = 4;
							gametime = 5001;
							score = 0;
							MessageBox(0, "恭喜过关。", "提示", 0);
							pause_state = 1;
						}
						break;
					case 4:
						speed = 10;
						if (score >= 4000)
						{
							level = 5;
							gametime = 6001;
							score = 0;
							MessageBox(0, "恭喜过关。", "提示", 0);
							pause_state = 1;
						}
						break;
					case 5:
						speed = 10;
						if (score >= 6000)
						{
							MessageBox(0, "恭喜通过所有关卡！", "提示", 0);
							game_state = GAME_STATES::MENU;
						}
						break;
					}
					apple.y += speed;
					if (apple.y + 128 > SCREENH)
					{
						apple.reset();
					}
					if (key_down(DIK_LEFT) && bucket.x >= 0)bucket.x -= 10;
					else if (key_down(DIK_RIGHT) && bucket.x <= SCREENW - 283)bucket.x += 10;
					bcx = apple.x + 64;
					bcy = apple.y + 64;
					if (bcx > bucket.x+32 && bcx<bucket.x + 251 && bcy>bucket.y+50 && bcy < bucket.y + 211)
					{
						score += 100;
						apple.reset();
						Playsound(sound_get);
					}
				}
			}
			else 
				time_state = 0;
			break;
		}
		case GAME_STATES::GAME2:
			UpdatePause(pause_state,esc_state);
			if(!pause_state)
			{
				if (life_state)flife.y += speed * 2;
				apple.y += speed;
				if (apple.y + 128 > SCREENH)
				{
					if (life == 0)
					{
						MessageBox(0, "游戏结束", "信息", 0);
						game_state = GAME_STATES::MENU;
					}
					else
					{
						life--;
						apple.reset();
						if (rand() % 100 <= 20 && !life_state)
						{
							life_state = 1;
							flife.reset();
						}
					}
				}
				if (flife.y + 128 > SCREENH)
				{
					life_state = 0;
					flife.y = 0;
				}
				if (key_down(DIK_LEFT) && bucket.x >= 0)bucket.x -= 10;
				else if (key_down(DIK_RIGHT) && bucket.x <= SCREENW - 283)bucket.x += 10;
				bcx = apple.x + 64;
				bcy = apple.y + 64;
				lcx = flife.x + 64;
				lcy = flife.y + 64;
				if (bcx > bucket.x+32 && bcx<bucket.x + 251 && bcy>bucket.y+50 && bcy < bucket.y + 211)
				{
					score++;
					if (score % 10 == 0 && score != 0&&speed!=10)speed++;
					apple.reset();
					if (rand() % 100 <= 20 && !life_state)
					{
						life_state = 1;
						flife.reset();
					}
					Playsound(sound_get);
				}
				if (lcx > bucket.x+32 && lcx<bucket.x + 251 && lcy>bucket.y+50 && lcy < bucket.y + 211)
				{
					life++;
					life_state = 0;
					flife.y = 0;
				}
			}
			break;
		case GAME_STATES::MENU:
			speed = 2;
			life = 2;
			score = 0;
			apple.reset();
			flife.reset();
			time_state = 0;
			gametime = 3001;
			level = 1;
			if (pos.y >= 136 && pos.y <= 336&&mouse_x()!=0)sele.y = 200;
			if (pos.y >= 336 && pos.y <= 536&&mouse_x() != 0)sele.y = 400;
			if (pos.y >= 536 && mouse_x() != 0)sele.y = 600;
			if (key_down(DIK_RETURN)|| (mouse_button(0) && (pos.x >= 100 && pos.x <= 228)))
			{
				return_state = 1;
			}
			else
			{
				if (return_state == 1)
				{
					if (sele.y == 200)
					{
						game_state = GAME_STATES::GAME1;
					}
					else if (sele.y == 400)
					{
						game_state = GAME_STATES::GAME2;
					}
					else if (sele.y == 600)
					{
						gameover = true;
					}
					return_state = 0;
				}
			}
			if (key_down(DIK_UP))
			{
				up_state = 1;
			}
			else
			{
				if (up_state == 1&&sele.y!=200)
				{
					sele.y = sele.y - 200;
					up_state = 0;
				}
			}
			if (key_down(DIK_DOWN))
			{
				down_state = 1;
			}
			else
			{
				if (down_state == 1&&sele.y!=600)
				{
					sele.y = sele.y + 200;
					down_state = 0;
				}
			}
			break;
		}
		screencount += 1.0;
		if (currenttime >= screentime + 1000)
		{
			screenfps = screencount;
			screencount = 0.0;
			screentime = currenttime;
		}
	}
	
	d3ddev->ColorFill(backbuffer, NULL, D3DCOLOR_XRGB(255, 255,255));
	if (d3ddev->BeginScene())
	{
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		switch (game_state)
		{
		case GAME_STATES::GAME1:
			DrawSurface(backbuffer, 0, 0, background1);
			Sprite_Transform_Draw(apple_surf, apple.x, apple.y, 128, 128);
			Sprite_Transform_Draw(bucket_surf, bucket.x, bucket.y, 566, 299,0,1,0,0.5);
			Sprite_Transform_Draw(textback, 0, 0, 1024, 768);
			FontPrint(text, 0, 0, "分数: " + to_string(score));
			FontPrint(text, 300, 0, "时间："+to_string(gametime/100));
			switch (level)
			{
			case 1:
				FontPrint(text, 600, 0, "关卡：" + to_string(level) + " 目标得分：1000");
				break;
			case 2:
				FontPrint(text, 600, 0, "关卡：" + to_string(level) + " 目标得分：1500");
				break;
			case 3:
				FontPrint(text, 600, 0, "关卡：" + to_string(level) + " 目标得分：3000");
				break;
			case 4:
				FontPrint(text, 600, 0, "关卡：" + to_string(level) + " 目标得分：4000");
				break;
			case 5:
				FontPrint(text, 600, 0, "关卡：" + to_string(level) + " 目标得分：6000");
				break;
			}
			PauseMenu(pause_state);
			break;
		case GAME_STATES::GAME2:
			DrawSurface(backbuffer, 0, 0, background1);
			Sprite_Transform_Draw(apple_surf, apple.x, apple.y, 128, 128);
			if (life_state)Sprite_Transform_Draw(life_surf, flife.x, flife.y, 128, 128);
			Sprite_Transform_Draw(bucket_surf, bucket.x, bucket.y, 566, 299,0,1,0,0.5);
			Sprite_Transform_Draw(textback, 0, 0, 1024, 768);
			FontPrint(text, 0, 0, "分数: "+to_string(score));
			FontPrint(text, 900, 0, "生命: "+to_string(life));
			PauseMenu(pause_state);
			break;
		case GAME_STATES::MENU:
			DrawSurface(backbuffer, 0, 0, background2);
			Sprite_Transform_Draw(apple_surf, sele.x-32, sele.y-48, 128, 128);
			Sprite_Transform_Draw(textback, 0, 0, 1024, 768);
			FontPrint(bigtext, 404, 50, "接苹果", white);
			FontPrint(text, 100, 200, "经典", white);
			FontPrint(text, 100, 400, "无限", white);
			FontPrint(text, 100, 600, "退出", white);
			FontPrint(text, 0, 732, "上海大学 曾诗洋 制作", white);
			break;
		}
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}
}
void Game_end()
{
	if (apple_surf)apple_surf->Release();
	if (bucket_surf)bucket_surf->Release();
	if (background1)background1->Release();
	if (life_surf)life_surf->Release();
	if (textback)textback->Release();
	if (sele_surf)sele_surf->Release();
	if (sound_get) delete sound_get;
	DirectInput_shutdown();
	Direct3D_shutdown();
}