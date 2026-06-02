#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define UNICODE

#include <windows.h>
#include <gl/GL.h>

#define DUMMY_WND_NAME L"dummy_WND"
#define REAL_WND_NAME L"real_WND"

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

//open gl verions
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092

int32_t pixel_format         =                  0;
uint32_t should_window_close =                  0;

static LRESULT window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


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
        HDC dummy_device_context = GetDC(dumy_Wnd_Config);

        //PIXELS
        PIXELFORMATDESCRIPTOR pfd = {
            .nSize      = sizeof(PIXELFORMATDESCRIPTOR),
            .nVersion   = 1,
            .dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            .cColorBits = 24

        };
        uint32_t pf = ChoosePixelFormat(dummy_device_context, &pfd);
        SetPixelFormat(dummy_device_context, pf, &pfd);

        //Enter Open gl
        HGLRC dummy_gl_context = wglCreateContext(dummy_device_context);
        wglMakeCurrent(dummy_device_context, dummy_gl_context);


        wglChoosePixelFormatARB    = (wglChoosePixelFormatARB_func*)wglGetProcAddress("wglCreateContextAttribsARB");
        wglCreateContextAttribsARB = (wglCreateContextAttribsARB_func*)wglGetProcAddress("wglCreateContextAttribsARB");

        int32_t piAttribIList[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,               
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,              
            WGL_DOUBLE_BUFFER_ARB , GL_TRUE,                
            WGL_COLOR_BITS_ARB    , 24,                 
            WGL_DEPTH_BITS_ARB    , 32,                 
            WGL_STENCIL_BITS_ARB  , 8,               
            0

        };


        uint32_t num_format   = 0;
        wglChoosePixelFormatARB(dummy_device_context, piAttribIList, NULL, 1, &pixel_format, &num_format);

        wglMakeCurrent(dummy_device_context, NULL);
        wglDeleteContext(dummy_gl_context);
        ReleaseDC(dumy_Wnd_Config, dummy_device_context);
        DestroyWindow(dumy_Wnd_Config);
        UnregisterClass(DUMMY_WND_NAME, module_handle);

    }
    

    //REAL WINDOW CLASS
    WNDCLASS real_wnd_class  = {
        .lpfnWndProc         = DefWindowProcW,
        .hInstance           = module_handle,
        .hCursor             = LoadCursorW(0, IDC_ARROW),
        .lpszClassName       = REAL_WND_NAME
    };
    RegisterClassW(&real_wnd_class);


    HWND real_window_config = CreateWindowW(
        REAL_WND_NAME,
        L"DOES THIS THING WORK",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1280, 
        720,
        NULL, 
        NULL, 
        NULL, 
        NULL
    );

    HDC real_dc               = GetDC(real_window_config);
    PIXELFORMATDESCRIPTOR pdf = {0};
    DescribePixelFormat(real_dc, pixel_format,sizeof(PIXELFORMATDESCRIPTOR), &pdf);

    int32_t context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        0
    };

    HGLRC real_gl_context = wglCreateContextAttribsARB(real_dc, NULL, context_attribs);
    wglMakeCurrent(real_dc, real_gl_context);
    
    ShowWindow(real_window_config, SW_SHOW);


    while(!should_window_close){
        MSG message = {0};
        while(PeekMessageW(&message, NULL, 0,0, PM_REMOVE)){
            TranslateMessage(&message);
            DispatchMessage(&message);

        }
        SwapBuffers(real_dc);
    };

    wglMakeCurrent(real_dc, NULL);
    wglDeleteContext(real_gl_context);
    ReleaseDC(real_window_config, real_dc);
    DestroyWindow(real_window_config);
    UnregisterClass(REAL_WND_NAME, module_handle);
    return 0;
}

static LRESULT window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_CLOSE: {
            should_window_close = true;
        }break;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}