//
//  main.cpp
//  Texture
//
//  Created by peiyu wang on 2019/5/4.
//  Copyright © 2019 peiyu wang. All rights reserved.
//
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include <iostream>
#include <stdio.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.5f, 0.5f, 0.6f);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lights and Shading", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // configure global opengl state
    // -----------------------------
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
    Shader phongObject("phong.vs", "phong.fs");
    Shader gouraudObject("gouraud.vs", "gouraud.fs");
    Shader lightShader("light.vs", "light.fs");
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.2f, -0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        0.2f, -0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        0.2f,  0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        0.2f,  0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        -0.2f,  0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        
        -0.2f, -0.2f,  0.2f,  0.0f,  0.0f, 1.0f,
        0.2f, -0.2f,  0.2f,  0.0f,  0.0f, 1.0f,
        0.2f,  0.2f,  0.2f,  0.0f,  0.0f, 1.0f,
        0.2f,  0.2f,  0.2f,  0.0f,  0.0f, 1.0f,
        -0.2f,  0.2f,  0.2f,  0.0f,  0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.0f,  0.0f, 1.0f,
        
        -0.2f,  0.2f,  0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f,  0.2f, -0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f, -0.2f, -0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f, -0.2f, -0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f, -0.2f,  0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f,  0.2f,  0.2f, -1.0f,  0.0f,  0.0f,
        
        0.2f,  0.2f,  0.2f,  1.0f,  0.0f,  0.0f,
        0.2f,  0.2f, -0.2f,  1.0f,  0.0f,  0.0f,
        0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  0.0f,
        0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  0.0f,
        0.2f, -0.2f,  0.2f,  1.0f,  0.0f,  0.0f,
        0.2f,  0.2f,  0.2f,  1.0f,  0.0f,  0.0f,
        
        -0.2f, -0.2f, -0.2f,  0.0f, -1.0f,  0.0f,
        0.2f, -0.2f, -0.2f,  0.0f, -1.0f,  0.0f,
        0.2f, -0.2f,  0.2f,  0.0f, -1.0f,  0.0f,
        0.2f, -0.2f,  0.2f,  0.0f, -1.0f,  0.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, -1.0f,  0.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, -1.0f,  0.0f,
        
        -0.2f,  0.2f, -0.2f,  0.0f,  1.0f,  0.0f,
        0.2f,  0.2f, -0.2f,  0.0f,  1.0f,  0.0f,
        0.2f,  0.2f,  0.2f,  0.0f,  1.0f,  0.0f,
        0.2f,  0.2f,  0.2f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.2f,  0.2f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.2f, -0.2f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    
    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    
    // render loop
    // -----------
    bool gouraud = false, phong = false, tool_active = false;
    float ambient = 0.1f, diffuse = 1.0f, specular = 0.5f;
    int reflectance = 32;
    float ambient1 = 0.1f, diffuse1 = 1.0f, specular1 = 0.5f;
    int reflectance1 = 32;
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        // -----
        processInput(window);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("HW6_WPY", &tool_active, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    if (ImGui::MenuItem("Phong Shading")) {
                        phong = true;
                        gouraud = false;
                    }
                    if (ImGui::MenuItem("Gouraud Shading")) {
                        gouraud = true;
                        phong = false;
                    }
                    if (ImGui::MenuItem("Close")) {
                        tool_active = false;
                        glfwSetWindowShouldClose(window, true);//关闭窗口
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            if (phong)
            {
                ImGui::Text("1.Phong Shading");
                ImGui::Text("change the ambient strength");
                ImGui::SliderFloat("ambient strength", &ambient, 0.0f, 1.0f);
                ImGui::Text("change the diffuse strength");
                ImGui::SliderFloat("diffuse strength", &diffuse, 0.0f, 5.0f);
                ImGui::Text("change the specular strength");
                ImGui::SliderFloat("specular strength", &specular, 0.0f, 10.0f);
                ImGui::Text("change the reflectance");
                ImGui::SliderInt("reflectance", &reflectance, 2, 256);
            }
            if (gouraud)
            {
                ImGui::Text("2.Gouraud Shading");
                ImGui::Text("change the ambient strength");
                ImGui::SliderFloat("ambient strength", &ambient1, 0.0f, 1.0f);
                ImGui::Text("change the diffuse strength");
                ImGui::SliderFloat("diffuse strength", &diffuse1, 0.0f, 5.0f);
                ImGui::Text("change the specular strength");
                ImGui::SliderFloat("specular strength", &specular1, 0.0f, 10.0f);
                ImGui::Text("change the reflectance");
                ImGui::SliderInt("reflectance", &reflectance1, 2, 256);
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        float radius = 1.0f;
        float X = sin(glfwGetTime()) * radius;
        float Z = cos(glfwGetTime()) * radius;
        lightPos = glm::vec3(X, 0.0f, Z);
        if (phong)
        {
            // be sure to activate shader when setting uniforms/drawing objects
            phongObject.use();
            glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            phongObject.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
            phongObject.setVec3("lightColor",  lightColor);
            phongObject.setVec3("lightPos", lightPos);
            phongObject.setVec3("viewPos", camera.Position);
            phongObject.setMat4("projection", projection);
            phongObject.setMat4("view", view);
            phongObject.setFloat("ambientStrength", ambient);
            phongObject.setFloat("diffuseStrength", diffuse);
            phongObject.setFloat("specularStrength", specular);
            phongObject.setInt("reflectance", reflectance);
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 1.0f));
            phongObject.setMat4("model", model);
            // render the cube
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            // also draw the lamp object
            lightShader.use();
            lightShader.setVec3("Color", lightColor);
            lightShader.setMat4("projection", projection);
            lightShader.setMat4("view", view);
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            lightShader.setMat4("model", model);
            
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            
        }
        if (gouraud)
        {
            glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            // be sure to activate shader when setting uniforms/drawing objects
            gouraudObject.use();
            gouraudObject.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
            gouraudObject.setVec3("lightColor",  lightColor);
            gouraudObject.setVec3("lightPos", lightPos);
            gouraudObject.setVec3("viewPos", camera.Position);
            gouraudObject.setMat4("projection", projection);
            gouraudObject.setMat4("view", view);
            gouraudObject.setFloat("ambientStrength", ambient1);
            gouraudObject.setFloat("diffuseStrength", diffuse1);
            gouraudObject.setFloat("specularStrength", specular1);
            gouraudObject.setInt("reflectance", reflectance1);
            // world transformation
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 1.0f));
            gouraudObject.setMat4("model", model);
            // render the cube
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            
            // also draw the lamp object
            lightShader.use();
            lightShader.setVec3("Color", lightColor);
            lightShader.setMat4("projection", projection);
            lightShader.setMat4("view", view);
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            lightShader.setMat4("model", model);
            
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            
        }
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
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
