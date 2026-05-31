#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define UNICODE

#include <windows.h>
#include <gl/GL.h>

#define DUMMY_WND_NAME L"dummy_WND"

typedef BOOL (wglChoosePixelFormatARB_func)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
static wglChoosePixelFormatARB_func* wglChoosePixelFormatARB = NULL;

typedef HGLRC (wglCreateContextAttribsARB_func)(HDC hDC, HGLRC hshareContext, const int* attribList);
static wglCreateContextAttribsARB_func* wglCreateContextAttribsARB = NULL;

#define WGL_DRAW_TO_WINDOW_ARB                  0x2001 
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_COLOR_BITS_ARB                      0x2014
#define WGL_DEPTH_BITS_ARB                      0x2022
#define WGL_STENCIL_BITS_ARB                    0x2023

uint32_t pixel_format = 0;

int main (){
    
    HINSTANCE module_handle = GetModuleHandle(NULL);
    {
        //DUMMY WINDOW
        WNDCLASS dummy_Wnd  = {
            .lpfnWndProc    = DefWindowProcW,
            .hInstance      = module_handle,
            .lpszClassName  = DUMMY_WND_NAME
        };
        RegisterClassW(&dummy_Wnd);

        //CONFIG
        HWND dumy_Wnd_Config = CreateWindowW(
            DUMMY_WND_NAME,
            L"dummy",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            NULL,
            NULL,
            NULL,
            NULL
        );
        HDC dummy_device_context = GetDC(&dumy_Wnd_Config);

        //PIXELS
        PIXELFORMATDESCRIPTOR pfd = {
            .nSize      = sizeof(PIXELFORMATDESCRIPTOR),
            .nVersion   = 1,
            .dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            .cColorBits = 24

        };
        uint32_t pf = ChoosePixelFormat(&dummy_device_context, &pfd);
        SetPixelFormat(dummy_device_context, pf, &pfd);

        //Enter Open gl
        HGLRC dummy_gl_context = wglCreateContext(dummy_device_context);
        wglMakeCurrent(dummy_device_context, dummy_gl_context);


        wglChoosePixelFormatARB    = (wglChoosePixelFormatARB_func*)wglGetProcAddress("wglCreateContextAttribsARB");
        wglCreateContextAttribsARB = (wglCreateContextAttribsARB_func*)wglGetProcAddress("wglCreateContextAttribsARB");

        uint32_t piAttribIList = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,               
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,              
            WGL_DOUBLE_BUFFER_ARB , GL_TRUE,                
            WGL_COLOR_BITS_ARB    , 24,                 
            WGL_DEPTH_BITS_ARB    , 32,                 
            WGL_STENCIL_BITS_ARB  , 8,               
            0

        };
        
        uint32_t num_format = 0;
        wglChoosePixelFormatARB(dummy_device_context, piAttribIList, NULL, 1, &pixel_format, &num_format);

    }


    BOOL wglChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);



    



    return 0;
}
