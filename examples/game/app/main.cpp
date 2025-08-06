//#define DORY_BENCHMARK_OPENGL
//#define DORY_BENCHMARK_DIRECTX
//#define DORY_BENCHMARK_OPENGL2

#ifndef DORY_BENCHMARK_OPENGL
#ifndef DORY_BENCHMARK_DIRECTX
#ifndef DORY_BENCHMARK_OPENGL2

#include "dory/game/setup.h"
#include <dory/core/services/logService.h>
#include <dory/core/resources/localizationImpl.h>
#include <dory/game/bootstrap.h>
#include "game.h"

#include <iostream>
#include <dory/profiling/profiler.h>

static auto systemMemPoolName = "System";

void* operator new(const std::size_t size)
{
    void* ptr = std::malloc(size);
    DORY_TRACE_MEM_ALLOC(ptr, size, systemMemPoolName);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    DORY_TRACE_MEM_FREE(ptr, systemMemPoolName);
    std::free(ptr);
}

void* operator new[](const std::size_t size)
{
    void* ptr = std::malloc(size);
    DORY_TRACE_MEM_ALLOC(ptr, size, systemMemPoolName);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void operator delete[](void* ptr) noexcept
{
    DORY_TRACE_MEM_FREE(ptr, systemMemPoolName);
    std::free(ptr);
}

#ifdef DORY_MAIN_FUNCTION_UNIX
int main()
#endif
#ifdef DORY_MAIN_FUNCTION_WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    DORY_TRACE_START();

    dory::game::Setup setup;
    dory::generic::extension::LibraryHandle staticLibraryHandle {};
    auto registry = dory::core::Registry{};
    auto configuration = dory::core::resources::configuration::Configuration{};
    auto localization = dory::core::resources::LocalizationImpl{};
    auto context = dory::core::resources::DataContext{ configuration, localization };

    setup.setupRegistry(staticLibraryHandle, registry, configuration);

    //initial config
    configuration.section.loadFrom.emplace_back("settings.yaml");
    auto& bootLoggerConfig = configuration.loggingConfiguration.configurationLogger;
    bootLoggerConfig.name = "boot";
    bootLoggerConfig.rotationLogger = dory::core::resources::configuration::RotationLogSink{"logs/boot.log"};
    bootLoggerConfig.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};

    auto bootstrap = dory::game::Bootstrap{registry};
    bootstrap.initialize(staticLibraryHandle, context);

    dory::core::resources::scene::Scene* rootScene;

    if(auto sceneConfigurationService = registry.get<dory::core::services::ISceneConfigurationService>())
    {
        dory::core::resources::scene::configuration::SceneConfiguration sceneConfig;
        sceneConfigurationService->load("scenes/root.yaml", sceneConfig, context);

        if(auto sceneBuilder = registry.get<dory::core::services::ISceneBuilder>())
        {
            rootScene = sceneBuilder->build(sceneConfig, context);
        }
    }

    auto game = dory::game::Game{ registry };
    game.initialize(staticLibraryHandle, context);

    bootstrap.run(context);

    if(rootScene)
    {
        if(auto sceneBuilder = registry.get<dory::core::services::ISceneBuilder>())
        {
            sceneBuilder->destroy(*rootScene, context);
        }
    }

    bootstrap.cleanup(context);

    DORY_TRACE_SHUTDOWN();

    return 0;
}
#endif
#endif
#endif

#ifdef DORY_BENCHMARK_OPENGL

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

// Vertex and Fragment Shader Sources
const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    uniform float xOffset;
    void main() {
        gl_Position = vec4(aPos.x + xOffset, aPos.y, 0.0, 1.0);
    }
)";
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
    }
)";

// Utility functions to create and link shaders
GLuint compileShader(GLenum type, const char* source);
GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);

int main() {
    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST)) {
        std::cerr << "Failed to set thread priority!" << std::endl;
    }

    std::cout << "Clock resolution: " << std::chrono::high_resolution_clock::period::num << "/"
              << std::chrono::high_resolution_clock::period::den << " seconds" << std::endl;

    // Initialize GLFW
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Smooth Square", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(0); // Enable V-Sync

    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Vertex Data
    float vertices[] = { -0.05f, -0.05f, 0.05f, -0.05f, 0.05f, 0.05f, -0.05f, 0.05f };
    GLuint indices[] = { 0, 1, 2, 2, 3, 0 };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Movement variables
    float squarePosition = -0.9f, squareSpeed = 0.5f;
    bool movingRight = true;

    const float targetFrameTime = 16.67f;
    auto frameStartTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float elapsedTime = std::chrono::duration<float, std::milli>(currentTime - frameStartTime).count();
        frameStartTime = currentTime;

        /*if (elapsedTime < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(targetFrameTime - elapsedTime)));
        }

        frameStartTime += std::chrono::milliseconds(static_cast<int>(targetFrameTime));*/

        // Update square position
        if (movingRight) {
            squarePosition += squareSpeed * (elapsedTime / 1000.0f);
            if (squarePosition >= 0.9f) movingRight = false;
        } else {
            squarePosition -= squareSpeed * (elapsedTime / 1000.0f);
            if (squarePosition <= -0.9f) movingRight = true;
        }

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), squarePosition);
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Frame timing
        //auto frameEndTime = std::chrono::high_resolution_clock::now();
        //float frameDuration = std::chrono::duration<float, std::milli>(frameEndTime - frameStartTime).count();
        //while (frameDuration < targetFrameTime) {
        //    frameEndTime = std::chrono::high_resolution_clock::now();
        //    frameDuration = std::chrono::duration<float, std::milli>(frameEndTime - frameStartTime).count();
        //}
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

#endif

#ifdef DORY_BENCHMARK_DIRECTX

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <cstdio>

// Link necessary d3d libraries.
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    float x, y, z;
    float r, g, b;
};

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst         = nullptr;
HWND                    g_hWnd          = nullptr;
D3D_FEATURE_LEVEL       g_featureLevel  = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice    = nullptr;
ID3D11DeviceContext*    g_pImmediateContext = nullptr;
IDXGISwapChain*         g_pSwapChain    = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11Buffer*           g_pVertexBuffer = nullptr;
ID3D11InputLayout*      g_pVertexLayout = nullptr;
ID3D11VertexShader*     g_pVertexShader = nullptr;
ID3D11PixelShader*      g_pPixelShader  = nullptr;
ID3D11RasterizerState*  g_pWireframeRS  = nullptr;

// Triangle movement parameters
float g_posX     = 0.0f;    // Current horizontal position
float g_velocity = 0.01f;   // Movement speed

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void    CleanupDevice();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void    Render();
void    Update(float deltaTime);

//--------------------------------------------------------------------------------------
// Shader source code (embedded HLSL)
//--------------------------------------------------------------------------------------
const char* g_VS = R"(
struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Col : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Col : COLOR;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = float4(input.Pos, 1.0f);
    output.Col = input.Col;
    return output;
}
)";

const char* g_PS = R"(
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Col : COLOR;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return float4(input.Col, 1.0f);
}
)";

//--------------------------------------------------------------------------------------
// Entry point to the program (ANSI)
//--------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (FAILED(InitWindow(hInstance, nCmdShow)))
        return 0;

    if (FAILED(InitDevice()))
    {
        CleanupDevice();
        return 0;
    }

    // Timer setup
    LARGE_INTEGER frequency;
    LARGE_INTEGER prevTime;
    LARGE_INTEGER currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&prevTime);

    MSG msg = {0};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        else
        {
            // Calculate deltaTime
            QueryPerformanceCounter(&currentTime);
            float deltaTime = static_cast<float>(
                    double(currentTime.QuadPart - prevTime.QuadPart) / double(frequency.QuadPart));
            prevTime = currentTime;

            // Update logic
            Update(deltaTime);

            // Render the frame
            Render();
        }
    }

    CleanupDevice();
    return (int) msg.wParam;
}

//--------------------------------------------------------------------------------------
// Register class and create window (ANSI)
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEXA wcex;
    wcex.cbSize         = sizeof(WNDCLASSEXA);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIconA(nullptr, IDI_APPLICATION);
    wcex.hCursor        = LoadCursorA(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = "WireframeWindowClass";
    wcex.hIconSm        = LoadIconA(nullptr, IDI_APPLICATION);

    if (!RegisterClassExA(&wcex))
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindowA(
            "WireframeWindowClass",
            "Wireframe Triangle Stuttering Test (DirectX 11) - ANSI",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rc.right - rc.left,
            rc.bottom - rc.top,
            nullptr,
            nullptr,
            hInstance,
            nullptr
    );

    if (!g_hWnd)
        return E_FAIL;

    ShowWindow(g_hWnd, nCmdShow);
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    // Get window dimensions
    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width  = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
    // #ifdef _DEBUG
    // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    // #endif

    D3D_DRIVER_TYPE driverTypes[] =
            {
                    D3D_DRIVER_TYPE_HARDWARE,
                    D3D_DRIVER_TYPE_WARP,
                    D3D_DRIVER_TYPE_REFERENCE,
            };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
            {
                    D3D_FEATURE_LEVEL_11_0,
            };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    // Swap chain descriptor
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount                        = 1;
    sd.BufferDesc.Width                   = width;
    sd.BufferDesc.Height                  = height;
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = g_hWnd;
    sd.SampleDesc.Count                   = 1;
    sd.SampleDesc.Quality                 = 0;
    sd.Windowed                           = TRUE;

    // Create device and swap chain
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(
                nullptr,
                driverType,
                nullptr,
                createDeviceFlags,
                featureLevels,
                numFeatureLevels,
                D3D11_SDK_VERSION,
                &sd,
                &g_pSwapChain,
                &g_pd3dDevice,
                &g_featureLevel,
                &g_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                 reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width    = static_cast<float>(width);
    vp.Height   = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    g_pImmediateContext->RSSetViewports(1, &vp);

    // Compile vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = D3DCompile(g_VS, strlen(g_VS), nullptr, nullptr, nullptr,
                    "VSMain", "vs_4_0", 0, 0, &pVSBlob, nullptr);
    if (FAILED(hr))
    {
        MessageBoxA(nullptr, "Error compiling vertex shader.", "Shader Compile Error", MB_OK);
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(),
                                          nullptr, &g_pVertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
            {
                    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                         D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float)*3,           D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout(layout, numElements,
                                         pVSBlob->GetBufferPointer(),
                                         pVSBlob->GetBufferSize(),
                                         &g_pVertexLayout);
    pVSBlob->Release();
    if (FAILED(hr))
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

    // Compile pixel shader
    ID3DBlob* pPSBlob = nullptr;
    hr = D3DCompile(g_PS, strlen(g_PS), nullptr, nullptr, nullptr,
                    "PSMain", "ps_4_0", 0, 0, &pPSBlob, nullptr);
    if (FAILED(hr))
    {
        MessageBoxA(nullptr, "Error compiling pixel shader.", "Shader Compile Error", MB_OK);
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
                                         pPSBlob->GetBufferSize(),
                                         nullptr, &g_pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return hr;

    // Create vertex buffer for a single red triangle
    // We'll define a small triangle around the origin. We'll shift it by g_posX each frame.
    SimpleVertex vertices[] =
            {
                    // x,    y,     z,     r,   g,   b
                    { -0.05f, -0.05f, 0.0f,   1.0f, 0.0f, 0.0f },  // bottom-left  (red)
                    {  0.05f, -0.05f, 0.0f,   1.0f, 0.0f, 0.0f },  // bottom-right (red)
                    {  0.00f,  0.05f, 0.0f,   1.0f, 0.0f, 0.0f },  // top-center   (red)
            };

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth      = sizeof(vertices);
    bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // We'll update positions each frame

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
    if (FAILED(hr))
        return hr;

    // Create a wireframe rasterizer state
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(rasterDesc));
    rasterDesc.FillMode              = D3D11_FILL_WIREFRAME;
    rasterDesc.CullMode              = D3D11_CULL_NONE;
    rasterDesc.DepthClipEnable       = TRUE;
    hr = g_pd3dDevice->CreateRasterizerState(&rasterDesc, &g_pWireframeRS);
    if (FAILED(hr))
        return hr;

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    // Clear the back buffer to black
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

    // Set up the input assembler
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set wireframe rasterizer
    g_pImmediateContext->RSSetState(g_pWireframeRS);

    // Set shaders
    g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

    // Update the triangle's X-position in the vertex buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = g_pImmediateContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        SimpleVertex* pData = (SimpleVertex*)mappedResource.pData;

        // Base positions for the triangle (centered around x=0)
        // We'll apply g_posX offset to each vertex
        pData[0].x = -0.05f + g_posX;  // bottom-left
        pData[0].y = -0.05f;
        pData[0].z =  0.0f;
        pData[0].r =  1.0f; pData[0].g = 0.0f; pData[0].b = 0.0f;

        pData[1].x =  0.05f + g_posX;  // bottom-right
        pData[1].y = -0.05f;
        pData[1].z =  0.0f;
        pData[1].r =  1.0f; pData[1].g = 0.0f; pData[1].b = 0.0f;

        pData[2].x =  0.00f + g_posX;  // top-center
        pData[2].y =  0.05f;
        pData[2].z =  0.0f;
        pData[2].r =  1.0f; pData[2].g = 0.0f; pData[2].b = 0.0f;

        g_pImmediateContext->Unmap(g_pVertexBuffer, 0);
    }

    // Draw the triangle (3 vertices)
    g_pImmediateContext->Draw(3, 0);

    // Present the back buffer
    g_pSwapChain->Present(0, 0);
}

//--------------------------------------------------------------------------------------
// Update game logic
//--------------------------------------------------------------------------------------
void Update(float deltaTime)
{
    // Move the triangle side to side
    // Multiply velocity by ~60 to get consistent speed across varied deltaTimes
    g_posX += g_velocity * deltaTime * 60.0f;

    // Bounce the triangle within -1.0f to +1.0f range
    float offset = 0.05f;
    if (g_posX > 1.0f - offset)
    {
        g_posX = 1.0f - offset;
        g_velocity = -g_velocity;
    }
    else if (g_posX < -1.0f + offset)
    {
        g_posX = -1.0f + offset;
        g_velocity = -g_velocity;
    }
}

//--------------------------------------------------------------------------------------
// Cleanup
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if (g_pImmediateContext) g_pImmediateContext->ClearState();
    if (g_pWireframeRS)      g_pWireframeRS->Release();
    if (g_pVertexBuffer)     g_pVertexBuffer->Release();
    if (g_pVertexLayout)     g_pVertexLayout->Release();
    if (g_pVertexShader)     g_pVertexShader->Release();
    if (g_pPixelShader)      g_pPixelShader->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain)        g_pSwapChain->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice)        g_pd3dDevice->Release();
}

//--------------------------------------------------------------------------------------
// Window Procedure (ANSI)
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProcA(hWnd, message, wParam, lParam);
    }

    return 0;
}

#endif

#ifdef DORY_BENCHMARK_OPENGL2

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Vertex and Fragment Shader Sources
const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    uniform float xOffset;
    void main() {
        gl_Position = vec4(aPos.x + xOffset, aPos.y, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
    }
)";

// Utility functions to create and link shaders
GLuint compileShader(GLenum type, const char* source);
GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);

int main() {
    // Try setting thread priority high (as in your original code)
    /*if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST)) {
        std::cerr << "Failed to set thread priority!" << std::endl;
    }*/

    // Print clock resolution info (from your original code)
    std::cout << "Clock resolution: "
              << std::chrono::high_resolution_clock::period::num << "/"
              << std::chrono::high_resolution_clock::period::den
              << " seconds" << std::endl;

    // Initialize GLFW
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Smooth Triangle", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL with glad
    gladLoadGL(glfwGetProcAddress);

    // NOTE: The original code sets "glfwSwapInterval(0);" but comments say "Enable V-Sync".
    // In reality, '0' means *NO* vsync. We'll keep it exactly as-is for consistency
    // with your old code that you said was "smooth".
    glfwSwapInterval(0);

    // Compile and link shaders
    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // -------------------------------
    // TRIANGLE GEOMETRY (new part)
    // A small wireframe triangle
    // 3 vertices, 3 indices
    // -------------------------------
    float vertices[] = {
            //   x       y
            -0.05f, -0.05f,  // Bottom-left
            0.05f, -0.05f,  // Bottom-right
            0.00f,  0.05f,  // Top-center
    };
    GLuint indices[] = { 0, 1, 2 };

    // Create VAO, VBO, EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Element buffer data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Movement variables (similar to original code)
    float shapePosition = -0.9f;
    float shapeSpeed    = 0.5f;
    bool movingRight    = true;

    // Frame timing setup (same as original code)
    const float targetFrameTime = 16.67f;
    auto frameStartTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float elapsedTime = std::chrono::duration<float, std::milli>(currentTime - frameStartTime).count();
        frameStartTime = currentTime;

        // (Optional) The original code had commented-out logic to Sleep or busy-wait:
        // If you want to restore the frame capping, uncomment lines below:
        /*
        if (elapsedTime < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(targetFrameTime - elapsedTime)));
        }
        frameStartTime += std::chrono::milliseconds(static_cast<int>(targetFrameTime));
        */

        // Update shape position
        // Convert elapsedTime from ms to seconds
        float deltaSeconds = elapsedTime / 1000.0f;
        if (movingRight) {
            shapePosition += shapeSpeed * deltaSeconds;
            if (shapePosition >= 0.9f) movingRight = false;
        } else {
            shapePosition -= shapeSpeed * deltaSeconds;
            if (shapePosition <= -0.9f) movingRight = true;
        }

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), shapePosition);

        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
        // We have 3 indices for the triangle
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader   = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

#endif