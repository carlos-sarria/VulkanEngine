#include <windows.h>

#include "EngineExample.h"

#ifdef VK_USE_PLATFORM_WIN32_KHR

#define APP_NAME "vkEngine Example"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // EngineExample* engine = reinterpret_cast<EngineExample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (uMsg)
	{
        case WM_CLOSE: PostQuitMessage(0); break;
        case WM_PAINT: return 0;
        case WM_SIZE: return 0;
        case WM_CHAR:
            if ( wParam == VK_ESCAPE ) PostQuitMessage(0);
            break;
        default: break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void createWin32WIndowSurface(SurfaceData& surfaceData)
{
    surfaceData.width = 1280.0f;
    surfaceData.height = 800.0f;

	WNDCLASS winClass;
    surfaceData.connection = GetModuleHandle(NULL);

	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = (WNDPROC)WndProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
    winClass.hInstance = surfaceData.connection;
    winClass.hIcon = LoadIcon(surfaceData.connection, "ICON");
	winClass.hCursor = 0;
	winClass.lpszMenuName = 0;
	winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winClass.lpszClassName = APP_NAME;

	if (!RegisterClass(&winClass))
	{
		exit(1);
	}

    RECT WndRect = { 0, 0, (LONG)surfaceData.width, (LONG)surfaceData.height };
	AdjustWindowRect(&WndRect, WS_OVERLAPPEDWINDOW, FALSE);
    surfaceData.window = CreateWindowEx(0, winClass.lpszClassName, winClass.lpszClassName, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU, 100, 100,
        WndRect.right - WndRect.left, WndRect.bottom - WndRect.top, NULL, NULL, surfaceData.connection, NULL);
    if (!surfaceData.window)
    {
		exit(1);
	}
}

static void destroyWin32WindowSurface(SurfaceData& surfaceData)
{
    DestroyWindow(surfaceData.window);
	PostQuitMessage(0);
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    EngineExample vulkanExample;

    // Initialisation
    createWin32WIndowSurface(vulkanExample.eng.surfaceData);
    vulkanExample.initialize(APP_NAME);

    //SetWindowLongPtrA(vulkanExample.eng.surfaceData.window, GWLP_USERDATA, (LONG_PTR)&vulkanExample);

    // Loop
    MSG msg;
    POINT mousePoint;
    unsigned char keyPressed;
    vulkanExample.eng.appManager.frameId = 0;
    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        if(msg.message == WM_QUIT) break;

        GetCursorPos(&mousePoint);
        keyPressed = 0;

        for (keyPressed=0; keyPressed<128; keyPressed++) if (GetAsyncKeyState(keyPressed) & (1<<15)) break;
        vulkanExample.updateCamera(keyPressed, mousePoint.x, mousePoint.y);

        vulkanExample.drawFrame();

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    // Closing up
    vulkanExample.deinitialize();
    destroyWin32WindowSurface(vulkanExample.eng.surfaceData);

	return 0;
}

#endif
