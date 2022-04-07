#if !defined(__cplusplus) && (__STDC_VERSION__ < 199901L)
	#define inline __forceinline
#endif

#if defined(DEBUG)
	#define assert(expression) if(!(expression)) { *(int *)0 = 0; }
#else
	#define assert(expression)
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

typedef struct
{
	s32 width;
	s32 height;
	s32 bytes_per_pixel;
	s32 line_stride;
	HDC bitmap_dc;
	HBITMAP bitmap_handle;
	void *pixels;
} win32_backbuffer;



global_variable b32 global_running;
global_variable WINDOWPLACEMENT global_window_placement;
global_variable win32_backbuffer global_backbuffer;



internal void
win32_resize_backbuffer(s32 new_width,
                        s32 new_height)
{
	BITMAPINFO bitmap_info;

	if(!global_backbuffer.bitmap_dc)
	{
		global_backbuffer.bitmap_dc = CreateCompatibleDC(0);
	}

	global_backbuffer.width = new_width;
	global_backbuffer.height = new_height;
	global_backbuffer.bytes_per_pixel = 4; /* ARGB */
	global_backbuffer.line_stride = global_backbuffer.width * global_backbuffer.bytes_per_pixel;

	bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
	bitmap_info.bmiHeader.biWidth = new_width;

	bitmap_info.bmiHeader.biHeight = new_height;
	/*
	 * NOTE: "biHeight" with a positive number are bottom-top drawing. For a top-bottom
	 * drawing, use the NEGATIVE value of the height.
	 */

	bitmap_info.bmiHeader.biPlanes = 1;
	bitmap_info.bmiHeader.biBitCount = 32;
	bitmap_info.bmiHeader.biCompression = BI_RGB;
	
	global_backbuffer.bitmap_handle = CreateDIBSection(global_backbuffer.bitmap_dc,
	                                                   &bitmap_info,
	                                                   DIB_RGB_COLORS,
	                                                   &global_backbuffer.pixels,
	                                                   0, 0);

	assert(global_backbuffer.bitmap_handle);
}

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

		case WM_DESTROY:
		{
			/* TODO: Diagnostic (main window destroyed) */
			/* TODO: Check if it was intended */

			PostQuitMessage(0);
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

			/* removing resizable window capabilities */
			SetWindowLongA(window, GWL_STYLE, (GetWindowLong(window, GWL_STYLE) & ~WS_SIZEBOX) & ~WS_MAXIMIZEBOX);

			/* adjusting window initial position (centered) on monitor */
			window_dc = GetDC(window);
			monitor_vertical_resolution = GetDeviceCaps(window_dc, VERTRES);
			monitor_horizontal_resolution = GetDeviceCaps(window_dc, HORZRES);

			SetWindowPos(window, 0,
			             (monitor_horizontal_resolution - window_horizontal_resolution_with_styles) / 2,
			             (monitor_vertical_resolution - window_vertical_resolution_with_styles) / 2,
			             0, 0, (SWP_NOSIZE | SWP_NOZORDER));

			/* backbuffer setup */
			win32_resize_backbuffer(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

			global_running = TRUE;
			while(global_running)
			{
				/*
				 * Input
				*/

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
							} break;

							case MK_MBUTTON:
							{
							} break;

							case MK_RBUTTON:
							{
							} break;
						}
					}

					/* (key released) mouse button messages */
					if(msg.message == WM_LBUTTONUP)
					{
					}

					if(msg.message == WM_MBUTTONUP)
					{
					}

					if(msg.message == WM_RBUTTONUP)
					{
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
								case VK_RETURN:
								{
									/*
									 * NOTE: Toggle fullscreen
									*/
									if(is_alt && is_down)
									{
										DWORD window_style;
										RECT client_rect = {0};

										window_style = GetWindowLong(window, GWL_STYLE);

										if(window_style & WS_OVERLAPPEDWINDOW)
										{
											MONITORINFO monitor_info = {0};
											monitor_info.cbSize = sizeof(MONITORINFO);

											if(GetWindowPlacement(window, &global_window_placement) &&
											   GetMonitorInfoA(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
											{
												SetWindowLong(window, GWL_STYLE, window_style & ~WS_OVERLAPPEDWINDOW);
												SetWindowPos(window, HWND_TOP,
												             monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
												             monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
												             monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
												             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
											}
										}
										else
										{
											SetWindowLong(window, GWL_STYLE, window_style | ((WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX) & ~WS_MAXIMIZEBOX));
											SetWindowPlacement(window, &global_window_placement);
											SetWindowPos(window, 0, 0, 0, 0, 0,
											             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
											             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
										}

										assert(GetClientRect(window, &client_rect));
										win32_resize_backbuffer(client_rect.right, client_rect.bottom);
									}
								} break;

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

				/*
				 * Update & Draw
				*/

				/* IMPORTANT: Test Code! Clear client area with a HEX color */
				{
					s32 x, y;
					u32 *pixel;
					u32 pixel_color;

					pixel = (u32 *)global_backbuffer.pixels;
					pixel_color = 0xff14233a; /* A R G B */

					for(y = global_backbuffer.height - 1;
					    y >= 0;
					    --y)
					{
						for(x = 0;
						    x < global_backbuffer.width;
						    ++x)
						{
							*pixel++ = pixel_color;
						}
					}
				}

				SelectObject(global_backbuffer.bitmap_dc, global_backbuffer.bitmap_handle);
				BitBlt(window_dc,
				       0, 0,
				       global_backbuffer.width, global_backbuffer.height,
				       global_backbuffer.bitmap_dc,
				       0, 0, SRCCOPY);
			}
		}
		else
		{
			/* TODO: Diagnostic (failed when creating the main window) */
		}
	}
	else
	{
		/* TODO: Diagnostic (failed when registering the main window class) */
	}

	return 0;
}
