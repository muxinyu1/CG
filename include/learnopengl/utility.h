//
// Created by MXY on 7/8/2022.
//

#ifndef CG_UTILITY_H
#define CG_UTILITY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <exception>

class utility {
    using window = GLFWwindow;
public:
    static void init(int major = 3, int minor = 3);

    static window* creat_window(int width, int height, const std::string &title, bool make_it_context = true,
                                void (*mouse_callback)(window *, double, double) = nullptr,
                                void(*scroll_callback)(window *, double, double) = nullptr);
};


#endif //CG_UTILITY_H
