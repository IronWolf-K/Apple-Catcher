#pragma once
#define WIN32_EXTRA_LEAN
#define DIRECTINPUT_VERSION 0x0800
#include<windows.h>
#include<d3d9.h>
#include<d3dx9.h>
#include<dinput.h>
#include<ctime>
#include<iostream>
#include<iomanip>
#include<sstream>
#include"DirectSound.h"
#include"resource.h"
using namespace std;
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxerr.lib")
extern const string APPTITLE;
extern const int SCREENW;
extern const int SCREENH;
extern bool gameover;
extern LPDIRECT3D9 d3d;
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DSURFACE9 backbuffer;
extern LPD3DXSPRITE spriteobj;
extern CSoundManager *dsound;
struct SPRITE
{
	float x, y;
	int frame, columns;
	int width, height;
	float rotation, scaling;
	int startframe, endframe;
	int starttime, delay;
	int direction;
	float velx, vely;
	D3DCOLOR color;
	SPRITE()
	{
		frame = 0;
		columns = 1;
		width = height = 0;
		scaling = 1.0f;
		rotation = 0.0f;
		startframe = endframe = 0;
		direction = 1;
		starttime = delay = 0;
		velx = vely = 0.0f;
		color = D3DCOLOR_XRGB(255, 255, 255);
	}
};
bool Direct3D_init(HWND hwnd, int width, int height, bool fullscreen);
void Direct3D_shutdown();
LPDIRECT3DSURFACE9 LoadSurface(string filename);
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source);
D3DXVECTOR2 GetBitmapSize(string filename);
LPDIRECT3DTEXTURE9 LoadTexture(string filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0));
void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framenum, int framew, int frameh, int columns);
void Sprite_Animate(int &frame, int startframe, int endframe, int direction, int &starttime, int delay);
void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, int frame = 0, int colunms = 1, float rotation = 0.0f, float scaling = 1.0f, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
LPD3DXFONT MakeFont(string name, int size);
void FontPrint(LPD3DXFONT font, int x, int y, string text, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
bool Directsound_init(HWND hwnd);
void Directsound_shutdown();
CSound *LoadSound(string filename);
void Playsound(CSound *sound);
void Loopsound(CSound *sound);
void Stopsound(CSound *sound);
extern LPDIRECTINPUT8 dinput;
extern LPDIRECTINPUTDEVICE8 dimouse;
extern LPDIRECTINPUTDEVICE8 dikeyboard;
extern DIMOUSESTATE mouse_state;
bool DirectInput_init(HWND);
void DirectInput_update();
void DirectInput_shutdown();
int key_down(int);
int mouse_button(int);
int mouse_x();
int mouse_y();
bool Game_init(HWND window);
void Game_run(HWND window);
void Game_end();