#if !defined(__cplusplus) && (__STDC_VERSION__ < 199901L)
	#define inline __forceinline
#endif

#define TRUE (1)
#define FALSE (0)

#define global_variable static
#define internal static

#include <stdint.h>
typedef int32_t b32;
typedef float f32;
typedef double f64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

internal inline s32
win32_abs(s32 value)
{
	return (value < 0) ? (value * -1) : (value);
}

#include <windows.h>

global_variable b32 global_running;
global_variable WINDOWPLACEMENT global_window_placement;

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
		/* monitor related variables */
		s32 monitor_vertical_resolution;
		s32 monitor_horizontal_resolution;

		/* window related variables */
		HWND window;
		HDC window_dc;
		RECT window_dimensions_with_styles = {0};
		DWORD window_style;
		HMENU window_menu;
		#define DEFAULT_WINDOW_WIDTH (1280)
		#define DEFAULT_WINDOW_HEIGHT (720)
		s32 window_horizontal_resolution_with_styles;
		s32 window_vertical_resolution_with_styles;

		/* adjusting window initial dimensions */
		window_style = (WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		window_menu = 0;
		window_dimensions_with_styles.right = DEFAULT_WINDOW_WIDTH;
		window_dimensions_with_styles.bottom = DEFAULT_WINDOW_HEIGHT;
		AdjustWindowRect(&window_dimensions_with_styles, window_style, (window_menu) ? (TRUE) : (FALSE));
		window_horizontal_resolution_with_styles = win32_abs(window_dimensions_with_styles.left) + win32_abs(window_dimensions_with_styles.right);
		window_vertical_resolution_with_styles = win32_abs(window_dimensions_with_styles.top) + win32_abs(window_dimensions_with_styles.bottom);

		window = CreateWindowExA(0, window_class.lpszClassName,
		                         "Main Window",
		                         window_style,
		                         0, 0,
		                         window_horizontal_resolution_with_styles,
		                         window_vertical_resolution_with_styles,
		                         0, 0, instance, 0);

		if(window)
		{
			/* window message loop related variables */
			MSG msg;

			/* adjusting window initial position (centered) on monitor */
			window_dc = GetDC(window);
			monitor_vertical_resolution = GetDeviceCaps(window_dc, VERTRES);
			monitor_horizontal_resolution = GetDeviceCaps(window_dc, HORZRES);

			SetWindowPos(window, 0,
			             (monitor_horizontal_resolution - window_horizontal_resolution_with_styles) / 2,
			             (monitor_vertical_resolution - window_vertical_resolution_with_styles) / 2,
			             0, 0, (SWP_NOSIZE | SWP_NOZORDER));

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
						b32 is_down, was_down, is_alt;

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
