#include "Box.h"

box box0;

//using namespace std;
struct WaveHeaderType
{
	char chunkId[4];
	unsigned long chunkSize;
	char format[4];
	char subChunkId[4];
	unsigned long subChunkSize;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long bytesPerSecond;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	char dataChunkId[4];
	unsigned long dataSize;
};


LRESULT _stdcall WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam);

HWND winInit(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(6);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "class";

	RegisterClass(&wc);
	HWND hWnd = CreateWindow(
		"class",
		"Okami",
		WS_OVERLAPPEDWINDOW, 100, 100, box0.width, box0.hight,
		NULL, NULL, hInstance, NULL);
	return hWnd;
}



int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//размеры окна
	box0.width = 1280;
	box0.hight = 720;

	HWND hWnd = winInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	IDirectInput8* pDirInp = NULL;
	if (DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&pDirInp, NULL) != DI_OK)
	{
		return -1;
	}

	IDirectInputDevice8* didev;


	if (pDirInp->CreateDevice(GUID_SysKeyboard, &didev, NULL) != DI_OK)
	{
		pDirInp->Release();
		return -1;
	}

	ID2D1Factory* factory = NULL;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);

	ID2D1HwndRenderTarget* rt = NULL;
	factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			hWnd, D2D1::SizeU(box0.width, box0.hight))
		, &rt);

	ID2D1SolidColorBrush ** brushes = new ID2D1SolidColorBrush*[32];

	for (int i = 0; i < 27; i++)
	{
		rt->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF((float)((i / 9) % 3) / 3.5, (float)((i / 3) % 3) / 3.5, (float)(i % 3) / 3.5)),
			&brushes[i]);
	}
	rt->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF(0.8f, 0.8f, 0.8f)),
		&brushes[27]);


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	MSG msg;


	

	int dy = 0;
	bool flag = true;

	level Level = level("materials/w1l1.txt");

	didev->SetDataFormat(&c_dfDIKeyboard);
	didev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	didev->Acquire();
	char buffer[256];

	IDWriteFactory* pDWriteFactory = NULL;
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory)
	);
	IDWriteTextFormat * pTextFormat = NULL;
	HRESULT hr = pDWriteFactory->CreateTextFormat(
		L"Gabriola",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		96.0f,
		L"en-us",
		&pTextFormat
	);



	int VisMode = 0;
	int act = 0;
	int CDvis = 0;
	while (flag)
	{
		act = 0;
		rt->BeginDraw();
		rt->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		HRESULT hr = didev->GetDeviceState(sizeof(buffer), buffer);
		if FAILED(hr)
			return 0;
		if (Level.player.actionCooldown <= 0)
		{
			if ((buffer[DIK_RIGHT] & 0x80) || (buffer[DIK_D] & 0x80))
			{
				if (Level.player.rotation.y == 0)
				{
					Level.player.rotation.y += Level.player.rotation.x;
					Level.player.rotation.x = 0;
				}
				else
				{
					Level.player.rotation.x -= Level.player.rotation.y;
					Level.player.rotation.y = 0;
				}
			}

			else if ((buffer[DIK_LEFT] & 0x80) || (buffer[DIK_A] & 0x80))
			{
				if (Level.player.rotation.y == 0)
				{
					Level.player.rotation.y -= Level.player.rotation.x;
					Level.player.rotation.x = 0;
				}
				else
				{
					Level.player.rotation.x += Level.player.rotation.y;
					Level.player.rotation.y = 0;
				}
			}

			else if ((buffer[DIK_UP] & 0x80) || (buffer[DIK_W] & 0x80))
			{
				Level.player.position = Level.player.position + Level.player.rotation;
				if (Level.map[Level.player.position.y][Level.player.position.x] == 1 || Level.map[Level.player.position.y][Level.player.position.x] == 3 || Level.map[Level.player.position.y][Level.player.position.x] == 4)
				{
					Level.player.position = Level.player.position - Level.player.rotation;
				}
				Level.player.last = 1;
			}

			else if ((buffer[DIK_DOWN] & 0x80) || (buffer[DIK_S] & 0x80))
			{
				Level.player.position = Level.player.position - Level.player.rotation;
				if (Level.map[Level.player.position.y][Level.player.position.x] == 1 || Level.map[Level.player.position.y][Level.player.position.x] == 3 || Level.map[Level.player.position.y][Level.player.position.x] == 4)
				{
					Level.player.position = Level.player.position + Level.player.rotation;
				}
				Level.player.last = -1;
			}

			if (((buffer[DIK_LEFT] & 0x80) || (buffer[DIK_A] & 0x80)) || ((buffer[DIK_RIGHT] & 0x80) || (buffer[DIK_D] & 0x80)))
			{
				Level.player.actionCooldown = Level.player.RotationCD;
			}

			if (((buffer[DIK_UP] & 0x80) || (buffer[DIK_W] & 0x80)) || ((buffer[DIK_DOWN] & 0x80) || (buffer[DIK_S] & 0x80)))
			{
				Level.player.actionCooldown = Level.player.MoveCD;
			}

			
			if (buffer[DIK_LCONTROL] && Level.player.actionCooldown <= 0)//
			{
				Level.playerFire();
				Level.player.actionCooldown = Level.player.FireCD;
				act = 1;
			}
			

			if (buffer[DIK_R] && Level.player.actionCooldown <= 0)//
			{
				if (Level.player.inGun < Level.player.GunCapasity && Level.player.ammo > 0)
				{
					if (Level.player.ammo >= Level.player.GunCapasity - Level.player.inGun)
					{
						Level.player.ammo -= Level.player.GunCapasity - Level.player.inGun;
						Level.player.inGun = Level.player.GunCapasity;
					}
					else
					{
						Level.player.ammo = 0;
						Level.player.inGun += Level.player.ammo;
					}
					Level.player.actionCooldown = Level.player.ReloadCD;
					act = 2;
				}
			}


		}

		if (((buffer[DIK_TAB] & 0x80) || (buffer[DIK_M] & 0x80)) && CDvis <= 0)
		{
			VisMode = (VisMode + 1) % 2;
			CDvis = 16;
		}
		CDvis--;
		if (CDvis < 0) CDvis = 0;




		if (Level.enemy1count == 0 && Level.enemy2count == 0 && Level.map[Level.player.position.y][Level.player.position.x] == 5)
		{
			flag = false;
		}
		else
		{
			if (Level.player.health <= 0)
			{
				flag = false;
			}
		}
		Level.getTurn();
		// acts: 0 - ничего не произошло, или произошло движение -> просто визуализировать.
		//       1 - произошла атака игрока -> анимация удара, если противник рядом, выстрел иначе.
		//       2 - перезарядка -> анимация перезарядки.
		// mod: 0 - 3D
		//      1 - режим карты с радаром живых врагов.
		Level.boxVIS(rt, VisMode, act, box0, brushes);

		Level.HUD(rt, box0, brushes, pTextFormat);

		Level.player.actionCooldown -= 1;
		if (Level.player.actionCooldown < 0)
		{
			Level.player.actionCooldown = -1;
		}
		rt->EndDraw();
	}


	
	D2D1_RECT_F rectf = D2D1::RectF(
		box0.width / 2 - 480.0f,
		box0.hight / 2 - 25.0f,
		box0.width / 2 + 480.0f,
		box0.hight / 2 + 25.0f
	);
	D2D1_RECT_F* rectf2 = new D2D1_RECT_F[31];
	for (int i = 0; i < 15; i++)
	{
		rectf2[i] = D2D1::RectF(
			box0.width / 2 - 480.0f + 13.0f * ((15.0f - i) / 15),
			box0.hight / 2 - 25.0f - 13.0f * ((15.0f - i) / 15),
			box0.width / 2 + 480.0f + 13.0f * ((15.0f - i) / 15),
			box0.hight / 2 + 25.0f - 13.0f * ((15.0f - i) / 15));
	}

	rectf2[15] = D2D1::RectF(
		box0.width / 2 - 480.0f,
		box0.hight / 2 - 25.0f,
		box0.width / 2 + 480.0f,
		box0.hight / 2 + 25.0f
	);

	for (int i = 16; i < 31; i++)
	{
		rectf2[i] = D2D1::RectF(
			box0.width / 2 - 480.0f + 13.0f * ((i - 15.0f) / 15),
			box0.hight / 2 - 25.0f - 13.0f * ((i - 15.0f) / 15),
			box0.width / 2 + 480.0f + 13.0f * ((i - 15.0f) / 15),
			box0.hight / 2 + 25.0f - 13.0f * ((i - 15.0f) / 15));
	}
	Sleep(500);
	int counter = 0;
	while (1)
	{
		counter += 3;
		rt->BeginDraw();

		rt->Clear();
		rt->DrawRectangle(
			D2D1::RectF(box0.width / 4, box0.hight / 4, 3 * box0.width / 4, 3 * box0.hight / 4),
			brushes[0], min(box0.width, box0.hight), NULL);
		if (Level.player.health > 0)
		{
			rt->DrawTextA(L"CONGRATULATIONS", 16, pTextFormat, rectf, brushes[26]);
			rt->DrawTextA(L"CONGRATULATIONS", 16, pTextFormat, rectf2[counter % 31], brushes[27]);
		}
		else
		{
			rt->DrawTextA(L"Game Over", 11, pTextFormat, rectf, brushes[26]);
			rt->DrawTextA(L"Game Over", 11, pTextFormat, rectf2[counter % 31], brushes[27]);
		}

		rt->EndDraw();
		

	}



	for (int i = 0; i < 28; i++)
	{
		if (brushes[i] != NULL)
			brushes[i]->Release();
	}

	if (rt != NULL)
		rt->Release();
	if (factory != NULL)
		factory->Release();
	Sleep(500);



	return 0;
}

LRESULT _stdcall WndProc(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}