#include <windows.h>
#include <d2d1.h>
#pragma comment(lib,"d2d1.lib")

#include <ctime>
#include <InitGuid.h>

//#include <dinput8.h>
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dinput8.lib")


#include <dinput.h>
#include <dsound.h>
#include <dwrite.h>

#include <iostream>
#include <fstream>
#include <string>

#include "Box.h"

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
		WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720,
		NULL, NULL, hInstance, NULL);
	return hWnd;
}



int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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
			hWnd, D2D1::SizeU(1280, 720))
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

	while (flag)
	{
		rt->BeginDraw();
		rt->Clear(D2D1::ColorF(D2D1::ColorF::White));

		rt->DrawRectangle(
			D2D1::RectF(0, 120, 1280, 360),
			brushes[17], 240.0f, NULL);

		rt->DrawRectangle(
			D2D1::RectF(0, 540, 1280, 660),
			brushes[7], 120.0f, NULL);

		rt->EndDraw();
	}


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
		72.0f,
		L"en-us",
		&pTextFormat
	);
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
		rt->DrawTextA(L"CONGRATULATIONS", 16, pTextFormat, rectf, brushes[26]);
		rt->DrawTextA(L"CONGRATULATIONS", 16, pTextFormat, rectf2[counter % 31], brushes[27]);

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