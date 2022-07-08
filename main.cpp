#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <iostream>
#include <learnopengl/vertices.h>
#include <learnopengl/utility.h>
#include <learnopengl/model.h>

// 窗口尺寸设置
const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 720;

// 照相机实例化
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 计时
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 检测W，S，A，D，Esc键的按下与释放，并做出相应
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    // 按E键关闭与开启摄像机灯光
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        //TODO:关闭与开启灯光
    }
}

// 检测窗口尺寸是否发生变化
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // 确保视口与新窗口尺寸匹配；宽度和高度将明显大于显示器上的指定值。
    glViewport(0, 0, width, height);
}

// 当鼠标移动时回调
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    auto xpos = static_cast<float>(xposIn);
    auto ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 当y坐标从底部到顶部时反转

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// 当鼠标滚轮操作时回调
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


// 用于从文件加载2D纹理的函数
unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

int main() {
    // glfw的初始化和配置
    utility::init();

    // glfw窗口创建
    const auto window = utility::creat_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", true, mouse_callback, scroll_callback);

    // 配置全局的OpenGL状态
    glEnable(GL_DEPTH_TEST);

    // 创建和编译着色器zprogram
    Shader lightingShader("../6.multiple_lights.vs", "../6.multiple_lights.fs");
    Shader lightCubeShader("../6.light_cube.vs", "../6.light_cube.fs");
    Shader modelShader{"../1.model_loading.vs", "../1.model_loading.fs"};

    model trunk{"../resources/models/trunk.obj"};

    // 首先配置立方体的VAO和VBO
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 其次,配置光源的VAO， VBO保持不变; 光源也是顶点组成的立方体。
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 更新了灯的位置属性的步长，以反映更新的缓冲区数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    // 加载纹理
    unsigned int diffuseMap = loadTexture("../resources/textures/container2.png");
    unsigned int specularMap = loadTexture("../resources/textures/container2_specular.png");

    // 着色器配置
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);


    // 渲染
    while (!glfwWindowShouldClose(window)) {
        // 每帧时间逻辑
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入
        processInput(window);

        // 渲染
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 设置uniforms/drawing对象之前，激活着色器
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        modelShader.use();

        // 平行光
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.1f);
        lightingShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7);
        lightingShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
        // 点光源1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y,
                               pointLightPositions[0].z);
        lightingShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1,
                               pointLightColors[0].z * 0.1);
        lightingShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y,
                               pointLightColors[0].z);
        lightingShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y,
                               pointLightColors[0].z);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // 点光源2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y,
                               pointLightPositions[1].z);
        lightingShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1,
                               pointLightColors[1].z * 0.1);
        lightingShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y,
                               pointLightColors[1].z);
        lightingShader.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y,
                               pointLightColors[1].z);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // 点光源3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y,
                               pointLightPositions[2].z);
        lightingShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1,
                               pointLightColors[2].z * 0.1);
        lightingShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y,
                               pointLightColors[2].z);
        lightingShader.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y,
                               pointLightColors[2].z);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // 点光源4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y,
                               pointLightPositions[3].z);
        lightingShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1,
                               pointLightColors[3].z * 0.1);
        lightingShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y,
                               pointLightColors[3].z);
        lightingShader.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y,
                               pointLightColors[3].z);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
        // 聚光灯
        lightingShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
        lightingShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.009f);
        lightingShader.setFloat("spotLight.quadratic", 0.0032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.5f)));

        // 视图和投影 变换
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        glm::mat4 trunk_model = glm::mat4(1.0f);
        trunk_model = glm::translate(trunk_model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        trunk_model = glm::scale(trunk_model, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", trunk_model);
        trunk.draw(modelShader);

        // 全局变换
        glm::mat4x4 model;
        model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // 绑定漫反射贴图
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // 渲染对象
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 10; i++) {
            // 计算每个对象的模型矩阵，并在绘制之前将其传递给着色器
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 绘制光源对象
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        glBindVertexArray(lightCubeVAO);
        for (auto &pointLightPosition: pointLightPositions) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPosition);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: 交换缓冲区和检测输入输出（按键/释放、鼠标移动等）
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 一旦资源超出其用途，则取消分配：
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // 终止，清除所有先前分配的GLFW
    glfwTerminate();
    return 0;
}

