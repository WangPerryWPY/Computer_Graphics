//
//  main.cpp
//  Texture
//
//  Created by peiyu wang on 2019/3/12.
//  Copyright © 2019 peiyu wang. All rights reserved.
//
#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 15.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Camera", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 将我们窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // 开启深度测试，只要放在画图前就可以
    glEnable(GL_DEPTH_TEST);
    // imgui窗口设置
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    // 设置颜色主题
    ImGui::StyleColorsLight();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    // build and compile our shader zprogram
    // ------------------------------------
    Shader object("shader.vs", "shader.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // 顶点位置           // 颜色坐标
        -2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 0.0f,
        2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 0.0f,
        2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 0.0f,
        2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 0.0f,
        -2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 0.0f,
        -2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 0.0f,
        
        -2.0f, -2.0f,  2.0f, 0.0f, 1.0f, 0.0f,
        2.0f, -2.0f,  2.0f, 0.0f, 1.0f, 0.0f,
        2.0f,  2.0f,  2.0f, 0.0f, 1.0f, 0.0f,
        2.0f,  2.0f,  2.0f, 0.0f, 1.0f, 0.0f,
        -2.0f,  2.0f,  2.0f, 0.0f, 1.0f, 0.0f,
        -2.0f, -2.0f,  2.0f, 0.0f, 1.0f, 0.0f,
        
        -2.0f,  2.0f,  2.0f, 0.0f, 0.0f, 1.0f,
        -2.0f,  2.0f, -2.0f, 0.0f, 0.0f, 1.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, 0.0f, 1.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, 0.0f, 1.0f,
        -2.0f, -2.0f,  2.0f, 0.0f, 0.0f, 1.0f,
        -2.0f,  2.0f,  2.0f, 0.0f, 0.0f, 1.0f,
        
        2.0f,  2.0f,  2.0f, 1.0f, 1.0f, 0.0f,
        2.0f,  2.0f, -2.0f, 1.0f, 1.0f, 0.0f,
        2.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.0f,
        2.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.0f,
        2.0f, -2.0f,  2.0f, 1.0f, 1.0f, 0.0f,
        2.0f,  2.0f,  2.0f, 1.0f, 1.0f, 0.0f,
        
        -2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 1.0f,
        2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 1.0f,
        2.0f, -2.0f,  2.0f, 0.0f, 1.0f, 1.0f,
        2.0f, -2.0f,  2.0f, 0.0f, 1.0f, 1.0f,
        -2.0f, -2.0f,  2.0f, 0.0f, 1.0f, 1.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 1.0f,
        
        -2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 1.0f,
        2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 1.0f,
        2.0f,  2.0f,  2.0f, 1.0f, 0.0f, 1.0f,
        2.0f,  2.0f,  2.0f, 1.0f, 0.0f, 1.0f,
        -2.0f,  2.0f,  2.0f, 1.0f, 0.0f, 1.0f,
        -2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 1.0f
        
    };
    // 绑定VAO，VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 告知着色器相应属性的相对地址位置(Location = 0， Location = 1)
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // imgui的menu
    bool tool_active = true;
    bool start = false;
    float radius = 15.0f;
    bool orth = false;
    bool pers = false;
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    object.use();
    object.setMat4("projection", projection);
    while (!glfwWindowShouldClose(window))
    {
        // 计算时间差
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);
        // ImGui框架
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("HW4_WPY", &tool_active, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    if (ImGui::MenuItem("Start")) {
                        start = true;
                    }
                    if (ImGui::MenuItem("Close")) {
                        tool_active = false;
                        glfwSetWindowShouldClose(window, true);//关闭窗口
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            if (start)
            {
                ImGui::Text("1.Projection");
                ImGui::Text("Select one of the following projections");
                if (ImGui::Button("Orthogonal projection"))
                {
                    orth = true;
                    pers = false;
                }
                if (ImGui::Button("Perspective projection"))
                {
                    orth = false;
                    pers = true;
                }
                ImGui::Text("2.Camera");
                ImGui::SliderFloat("Radius", &radius, 5.0f, 50.0f);
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindVertexArray(VAO);
        if (start)
        {
            object.use(); // don't forget to activate/use the shader before setting uniforms!
            // 变换组合
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            object.setMat4("model", model);
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);
            view = camera.GetViewMatrix();
            if (orth)
                projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
           else
                projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            object.use(); // don't forget to activate/use the shader before setting uniforms!
            object.setMat4("view", view);
            object.setMat4("projection", projection);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
