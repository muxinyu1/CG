//
// Created by MXY on 7/8/2022.
//

#include "utility.h"

void utility::init(int major, int minor) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

utility::window *
utility::creat_window(const int width, const int height, const std::string &title, bool make_it_context,
                      void (*mouse_callback)(window *, double, double),
                      void (*scroll_callback)(window *, double, double)) {
    const auto window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        throw "Can't create a window.\n";
    }
    if (make_it_context) {
        glfwMakeContextCurrent(window);
    }
    glfwSetFramebufferSizeCallback(window, [](auto* window, auto width, auto height){
        glViewport(0, 0, width, height);
    });
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // 让GLFW捕捉鼠标动作
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad加载所有OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw "Failed to initialize GLAD";
    }
    return window;
}

