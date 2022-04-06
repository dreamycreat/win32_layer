
#define TRUE (1)
#define FALSE (0)

#define global_variable static
#define internal static


#include <windows.h>

global_variable int global_running;

internal LRESULT 
win32_window_messages_callback(HWND window,
                               UINT msg,
                               WPARAM wparam,
                               LPARAM lparam)
{
	LRESULT result = 0;

	switch(msg)
	{
		case WM_CLOSE:
		{
			global_running = FALSE;
		} break;

		default:
		{
			result = DefWindowProcA(window, msg, wparam, lparam);
		}
	}

	return result;
}

int WINAPI
WinMain(HINSTANCE instance,
        HINSTANCE prev_instance,
        LPSTR cmd_line,
        int cmd_show)
{
	WNDCLASSEXA window_class = {0};

	window_class.cbSize = sizeof(window_class);
	window_class.style = (CS_VREDRAW | CS_HREDRAW);
	window_class.lpfnWndProc = win32_window_messages_callback;
	window_class.hInstance = instance;
	window_class.hCursor = LoadCursor(instance, IDC_ARROW);
	window_class.lpszClassName = "Main Window Class";

	if(RegisterClassExA(&window_class))
	{
		HWND window;

		window = CreateWindowExA(0, window_class.lpszClassName,
		                         "Main Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		                         0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
		                         0, 0, instance, 0);

		if(window)
		{
			MSG msg;

			global_running = TRUE;
			while(global_running)
			{
				while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		else
		{
			/* TODO: Diagnóstico (falha ao criar a janela principal) */
		}
	}
	else
	{
		/* TODO: Diagnóstico (falha ao registrar a classe da janela principal) */
	}

	return 0;
}
