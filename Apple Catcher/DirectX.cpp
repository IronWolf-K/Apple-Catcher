#include"DirectX.h"
#include"DirectSound.h"
#include<iostream>
using namespace std;
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
LPD3DXSPRITE spriteobj;
CSoundManager *dsound = NULL;
char keys[256];
int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;
bool Direct3D_init(HWND window, int width, int height, bool fullscreen)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)return false;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = (!fullscreen);
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.hDeviceWindow = window;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (!d3ddev)return false;
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer); 
	D3DXCreateSprite(d3ddev, &spriteobj);
	return true;
}
void Direct3D_shutdown()
{
	if (spriteobj)spriteobj->Release();
	if (d3ddev)d3ddev->Release();
	if (d3d)d3d->Release();
}
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
	D3DSURFACE_DESC desc;
	source->GetDesc(&desc);
	RECT source_rect = { 0,0,(long)desc.Width,(long)desc.Height };
	RECT dest_rect = { (long)x,(long)y,(long)x + desc.Width,(long)y + desc.Height };
	d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);
}
LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
	LPDIRECT3DSURFACE9 image = NULL;
	_D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK)return NULL;
	result = d3ddev->CreateOffscreenPlainSurface(info.Width, info.Height, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &image, NULL);
	if (result != D3D_OK)return NULL;
	result = D3DXLoadSurfaceFromFile(image, NULL, NULL, filename.c_str(), NULL, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 0, 0), NULL);
	if (result != D3D_OK)return NULL;
	return image;
}
LPDIRECT3DTEXTURE9 LoadTexture(std::string filename, D3DCOLOR transcolor)
{
	LPDIRECT3DTEXTURE9 texture = NULL;
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK)return NULL;
	D3DXCreateTextureFromFileEx(d3ddev, filename.c_str(), info.Width, info.Height, 1, D3DPOOL_DEFAULT, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, transcolor, &info, NULL, &texture);
	if (result != D3D_OK)return NULL;
	return texture;
}
D3DXVECTOR2 GetBitmapSize(string filename)
{
	D3DXIMAGE_INFO info;
	D3DXVECTOR2 size = D3DXVECTOR2(0.0f, 0.0f);
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result == D3D_OK)
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);
	else
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);
	return size;
}
void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framenum, int framew, int frameh, int columns)
{
	D3DXVECTOR3 position((float)destx, (float)desty, 0);
	D3DCOLOR white = D3DCOLOR_XRGB(255, 255, 255);
	RECT rect;
	rect.left = (framenum%columns)*framew;
	rect.top = (framenum / columns)*frameh;
	rect.right = rect.left + framew;
	rect.bottom = rect.top + frameh;
	spriteobj->Draw(texture, &rect, NULL, &position, white);
}
void Sprite_Animate(int&frame, int startframe, int endframe, int direction, int&starttime, int delay)
{
	if ((int)GetTickCount() > starttime + delay)
	{
		starttime = GetTickCount();
		frame += direction;
		if (frame > endframe)frame = startframe;
		if (frame < startframe)frame = endframe;
	}
}
void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, int frame, int columns, float rotation, float scaling, D3DCOLOR color)
{
	D3DXVECTOR2 scale(scaling, scaling);
	D3DXVECTOR2 trans(x, y);
	D3DXVECTOR2 center((float)(width * scaling) / 2, (float)(height * scaling) / 2);
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);
	spriteobj->SetTransform(&mat);
	int fx = (frame % columns)*width;
	int fy = (frame / columns)*height;
	RECT srcrect = { fx, fy, fx + width, fy + height };
	spriteobj->Draw(image, &srcrect, NULL, NULL, color);
}
LPD3DXFONT MakeFont(string name, int size)
{
	LPD3DXFONT font = NULL;
	D3DXFONT_DESC desc = { size,0,0,0,false,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_PITCH,"" };
	strcpy(desc.FaceName, name.c_str());
	D3DXCreateFontIndirect(d3ddev, &desc, &font);
	return font;
}
void FontPrint(LPD3DXFONT font, int x, int y, string text, D3DCOLOR color)
{
	RECT rect = { x,y,0,0 };
	font->DrawTextA(NULL, text.c_str(), text.length(), &rect, DT_CALCRECT, color);
	font->DrawTextA(spriteobj, text.c_str(), text.length(), &rect, DT_LEFT, color);
}
bool Directsound_init(HWND hwnd)
{
	dsound = new CSoundManager();
	HRESULT result;
	result = dsound->Initialize(hwnd, DSSCL_PRIORITY);
	if (result != DS_OK)return false;
	result = dsound->SetPrimaryBufferFormat(2, 22050, 16);
	if (result != DS_OK)return false;
	return true;
}
void Directsound_shutdown()
{
	if (dsound)delete dsound;
}
CSound *LoadSound(string fliename)
{
	HRESULT result;
	CSound *wave = NULL;
	char s[255];
	sprintf(s, "%s", fliename.c_str());
	result = dsound->Create(&wave, s);
	if (result != DS_OK)wave = NULL;
	return wave;
}
void Playsound(CSound *sound)
{
	sound->Play();
}
void Loopsound(CSound *sound)
{
	sound->Play(0, DSBPLAY_LOOPING);
}
void Stopsound(CSound *sound)
{
	sound->Stop();
}
bool DirectInput_init(HWND hwnd)
{
	HRESULT result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, NULL);
	dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
	dikeyboard->SetDataFormat(&c_dfDIKeyboard);
	dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dikeyboard->Acquire();
	dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dimouse->Acquire();
	d3ddev->ShowCursor(false);
	return true;
}
void DirectInput_update()
{
	dimouse->Poll();
	if(!SUCCEEDED(dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state)))
		dimouse->Acquire();
	dikeyboard->Poll();
	if (!SUCCEEDED(dikeyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys)))
		dikeyboard->Acquire();

}
int mouse_x()
{
	return mouse_state.lX;
}
int mouse_y()
{
	return mouse_state.lY;
}
int mouse_button(int button)
{
	return mouse_state.rgbButtons[button] & 0x80;
}
int key_down(int key)
{
	return(keys[key] & 0x80);
}
void DirectInput_shutdown()
{
	if (dikeyboard)
	{
		dikeyboard->Unacquire();
		dikeyboard->Release();
		dikeyboard = NULL;
	}
	if (dimouse)
	{
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = NULL;
	}
}