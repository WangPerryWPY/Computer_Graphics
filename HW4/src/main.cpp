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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int LoadTexture(const char *pic, bool Alpha);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 加载纹理
unsigned int LoadTexture(const char *pic, bool Alpha)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters 线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(pic, &width, &height, &nrChannels, 0);
    if (data)
    {
        if (Alpha)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TextureOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 将我们窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    Shader object1("shader.vs", "shader.fs");
    Shader object2("shader1.vs", "shader1.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // 顶点位置           // 纹理坐标
        -2.0f, -2.0f, -2.0f, 0.0f, 0.0f,
         2.0f, -2.0f, -2.0f, 1.0f, 0.0f,
         2.0f,  2.0f, -2.0f, 1.0f, 1.0f,
         2.0f,  2.0f, -2.0f, 1.0f, 1.0f,
        -2.0f,  2.0f, -2.0f, 0.0f, 1.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, 0.0f,
        
        -2.0f, -2.0f,  2.0f, 0.0f, 0.0f,
         2.0f, -2.0f,  2.0f, 1.0f, 0.0f,
         2.0f,  2.0f,  2.0f, 1.0f, 1.0f,
         2.0f,  2.0f,  2.0f, 1.0f, 1.0f,
        -2.0f,  2.0f,  2.0f, 0.0f, 1.0f,
        -2.0f, -2.0f,  2.0f, 0.0f, 0.0f,
        
        -2.0f,  2.0f,  2.0f, 1.0f, 0.0f,
        -2.0f,  2.0f, -2.0f, 1.0f, 1.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, 1.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, 1.0f,
        -2.0f, -2.0f,  2.0f, 0.0f, 0.0f,
        -2.0f,  2.0f,  2.0f, 1.0f, 0.0f,
        
         2.0f,  2.0f,  2.0f, 1.0f, 0.0f,
         2.0f,  2.0f, -2.0f, 1.0f, 1.0f,
         2.0f, -2.0f, -2.0f, 0.0f, 1.0f,
         2.0f, -2.0f, -2.0f, 0.0f, 1.0f,
         2.0f, -2.0f,  2.0f, 0.0f, 0.0f,
         2.0f,  2.0f,  2.0f, 1.0f, 0.0f,
        
        -2.0f, -2.0f, -2.0f, 0.0f, 1.0f,
         2.0f, -2.0f, -2.0f, 1.0f, 1.0f,
         2.0f, -2.0f,  2.0f, 1.0f, 0.0f,
         2.0f, -2.0f,  2.0f, 1.0f, 0.0f,
        -2.0f, -2.0f,  2.0f, 0.0f, 0.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, 1.0f,
        
        -2.0f,  2.0f, -2.0f, 0.0f, 1.0f,
         2.0f,  2.0f, -2.0f, 1.0f, 1.0f,
         2.0f,  2.0f,  2.0f, 1.0f, 0.0f,
         2.0f,  2.0f,  2.0f, 1.0f, 0.0f,
        -2.0f,  2.0f,  2.0f, 0.0f, 0.0f,
        -2.0f,  2.0f, -2.0f, 0.0f, 1.0f
        
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // load and create a texture
    // -------------------------
    unsigned int texture1, texture2, texture3, texture4;
    texture1 = LoadTexture("timg.jpg", 0);
    texture2 = LoadTexture("rocket.png", 1);
    texture3 = LoadTexture("地球.jpg", 0);
    texture4 = LoadTexture("太阳.png", 1);
    
    // 激活并绑定纹理单元
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture4);
    
    // 设置view和透视矩阵，因为默认摄像机和透视不变化所以开始就可以设置好
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    object1.use(); // don't forget to activate/use the shader before setting uniforms!
    object1.setMat4("view", view);
    object1.setMat4("projection", projection);
    object2.use();
    object2.setMat4("view", view);
    object2.setMat4("projection", projection);
    
    // imgui的Slider变化量
    float _t_x = 0, _t_y = 0, _s = 1;
    float rad_x = 0.0f, rad_y = 0.0f, rad_z = 0.0f;
    bool r_x = false, r_y = false, r_z = false;
    // imgui的menu
    bool tool_active = true;
    bool basic = false;
    bool bonus = false;
    while (!glfwWindowShouldClose(window))
    {
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
                    if (ImGui::MenuItem("Basic")) {
                        basic = true;
                        bonus = false;
                    }
                    if (ImGui::MenuItem("Bonus")) {
                        basic = false;
                        bonus = true;
                    }
                    if (ImGui::MenuItem("Close")) {
                        tool_active = false;
                        glfwSetWindowShouldClose(window, true);//关闭窗口
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            if (basic)
            {
                ImGui::Text("(1).Translation");
                ImGui::SliderFloat("translation_X", &_t_x, -1, 1);
                ImGui::SliderFloat("translation_Y", &_t_y, -1, 1);
                //ImGui::SliderFloat("translation_Z", &_t_z, -1, 1);
                ImGui::Text("(2).Rotation");
                //ImGui::SliderFloat("Rotation", &_r, -1, 1);
                if (ImGui::Button("rotate by x"))
                {
                    r_x = true;
                    rad_x += glm::radians(30.0f);
                }
                if (ImGui::Button("rotata by y"))
                {
                    r_y = true;
                    rad_y += glm::radians(30.0f);
                }
                if (ImGui::Button("rotate by z"))
                {
                    r_z = true;
                    rad_z += glm::radians(30.0f);
                }
                ImGui::Text("(3).Scale");
                ImGui::SliderFloat("Scale", &_s, 0, 2);
            }
            if (bonus)
            {
                ImGui::Text("The sun goes round the earth");
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindVertexArray(VAO);
        if (basic)
        {
            // 将纹理绑定到对应的纹理单元
            object1.use(); // don't forget to activate/use the shader before setting uniforms!
            object1.setInt("texture1", 0);
            object1.setInt("texture2", 1);
            object1.use();
            // 变换组合
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            // 先缩放，后旋转，最后位移，矩阵乘法顺序相反
            // 沿水平方向来回移动
            model = glm::translate(model, glm::vec3(_t_x * 5, _t_y * 5, 0.0));
            // 沿着x = z轴持续旋转
            if (r_x)
            {
                model = glm::rotate(model, rad_x , glm::vec3(1.0f, 0.0f, 0.0f));
                
            }
            if (r_y)
            {
                model = glm::rotate(model, rad_y,  glm::vec3(0.0f, 1.0f, 0.0f));
                
            }
            if (r_z)
            {
                model = glm::rotate(model, rad_z, glm::vec3(0.0f, 0.0f, 1.0f));
            }
            // 持续放大缩小
            //float scale_size = 2 * abs(cos((float)glfwGetTime()));
            model = glm::scale(model, glm::vec3(_s, _s, _s));
            object1.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        if (bonus)
        {
            object1.use(); // don't forget to activate/use the shader before setting uniforms!
            object1.setInt("texture1", 0);
            object1.setInt("texture2", 3);
            object2.use();
            object2.setInt("Texture", 2);
            object1.use();
            // 变换组合
            // 太阳
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(0.5f, 0.8f, 0.0f));
            object1.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            object2.use();
            glm::mat4 model1 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            // 公转
            float X = sin(glfwGetTime()) * 5;
            float Z = cos(glfwGetTime()) * 5;
            model1 = glm::translate(model1, glm::vec3(X, 0.0f, Z));
           // 自转
            model1 = glm::rotate(model1, (float)glfwGetTime() * glm::radians(45.0f), glm::vec3(0.5f, 1.0f, 0.3f));
            // 地球比太阳小
            model1 = glm::scale(model1, glm::vec3(0.5f, 0.5f, 0.5f));
            object2.setMat4("model", model1);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
