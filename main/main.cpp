// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

//使用异步log， 优点是速度快 缺点是发生异常可能会丢失LOG
//#define  ASYNC_LOGGER

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <spd_logger.h>
#include <configger.h>
#include "exports.h"
#include "DXGICapture.h"

#include "simhei.h" //font
#include <chrono>   //计算时间

#include "imcmd_command_palette.h"

using namespace std::chrono;

// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//全局变量， 线程共享
HWND g_main_hwnd = NULL;
int g_thread_id_hotkey = NULL;
LRESULT OnForceShow(HWND hWnd)
{
	HWND hForeWnd = NULL; 
	DWORD dwForeID = 0; 
	DWORD dwCurID = 0; 
 
	hForeWnd =  ::GetForegroundWindow(); 
	dwCurID = :: GetCurrentThreadId(); 
	dwForeID =  ::GetWindowThreadProcessId( hForeWnd, NULL ); 
	::AttachThreadInput( dwCurID, dwForeID, TRUE); 
	::ShowWindow( hWnd, SW_SHOWNORMAL ); 
	::SetWindowPos( hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); 
	::SetWindowPos( hWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); 
	::SetForegroundWindow( hWnd ); 
	::AttachThreadInput( dwCurID, dwForeID, FALSE);
 
	return S_OK;
}

void call_back_function()
{
    static bool s_is_show = true;
	TRACE("{}",GetCurrentThreadId());
	TRACE("i am in cpp");
    if (s_is_show)
        ShowWindow(g_main_hwnd, SW_HIDE);
    else
    {
        ShowWindow(g_main_hwnd, SW_NORMAL);
	    ::SetWindowPos( g_main_hwnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); 
    }
    s_is_show = !s_is_show;
	return;
}

void imgui_get_pix_color(int x, int y, std::string &color)
{
    char color_tmp[6] = "";
    void *ptr_color = NULL;
    void *ptr_bitmap = NULL;
    void *ptr_bitmap_scan0 = NULL;
    int is_time_out = 0;
    dxgi_pixelgetcolor(x, y, color_tmp, &ptr_color, &ptr_bitmap_scan0, &ptr_bitmap, &is_time_out);
    color = string(color_tmp);
    return;
}

//卡牌切牌
void imgui_card_back(wchar_t car_w[10])
{
    string car = wstr_utf8(car_w);
    auto& config = Configger::instance()->config();
    std::string color_obj;
    std::string color_pre;
    INFO(std::string(car));
    if (std::string("red") == std::string(car))
    {
        color_obj = config["color_red"];
        color_pre = config["color_blue"];
    }
    else if (std::string("yellow") == std::string(car))
    {
        color_obj = config["color_yello"];
        color_pre = config["color_red"];
    }
    else if (std::string("blue") == std::string(car))
    {
        color_obj = config["color_blue"];
        color_pre = config["color_yello"];
    }
    else
    {
        ERR("car error!");
    }
    TRACE(color_obj);
    TRACE(color_pre);
    config = Configger::instance()->config();
    auto x = config["color_x"];
    auto y = config["color_y"];

    string color("");
    imgui_get_pix_color(x, y, color);
    if (color == config["color_now"])
    {
        ahkFunction(_T("sendkey"), _T("t"), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, g_thread_id_hotkey);
		spdlog::stopwatch sw;
		do
		{
			imgui_get_pix_color(x, y, color);
		} while (sw.elapsed().count() < 6 && color != color_obj);
        if(color == color_obj)
        {
            ahkFunction(_T("sendkey"), _T("t"), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, g_thread_id_hotkey);
        }
    }
    else if (color == color_obj)// 当颜色为黄牌时
    {
    }
    else
    {
		spdlog::stopwatch sw;
		do
		{
			imgui_get_pix_color(x, y, color);
		} while (sw.elapsed().count() < 6 && color != color_obj && color != config["color_now"]);
        if(color == color_obj)
        {
            ahkFunction(_T("sendkey"), _T("t"), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, g_thread_id_hotkey);
        }
    }
}

//load font
void imgui_load_font(const float &size_pixels)
{
    auto fontAtlas = ImGui::GetIO().Fonts;
    fontAtlas->ClearFonts();
    auto glyphRange = fontAtlas->GetGlyphRangesVietnamese();
	glyphRange = fontAtlas->GetGlyphRangesChineseFull();

    ImFontConfig config_words{};
	config_words.OversampleH = 1;
	config_words.OversampleV = 1;

	fontAtlas->AddFontFromMemoryTTF(simhei_font(), simhei_size, size_pixels, &config_words, glyphRange);	
    return;
}


// Main code
int main(int, char**)
{
    int a = -799;
    TRACE(sizeof(a));
    UINT b = a;
    UINT c = 799;
    int d = c;
    TRACE(d);

    TRACE(b);
    TRACE("start");
    //hide console
    HWND h_console = GetConsoleWindow();
    ShowWindow(h_console, SW_HIDE);

	//dx初始化
    dxgi_init(5);
    char color[6] = "";
    void *ptr_color = NULL;
    void *ptr_bitmap = NULL;
    void *ptr_bitmap_scan0 = NULL;
    int is_time_out = 0;
    dxgi_pixelgetcolor(100, 100, color, &ptr_color, &ptr_bitmap_scan0, &ptr_bitmap, &is_time_out);
    Sleep(1000);
    dxgi_pixelgetcolor(100, 100, color, &ptr_color, &ptr_bitmap_scan0, &ptr_bitmap, &is_time_out);
    TRACE("{}", color);

	//ahk 线程  用于热键
	wstringstream ss; 
	wstring func_id(L"");
	ss << &call_back_function;
	ss >> func_id;
	func_id = L"0x" + func_id;

	wstringstream sss; 
    wstring car_func_id(L"");
    sss << &imgui_card_back;
    sss >> car_func_id;
    func_id = func_id + L" " + L"0x" + car_func_id;
	//int thread_id = NewThread(L"#include G:\\c++学习\\vsproject\\imguitest\\imgui-docking-1.88\\imgui模板\\main\\cpp.ah2", func_id.c_str());
	//g_thread_id_hotkey = NewThread(L"#include G:\\c++学习\\vsproject\\imguitest\\imgui-docking-1.88\\imgui模板\\main\\hotkey.ah2", func_id.c_str());
	int thread_id = NewThread(L"#include cpp.ah2", func_id.c_str());
	g_thread_id_hotkey = NewThread(L"#include hotkey.ah2", func_id.c_str());
    while (!ahkReady(g_thread_id_hotkey) && !ahkReady(thread_id))
    {
    }
    INFO("start");
    //ahkPostFunction(_T("sendkey"),NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, g_thread_id_hotkey);
    INFO("end");

    INFO("load config");
	//load config 加载配置
    //Configger::instance()->set_config_path(u8"G:\\c++学习\\vsproject\\imguitest\\imgui-docking-1.88\\imgui模板\\main\\config\\config.json");
    Configger::instance()->set_config_path(u8"config.json");
    Configger::instance()->load();
    auto &config = Configger::instance()->config();
    Configger::instance()->save();

    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
    //HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, -100, -100, 1, 1, NULL, NULL, wc.hInstance, NULL);
    g_main_hwnd = hwnd;
	::SetWindowPos( hwnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); 

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    imgui_load_font(20);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //===============================cmd=============================================
    // Note: if you only have one context, simply call ImCmd::CreateContext(); here
    constexpr int kContextCount = 4;
    ImCmd::Context* contexts[kContextCount];
    int current_context = 0;
    for (int i = 0; i < kContextCount; ++i) {
        contexts[i] = ImCmd::CreateContext();
    }

    bool show_command_palette = false;
    bool show_custom_command_palette = false;

    ImCmd::Command toggle_demo_cmd;
    toggle_demo_cmd.Name = "Toggle ImGui demo window";
    toggle_demo_cmd.InitialCallback = [&]() {
        show_demo_window = !show_demo_window;
    };
    ImCmd::AddCommand(std::move(toggle_demo_cmd));

    ImCmd::Command select_theme_cmd;
    select_theme_cmd.Name = "Select theme";
    select_theme_cmd.InitialCallback = [&]() {
        ImCmd::Prompt(std::vector<std::string>{
            "Classic",
            "Dark",
            "Light",
        });
    };
    select_theme_cmd.SubsequentCallback = [&](int selected_option) {
        switch (selected_option) {
            case 0: ImGui::StyleColorsClassic(); break;
            case 1: ImGui::StyleColorsDark(); break;
            case 2: ImGui::StyleColorsLight(); break;
            default: break;
        }
    };
    ImCmd::AddCommand(std::move(select_theme_cmd));

    ImCmd::Command example_cmd;
    example_cmd.Name = "Example command";

    ImCmd::Command add_example_cmd_cmd;
    add_example_cmd_cmd.Name = "Add 'Example command'";
    add_example_cmd_cmd.InitialCallback = [&]() {
        ImCmd::AddCommand(example_cmd);
    };

    ImCmd::Command remove_example_cmd_cmd;
    remove_example_cmd_cmd.Name = "Remove 'Example command'";
    remove_example_cmd_cmd.InitialCallback = [&]() {
        ImCmd::RemoveCommand(example_cmd.Name);
    },

    ImCmd::AddCommand(example_cmd); // Copy intentionally
    ImCmd::AddCommand(std::move(add_example_cmd_cmd));
    ImCmd::AddCommand(std::move(remove_example_cmd_cmd));

    bool use_highlight_font = true;
    bool use_highlight_font_color = false;
    ImVec4 highlight_font_color(1.0f, 0.0f, 0.0f, 1.0f);

    int custom_counter = 0;
    //=======================================================================================
	
    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //=========================command window==========================================
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_P, false)) {
            show_command_palette = !show_command_palette;
        }
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
            show_custom_command_palette = !show_custom_command_palette;
        }

        if (show_command_palette) {
            ImCmd::CommandPaletteWindow("CommandPalette", &show_command_palette);
        }
        if (show_custom_command_palette) {
            ImCmd::SetNextWindowAffixedTop(ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x * 0.3f, 0.0f), ImGuiCond_Once);
            ImGui::Begin("CustomCommandPalette");

            ImGui::Text("Hi! This is a custom label");
            ImGui::Text("You may also notice that this window has a title bar and is movable - that's also custom");

            ImGui::Separator();

            ImGui::Text("Counter: %d", custom_counter);
            ImGui::SameLine();
            if (ImGui::Button("Click me")) {
                ++custom_counter;
            }

            // BEGIN command palette widget
            // Note: see ImCmd::CommandPaletteWindow for all the default behaviors, we've omitted some here
            if (ImGui::IsWindowAppearing()) {
                ImCmd::SetNextCommandPaletteSearchBoxFocused();
            }

            ImCmd::CommandPalette("");

            if (ImCmd::IsAnyItemSelected()) {
                //show_custom_command_palette = false;
            }
            // BEGIN command palette widget

            ImGui::End();
        }
        //===========================================================================




        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button(u8"显示隐藏终端"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            {
                counter++;
                TRACE("{}", counter);
                bool bVisible = (::GetWindowLong(h_console, GWL_STYLE) & WS_VISIBLE) != 0;
                if (bVisible)
                    ShowWindow(h_console, SW_HIDE);
                else
                    ShowWindow(h_console, SW_NORMAL);

            }
            static int x_position = 100;
            ImGui::InputInt(u8"x坐标##x坐标", &x_position);

            static int y_position = 100;
            ImGui::InputInt(u8"y坐标##y坐标", &y_position);

            static string color = "";
            ImGui::Text("%s", color);
            if (ImGui::Button("log button"))
            {
                imgui_get_pix_color(x_position, y_position, color);
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

			

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
