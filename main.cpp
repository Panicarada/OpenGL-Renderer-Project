#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <string>
#include "Renderer.h"

#include "Scene.h"


std::shared_ptr<Camera> camera = nullptr;
test::Test* CurrentTest = nullptr;

// GUI库
#include "imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"


int main()
{
    Basic::init();
    /* GLFW初始化 */
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    // Decide GL+GLSL versions
#ifdef __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 不能改变窗口大小
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)

    window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "My App", nullptr, nullptr);

    if( window == nullptr )
    {
        fprintf( stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    /* ImGUI初始化 */
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    auto testMenu = new test::TestMenu(CurrentTest);
    CurrentTest = testMenu;

    testMenu->RegisterTest<test::Scene>("Scene");

    float lastTime = 0.0f;

    // 注册鼠标回调函数
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
        if (camera)
        {
            camera->OnMouseAction(window, {xPos, yPos});
        }
    });

    // 注册键盘回调函数
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
        if (CurrentTest && action == GLFW_PRESS)
        {
            CurrentTest->OnKeyAction(key, mods);
        }
        if (camera)
        {
            camera->OnKeyAction(window, key, scancode, action, mods);
        }
    });
    // 注册触摸板回调函数
    glfwSetScrollCallback(window, [](GLFWwindow* window, double deltaX, double deltaY){
        if (camera)
        {
            camera->OnScrollAction(glm::vec2(deltaX, deltaY));
        }
    });

    /* 主循环 */
    while( !glfwWindowShouldClose(window) )
    {
        DebugCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f))
        Renderer::clear();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (CurrentTest)
        {
            camera = CurrentTest->getCamera();
            float currentTime = glfwGetTime();
            CurrentTest->OnUpdate(window, currentTime - lastTime);
            lastTime = currentTime;
            CurrentTest->OnRender();
            ImGui::Begin("Test");
            if (CurrentTest != testMenu && ImGui::Button("<-"))
            {
                delete CurrentTest;
                CurrentTest = testMenu;
            }
            CurrentTest->OnImGuiRender();
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /* 清空指针 */
    delete CurrentTest;
    if (CurrentTest != testMenu)
    {
        delete testMenu;
    }
    /* ImGUI Cleanup */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    /* GLFW Cleanup */
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
