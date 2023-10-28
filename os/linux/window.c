#include "os.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#pragma warning(push, 0)
#pragma warning(pop)
#include <stdio.h>

#define MAX_DROP_FILES 10
struct window
{
   GLFWwindow *handle;
   s32         number_of_drop_files;
   char        drop_file[MAX_DROP_FILES][1024];
   b32         resized;
};

int _mouse_button_left   = 0;
int _mouse_button_right  = 1;
int _mouse_button_middle = 2;

struct mouse    mouse    = { 0 };
struct keyboard keyboard = { 0 };
struct window   window   = { 0 };

static uint32 win_Win32_KeyMapping[256]          = { 0 };
static char   win_Char_KeyMapping_LowerCase[256] = { 0 };
static char   win_Char_KeyMapping_UpperCase[256] = { 0 };

/*
static WPARAM _MapLeftRightKeys(WPARAM vk, LPARAM lParam)
{
    WPARAM new_vk = vk;
    UINT scancode = ((uint32)lParam & 0x00ff0000) >> 16;
    b32 extended  = (lParam & 0x01000000) != 0;

    switch (vk) {
    case VK_SHIFT:
    new_vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
    break;
    case VK_CONTROL:
    new_vk = extended ? (WPARAM)VK_RCONTROL : (WPARAM)VK_LCONTROL;
    break;
    case VK_MENU:
    new_vk = extended ? (WPARAM)VK_RMENU : (WPARAM)VK_LMENU;
    break;
    default:
    // not a key we map from generic to left/right specialized
    //  just return it.
    new_vk = vk;
    break;
    }

    return(new_vk);
}

uint32
_get_key_mapping(uint32 key)
{
    if (key > 256)
        return(keycode_F12);
    else
        return(win_Win32_KeyMapping[key]);
}
*/

inline b32
left_mouse_button_pressed()
{
   return (mouse.button[_mouse_button_left] && !mouse.button_last_frame[_mouse_button_left]);
}

inline b32
left_mouse_button_held()
{
   return (mouse.button[_mouse_button_left]);
}

inline b32
left_mouse_button_released()
{
   return (!mouse.button[_mouse_button_left] && mouse.button_last_frame[_mouse_button_left]);
}

inline b32
right_mouse_button_pressed()
{
   return (mouse.button[_mouse_button_right] && !mouse.button_last_frame[_mouse_button_right]);
}

inline b32
right_mouse_button_held()
{
   return (mouse.button[_mouse_button_right]);
}

inline b32
right_mouse_button_released()
{
   return (!mouse.button[_mouse_button_right] && mouse.button_last_frame[_mouse_button_right]);
}

inline b32
middle_mouse_button_pressed()
{
   return mouse.button[_mouse_button_middle] && !mouse.button_last_frame[_mouse_button_middle];
}

inline b32
middle_mouse_button_held()
{
   return mouse.button[_mouse_button_middle];
}

inline b32
middle_mouse_button_released()
{
   return !mouse.button[_mouse_button_middle] && mouse.button_last_frame[_mouse_button_middle];
}

/*
void
set_cursor_hand(void)
{
    window.cursor = LoadCursor(0, IDC_HAND);
    SetCursor(window.cursor);
}


void
set_cursor_move(void)
{
    window.cursor = LoadCursor(0, IDC_SIZEALL);
    SetCursor(window.cursor);
}


void
set_cursor_arrow(void)
{
    window.cursor = LoadCursor(0, IDC_ARROW);
    SetCursor(window.cursor);
}
*/

void
update_window()
{
   window.number_of_drop_files = 0;
   window.resized              = 0;
   keyboard.chars_typed        = 0;
   keyboard.any_key_released   = 0;
   keyboard.any_key_pressed    = 0;
   memcpy(keyboard.key_state_last_frame, keyboard.key_state, sizeof(keyboard.key_state_last_frame));
   memcpy(mouse.button_last_frame, mouse.button, sizeof(mouse.button_last_frame));
   mouse.wheel_delta    = 0;
   mouse.position_delta = (struct v2s) { 0, 0 };
   // MSG msg = {0};
   // while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
   //{
   //     UINT Message = msg.message;
   //     WPARAM wParam = msg.wParam;
   //     LPARAM lParam = msg.lParam;

   //    if(window.AdditionalWndProc) {
   //        window.AdditionalWndProc(window.handle, Message, wParam, lParam);
   //    }
   //    switch (Message) {
   //    case WM_DROPFILES:
   //    {
   //        HDROP drop = (HDROP)wParam;
   //        uint32 number_of_files = DragQueryFileA(drop, 0xFFFFFFFF, 0, 1024);
   //        window.number_of_drop_files = min(MAX_DROP_FILES, (int32)number_of_files);
   //        for(uint32 index = 0; index < (uint32)window.number_of_drop_files; ++index)
   //        {
   //            uint32 file_name_length = DragQueryFileA(drop, index, 0, 1024);
   //            assert(file_name_length < 1024);
   //            DragQueryFileA(drop, index, window.drop_file[index], 1024);
   //        }
   //        DragFinish(drop);
   //    } break;
   //    case WM_MOUSEWHEEL:
   //    {
   //        mouse.wheel_delta += GET_WHEEL_DELTA_WPARAM(wParam)/120; /* microsoft defines one click to be 120 */
   //    } break;
   //    case WM_MOUSEMOVE:
   //    {
   //        int32 x = GET_X_LPARAM(lParam);
   //        int32 y = GET_Y_LPARAM(lParam);
   //        struct v2s position = {x, window.height - y - 1};
   //        mouse.position_delta = add_v2s(mouse.position_delta, sub_v2s(position, mouse.position));
   //        mouse.position = position;
   //    } break;
   //    case WM_LBUTTONDOWN:
   //    {
   //        SetCapture(window.handle);
   //        mouse.button[_mouse_button_left] = 1;
   //    } break;
   //    case WM_LBUTTONUP:
   //    {
   //        ReleaseCapture();
   //        mouse.button[_mouse_button_left] = 0;
   //    } break;
   //    case WM_RBUTTONDOWN:
   //    {
   //        SetCapture(window.handle);
   //        mouse.button[_mouse_button_right] = 1;
   //    } break;
   //    case WM_RBUTTONUP:
   //    {
   //        ReleaseCapture();
   //        mouse.button[_mouse_button_right] = 0;
   //    } break;
   //    case WM_MBUTTONDOWN:
   //    {
   //        mouse.button[_mouse_button_middle] = 1;
   //    } break;
   //    case WM_MBUTTONUP:
   //    {
   //        mouse.button[_mouse_button_middle] = 0;
   //    } break;
   //    case WM_CHAR:
   //    {
   //        if (keyboard.chars_typed < 10)
   //        {
   //            keyboard.chars[keyboard.chars_typed] = (s32)wParam;
   //            keyboard.chars_typed += 1;
   //        }
   //        else
   //        {
   //            printf("WARNING: to many chars per frame\n");
   //        }
   //    } break;
   //    case WM_KEYDOWN:
   //    {
   //        uint32 Key = (uint32)_MapLeftRightKeys(wParam, lParam);
   //        keyboard.key_state[_get_key_mapping(Key)] += 1;
   //        keyboard.any_key_pressed = 1;
   //    } break;
   //    case WM_KEYUP:
   //    {
   //        uint32 Key = (uint32)_MapLeftRightKeys(wParam, lParam);
   //        keyboard.key_state[_get_key_mapping(Key)] = 0;
   //        keyboard.any_key_released = 1;
   //    } break;
   //    case WM_QUIT:
   //    {
   //        window.running = 0;
   //    } break;
   //    default:
   //    {
   //        TranslateMessage(&msg);
   //        DispatchMessage(&msg);
   //    } break;
   //    }
   //}
}

/*
inline
b32
altkey_held()
{
    return GetKeyState(VK_MENU) & 0x8000;
}

inline
b32
ctrlkey_held()
{
    return GetKeyState(VK_CONTROL) & 0x8000;
}
*/

inline b32
key_held(enum keycode key)
{
   return keyboard.key_state[key] > 0;
}

inline b32
key_released(enum keycode key)
{
   return keyboard.key_state[key] == 0 && keyboard.key_state_last_frame[key] > 0;
}

inline b32
key_pressed(enum keycode key)
{
   return keyboard.key_state[key] != 0 && keyboard.key_state_last_frame[key] == 0;
}

inline b32
key_repeated(enum keycode key)
{
   return keyboard.key_state_last_frame[key] < keyboard.key_state[key];
}

inline b32
any_key_pressed()
{
   return keyboard.any_key_pressed;
}

inline b32
any_key_released()
{
   return keyboard.any_key_released;
}

/*
LRESULT CALLBACK
win_WndProc(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = 0;
    switch(Message)
    {

    case WM_CLOSE:
    {
        window.running = 0;
    } break;

    case WM_SIZE:
    {
        window.width = LOWORD(lParam);
        window.height = HIWORD(lParam);
        window.resized = 1;
    } break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        assert(!"Keyboard input how?");
    } break;

    case WM_SETCURSOR:
    {
        if (LOWORD(lParam) == HTCLIENT)
        {
            SetCursor(window.cursor);
            return TRUE;
        }
        Result = DefWindowProc(WindowHandle, Message, wParam, lParam);
    } break;

    case WM_PAINT:
    {
        PAINTSTRUCT paint;
        BeginPaint(window.handle, &paint);
        EndPaint(window.handle, &paint);
    } break;

    default:
    {
        Result = DefWindowProc(WindowHandle, Message, wParam, lParam);
    } break;
    }
    return(Result);
}

void
resize_window(int32 width, int32 height)
{
    RECT client_size, window_size;
    GetClientRect(window.handle, &client_size);
    GetWindowRect(window.handle, &window_size);
    int BarSize = abs((window_size.bottom - window_size.top) - (client_size.bottom - client_size.top));
    int BorderSize = abs((window_size.left - window_size.right) - (client_size.left - client_size.right));
    SetWindowPos(window.handle, HWND_TOP, 0, 0, width + BorderSize, height + BarSize, SWP_NOMOVE);
}
*/

static void
_Makewin_Char_KeyMapping_UpperCase(void)
{
   for (int i = 0; i < 256; i++)
   {
      win_Char_KeyMapping_UpperCase[i] = 0;
   }
   win_Char_KeyMapping_UpperCase[keycode_Minus]        = '_';
   win_Char_KeyMapping_UpperCase[keycode_Equals]       = '+';
   win_Char_KeyMapping_UpperCase[keycode_Comma]        = '<';
   win_Char_KeyMapping_UpperCase[keycode_Period]       = '>';
   win_Char_KeyMapping_UpperCase[keycode_Space]        = ' ';
   win_Char_KeyMapping_UpperCase[keycode_Slash]        = '?';
   win_Char_KeyMapping_UpperCase[keycode_BackSlash]    = '|';
   win_Char_KeyMapping_UpperCase[keycode_LeftBracket]  = '{';
   win_Char_KeyMapping_UpperCase[keycode_RightBracket] = '}';
   win_Char_KeyMapping_UpperCase[keycode_Tick]         = '"';
   win_Char_KeyMapping_UpperCase[keycode_SemiColon]    = ':';
   win_Char_KeyMapping_UpperCase[keycode_Tilde]        = '~';

   win_Char_KeyMapping_UpperCase[keycode_0] = ')';
   win_Char_KeyMapping_UpperCase[keycode_1] = '!';
   win_Char_KeyMapping_UpperCase[keycode_2] = '@';
   win_Char_KeyMapping_UpperCase[keycode_3] = '#';
   win_Char_KeyMapping_UpperCase[keycode_4] = '$';
   win_Char_KeyMapping_UpperCase[keycode_5] = '%';
   win_Char_KeyMapping_UpperCase[keycode_6] = '^';
   win_Char_KeyMapping_UpperCase[keycode_7] = '&';
   win_Char_KeyMapping_UpperCase[keycode_8] = '*';
   win_Char_KeyMapping_UpperCase[keycode_9] = '(';

   win_Char_KeyMapping_UpperCase[keycode_A] = 'A';
   win_Char_KeyMapping_UpperCase[keycode_B] = 'B';
   win_Char_KeyMapping_UpperCase[keycode_C] = 'C';
   win_Char_KeyMapping_UpperCase[keycode_D] = 'D';
   win_Char_KeyMapping_UpperCase[keycode_E] = 'E';
   win_Char_KeyMapping_UpperCase[keycode_F] = 'F';
   win_Char_KeyMapping_UpperCase[keycode_G] = 'G';
   win_Char_KeyMapping_UpperCase[keycode_H] = 'H';
   win_Char_KeyMapping_UpperCase[keycode_I] = 'I';
   win_Char_KeyMapping_UpperCase[keycode_J] = 'J';
   win_Char_KeyMapping_UpperCase[keycode_K] = 'K';
   win_Char_KeyMapping_UpperCase[keycode_L] = 'L';
   win_Char_KeyMapping_UpperCase[keycode_M] = 'M';
   win_Char_KeyMapping_UpperCase[keycode_N] = 'N';
   win_Char_KeyMapping_UpperCase[keycode_O] = 'O';
   win_Char_KeyMapping_UpperCase[keycode_P] = 'P';
   win_Char_KeyMapping_UpperCase[keycode_Q] = 'Q';
   win_Char_KeyMapping_UpperCase[keycode_R] = 'R';
   win_Char_KeyMapping_UpperCase[keycode_S] = 'S';
   win_Char_KeyMapping_UpperCase[keycode_T] = 'T';
   win_Char_KeyMapping_UpperCase[keycode_U] = 'U';
   win_Char_KeyMapping_UpperCase[keycode_V] = 'V';
   win_Char_KeyMapping_UpperCase[keycode_W] = 'W';
   win_Char_KeyMapping_UpperCase[keycode_X] = 'X';
   win_Char_KeyMapping_UpperCase[keycode_Y] = 'Y';
   win_Char_KeyMapping_UpperCase[keycode_Z] = 'Z';
}

static void
_Makewin_Char_KeyMapping_LowerCase(void)
{
   for (int i = 0; i < 256; i++)
   {
      win_Char_KeyMapping_LowerCase[i] = 0;
   }
   win_Char_KeyMapping_LowerCase[keycode_Minus]        = '-';
   win_Char_KeyMapping_LowerCase[keycode_Equals]       = '=';
   win_Char_KeyMapping_LowerCase[keycode_Comma]        = ',';
   win_Char_KeyMapping_LowerCase[keycode_Period]       = '.';
   win_Char_KeyMapping_LowerCase[keycode_Space]        = ' ';
   win_Char_KeyMapping_LowerCase[keycode_Slash]        = '/';
   win_Char_KeyMapping_LowerCase[keycode_BackSlash]    = '\\';
   win_Char_KeyMapping_LowerCase[keycode_LeftBracket]  = '[';
   win_Char_KeyMapping_LowerCase[keycode_RightBracket] = ']';
   win_Char_KeyMapping_LowerCase[keycode_Tick]         = '\'';
   win_Char_KeyMapping_LowerCase[keycode_SemiColon]    = ';';
   win_Char_KeyMapping_LowerCase[keycode_Tilde]        = '`';

   win_Char_KeyMapping_LowerCase[keycode_0] = '0';
   win_Char_KeyMapping_LowerCase[keycode_1] = '1';
   win_Char_KeyMapping_LowerCase[keycode_2] = '2';
   win_Char_KeyMapping_LowerCase[keycode_3] = '3';
   win_Char_KeyMapping_LowerCase[keycode_4] = '4';
   win_Char_KeyMapping_LowerCase[keycode_5] = '5';
   win_Char_KeyMapping_LowerCase[keycode_6] = '6';
   win_Char_KeyMapping_LowerCase[keycode_7] = '7';
   win_Char_KeyMapping_LowerCase[keycode_8] = '8';
   win_Char_KeyMapping_LowerCase[keycode_9] = '9';

   win_Char_KeyMapping_LowerCase[keycode_A] = 'a';
   win_Char_KeyMapping_LowerCase[keycode_B] = 'b';
   win_Char_KeyMapping_LowerCase[keycode_C] = 'c';
   win_Char_KeyMapping_LowerCase[keycode_D] = 'd';
   win_Char_KeyMapping_LowerCase[keycode_E] = 'e';
   win_Char_KeyMapping_LowerCase[keycode_F] = 'f';
   win_Char_KeyMapping_LowerCase[keycode_G] = 'g';
   win_Char_KeyMapping_LowerCase[keycode_H] = 'h';
   win_Char_KeyMapping_LowerCase[keycode_I] = 'i';
   win_Char_KeyMapping_LowerCase[keycode_J] = 'j';
   win_Char_KeyMapping_LowerCase[keycode_K] = 'k';
   win_Char_KeyMapping_LowerCase[keycode_L] = 'l';
   win_Char_KeyMapping_LowerCase[keycode_M] = 'm';
   win_Char_KeyMapping_LowerCase[keycode_N] = 'n';
   win_Char_KeyMapping_LowerCase[keycode_O] = 'o';
   win_Char_KeyMapping_LowerCase[keycode_P] = 'p';
   win_Char_KeyMapping_LowerCase[keycode_Q] = 'q';
   win_Char_KeyMapping_LowerCase[keycode_R] = 'r';
   win_Char_KeyMapping_LowerCase[keycode_S] = 's';
   win_Char_KeyMapping_LowerCase[keycode_T] = 't';
   win_Char_KeyMapping_LowerCase[keycode_U] = 'u';
   win_Char_KeyMapping_LowerCase[keycode_V] = 'v';
   win_Char_KeyMapping_LowerCase[keycode_W] = 'w';
   win_Char_KeyMapping_LowerCase[keycode_X] = 'x';
   win_Char_KeyMapping_LowerCase[keycode_Y] = 'y';
   win_Char_KeyMapping_LowerCase[keycode_Z] = 'z';
}

/*
static void _Makewin_Win32_KeyMapping(void) {
    for (int i = 0; i < 256; i++) {
        win_Win32_KeyMapping[i] = keycode_F12;
    }
    win_Win32_KeyMapping[VK_UP] = keycode_Up;
    win_Win32_KeyMapping[VK_LEFT] = keycode_Left;
    win_Win32_KeyMapping[VK_DOWN] = keycode_Down;
    win_Win32_KeyMapping[VK_RIGHT] = keycode_Right;
    win_Win32_KeyMapping[VK_DELETE] = keycode_Delete;
    win_Win32_KeyMapping[VK_OEM_1] = keycode_SemiColon;
    win_Win32_KeyMapping[VK_OEM_2] = keycode_Slash;
    win_Win32_KeyMapping[VK_OEM_3] = keycode_Tilde;
    win_Win32_KeyMapping[VK_OEM_4] = keycode_LeftBracket;
    win_Win32_KeyMapping[VK_OEM_5] = keycode_BackSlash;
    win_Win32_KeyMapping[VK_OEM_6] = keycode_RightBracket;
    win_Win32_KeyMapping[VK_OEM_7] = keycode_Tick;
    win_Win32_KeyMapping[VK_OEM_MINUS] = keycode_Minus;
    win_Win32_KeyMapping[VK_OEM_PLUS] = keycode_Equals;
    win_Win32_KeyMapping[VK_OEM_COMMA] = keycode_Comma;
    win_Win32_KeyMapping[VK_OEM_PERIOD] = keycode_Period;
    win_Win32_KeyMapping[VK_BACK] = keycode_Backspace;
    win_Win32_KeyMapping[VK_TAB] = keycode_Tab;
    win_Win32_KeyMapping[VK_RETURN] = keycode_Enter;

    win_Win32_KeyMapping[VK_CAPITAL] = keycode_CapsLock;
    win_Win32_KeyMapping[VK_ESCAPE] = keycode_Escape;
    win_Win32_KeyMapping[VK_SPACE] = keycode_Space;

    int vk0 = 0x30;
    win_Win32_KeyMapping[vk0++] = keycode_0;
    win_Win32_KeyMapping[vk0++] = keycode_1;
    win_Win32_KeyMapping[vk0++] = keycode_2;
    win_Win32_KeyMapping[vk0++] = keycode_3;
    win_Win32_KeyMapping[vk0++] = keycode_4;
    win_Win32_KeyMapping[vk0++] = keycode_5;
    win_Win32_KeyMapping[vk0++] = keycode_6;
    win_Win32_KeyMapping[vk0++] = keycode_7;
    win_Win32_KeyMapping[vk0++] = keycode_8;
    win_Win32_KeyMapping[vk0++] = keycode_9;
    int vka = 0x41;
    win_Win32_KeyMapping[vka++] = keycode_A;
    win_Win32_KeyMapping[vka++] = keycode_B;
    win_Win32_KeyMapping[vka++] = keycode_C;
    win_Win32_KeyMapping[vka++] = keycode_D;
    win_Win32_KeyMapping[vka++] = keycode_E;
    win_Win32_KeyMapping[vka++] = keycode_F;
    win_Win32_KeyMapping[vka++] = keycode_G;
    win_Win32_KeyMapping[vka++] = keycode_H;
    win_Win32_KeyMapping[vka++] = keycode_I;
    win_Win32_KeyMapping[vka++] = keycode_J;
    win_Win32_KeyMapping[vka++] = keycode_K;
    win_Win32_KeyMapping[vka++] = keycode_L;
    win_Win32_KeyMapping[vka++] = keycode_M;
    win_Win32_KeyMapping[vka++] = keycode_N;
    win_Win32_KeyMapping[vka++] = keycode_O;
    win_Win32_KeyMapping[vka++] = keycode_P;
    win_Win32_KeyMapping[vka++] = keycode_Q;
    win_Win32_KeyMapping[vka++] = keycode_R;
    win_Win32_KeyMapping[vka++] = keycode_S;
    win_Win32_KeyMapping[vka++] = keycode_T;
    win_Win32_KeyMapping[vka++] = keycode_U;
    win_Win32_KeyMapping[vka++] = keycode_V;
    win_Win32_KeyMapping[vka++] = keycode_W;
    win_Win32_KeyMapping[vka++] = keycode_X;
    win_Win32_KeyMapping[vka++] = keycode_Y;
    win_Win32_KeyMapping[vka++] = keycode_Z;

    win_Win32_KeyMapping[VK_LWIN] = keycode_LeftSuper;
    win_Win32_KeyMapping[VK_RWIN] = keycode_RightSuper;

    win_Win32_KeyMapping[VK_F1] = keycode_F1;
    win_Win32_KeyMapping[VK_F2] = keycode_F2;
    win_Win32_KeyMapping[VK_F3] = keycode_F3;
    win_Win32_KeyMapping[VK_F4] = keycode_F4;
    win_Win32_KeyMapping[VK_F5] = keycode_F5;
    win_Win32_KeyMapping[VK_F6] = keycode_F6;
    win_Win32_KeyMapping[VK_F7] = keycode_F7;
    win_Win32_KeyMapping[VK_F8] = keycode_F8;
    win_Win32_KeyMapping[VK_F9] = keycode_F9;
    win_Win32_KeyMapping[VK_F10] = keycode_F10;
    win_Win32_KeyMapping[VK_F11] = keycode_F11;
    win_Win32_KeyMapping[VK_F12] = keycode_F12;

    win_Win32_KeyMapping[VK_LSHIFT] = keycode_LeftShift;
    win_Win32_KeyMapping[VK_RSHIFT] = keycode_RightShift;
    win_Win32_KeyMapping[VK_LCONTROL] = keycode_LeftControl;
    win_Win32_KeyMapping[VK_RCONTROL] = keycode_RightControl;
    win_Win32_KeyMapping[VK_LMENU] = keycode_Alt;
    win_Win32_KeyMapping[VK_RMENU] = keycode_RightAlt;
}
*/

void
open_window(char *title, s32 width, s32 height)
{
   glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), 0);
   //_Makewin_Win32_KeyMapping();
   //_Makewin_Char_KeyMapping_LowerCase();
   //_Makewin_Char_KeyMapping_UpperCase();
   // LPCTSTR WindowTitle = title;

   // int WidthInPixel = 640;
   // int HeightInPixel = 480;

   // WNDCLASSEX WindowClass;
   // WindowClass.cbSize = sizeof(WNDCLASSEX);
   // WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
   // WindowClass.lpfnWndProc = win_WndProc;
   // WindowClass.cbClsExtra = 0;
   // WindowClass.cbWndExtra = 0;
   // WindowClass.hInstance = instance;
   // WindowClass.hIcon = LoadIcon(0, IDI_APPLICATION); // TODO: nicer icon
   // WindowClass.hCursor = 0;//LoadCursor(0, IDC_ARROW);
   // window.cursor = LoadCursor(0, IDC_ARROW);
   // WindowClass.hbrBackground = 0;
   // WindowClass.lpszMenuName = 0;
   // LPCTSTR WindowClassName = _T("BloxWindowClass");
   // WindowClass.lpszClassName = WindowClassName;
   // WindowClass.hIconSm = 0; // TODO: nicer icon

   // assert(RegisterClassEx(&WindowClass));

   // DWORD WindowStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
   // window.handle = CreateWindowEx(WindowStyle, WindowClassName, WindowTitle, WS_OVERLAPPEDWINDOW,
   //         CW_USEDEFAULT, 0, WidthInPixel+10, HeightInPixel, 0, 0, instance, 0);
   // assert(window.handle);

   // DragAcceptFiles(window.handle, TRUE);

   // window.device_context = GetDC(window.handle);

   // PIXELFORMATDESCRIPTOR pfd = {0};
   // pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
   // pfd.nVersion = 1;
   // pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
   // pfd.iPixelType = PFD_TYPE_RGBA;
   // pfd.cColorBits = 32;
   // pfd.cDepthBits = 32;
   // pfd.iLayerType = PFD_MAIN_PLANE;

   // int PixelFormat = ChoosePixelFormat(window.device_context, &pfd);
   // assert(PixelFormat != 0);

   // b32 Result = SetPixelFormat(window.device_context, PixelFormat, &pfd);
   // assert(Result != 0);
   // window.running = 1;

   // ShowWindow(window.handle, SW_SHOW);
   // UpdateWindow(window.handle);
   // SetForegroundWindow(window.handle);
   // window.width = width;
   // window.height = height;
   // resize_window(width, height);
}

void
set_window_position(struct v2s new_position)
{
   // SetWindowPos(window.handle, HWND_TOP, new_position.x, new_position.y, 0, 0, SWP_NOSIZE);
}
