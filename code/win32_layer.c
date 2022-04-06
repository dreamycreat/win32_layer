
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
		/* window related variables */
		HWND window;

		window = CreateWindowExA(0, window_class.lpszClassName,
		                         "Main Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		                         0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
		                         0, 0, instance, 0);

		if(window)
		{
			/* window related variables */
			MSG msg;

			global_running = TRUE;
			while(global_running)
			{
				while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
				{
					/*
					 * Mouse input
					*/

					/* (key pressed) mouse button messages */
					if(msg.message == WM_LBUTTONDOWN ||
					   msg.message == WM_MBUTTONDOWN ||
					   msg.message == WM_RBUTTONDOWN)
					{
						UINT mouse_button;

						mouse_button = (UINT) msg.wParam;

						switch(mouse_button)
						{
							case MK_LBUTTON:
							{
								OutputDebugStringA("MB Left (is down)!!!\n");
							} break;

							case MK_MBUTTON:
							{
								OutputDebugStringA("MB Middle (is down)!!!\n");
							} break;

							case MK_RBUTTON:
							{
								OutputDebugStringA("MB Right (is down)!!!\n");
							} break;
						}
					}

					/* (key released) mouse button messages */
					if(msg.message == WM_LBUTTONUP)
					{
						OutputDebugStringA("MB Left (released)!\n");
					}

					if(msg.message == WM_MBUTTONUP)
					{
						OutputDebugStringA("MB Left (released)!\n");
					}

					if(msg.message == WM_RBUTTONUP)
					{
						OutputDebugStringA("MB Left (released)!\n");
					}

					/*
					 * Keyboard input
					*/

					if(msg.message == WM_KEYDOWN ||
					   msg.message == WM_KEYUP ||
					   msg.message == WM_SYSKEYDOWN ||
					   msg.message == WM_SYSKEYUP)
					{
						UINT key;
						int is_down, was_down, is_alt;

						key = (UINT) msg.wParam;
						is_alt = (msg.lParam & (1 << 29)) != 0;
						is_down = (msg.lParam & (1 << 31)) == 0;
						was_down = (msg.lParam & (1 << 30)) != 0;

						if(was_down != is_down)
						{
							switch(key)
							{
								case VK_F4:
								{
									if(is_alt && is_down)
									{
										global_running = FALSE;
									}
								} break;
							}
						}
					}
					else
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
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
