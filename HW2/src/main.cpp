#include "shader.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

//settings
const unsigned int _WIDTH = 800;
const unsigned int _HEIGHT = 600;
int main () {
    // glfw窗口初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // 创建glfw窗口
    GLFWwindow* window = glfwCreateWindow(_WIDTH, _HEIGHT, "HW2_王培钰", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        // 释放资源
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // 加载openGL的函数指针(glad)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
    
    // imgui窗口设置
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    // 设置颜色主题
    ImGui::StyleColorsLight();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // 编译着色器程序
    Shader ourShader("../file/shader.vs", "../file/shader.fs");
    
    //输入顶点
    float vertices[] = {
        //位置               // 颜色
        // 三角形
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // 左
        1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 右
        0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 上
        0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 第二个三角形
        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, //
        // 直线1
        -0.5f, 0.0f, 0.0f, 0.9f, 0.9f, 0.666f, // 左下
        -1.0f, 1.0f, 0.0f, 0.9f, 0.9f, 0.666f, // 左上
        // 直线2
        0.5f, 0.0f, 0.0f, 0.8f, 0.36f, 0.36f, // 右下
        1.0f, 1.0f, 0.0f, 0.8f, 0.36f, 0.36f, // 右上
        // 点1
        0.75f, 0.05f, 0.0f, 0.0f, 0.0f, 0.0f, // 上
        // 点2
        0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 中
        // 点3
        0.75f, -0.05f, 0.0f, 0.0f, 0.0f, 0.0f, // 下
        // 点1
        0.8f, 0.05f, 0.0f, 0.0f, 0.0f, 0.0f, // 上
        // 点2
        0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 中
        // 点3
        0.8f, -0.05f, 0.0f, 0.0f, 0.0f, 0.0f // 下
    };
    // 索引绘制
    unsigned int indices[] = {
        0, 1, 2, // 第一个三角形
        2, 3, 4 // 第二个三角形
    };
    unsigned int VBO, VAO, EBO;
    // 生成对象
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // 渲染循环
    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool default_color = true;
    bool lines = false;
    bool points = false;
    while (!glfwWindowShouldClose(window))
    {
        // 输入
        processInput(window);
        // 启动ImGui框架
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            //static float f = 0.0f;
            ImGui::Begin("HW2");
            ImGui::SetWindowSize(ImVec2(400, 180));
            ImGui::Text("change color");
            //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            // 将更改的颜色存入color中
            ImGui::ColorEdit3("choose one color", (float *)&color, 1);
            // 勾选框，选中则使三角形保持三色渐变色，取消勾选则课进行全局颜色设置
            ImGui::Checkbox("default color",  &default_color);
            ImGui::Checkbox("draw lines", &lines);
            ImGui::Checkbox("draw points", &points);
            if (lines)
            {
                vertices[33] = 0.9f; vertices[34] = 0.9f; vertices[35] = 0.666f;
                vertices[39] = 0.9f; vertices[40] = 0.9f; vertices[41] = 0.666f;
                vertices[45] = 0.8f; vertices[46] = 0.36f; vertices[47] = 0.36f;
                vertices[51] = 0.8f; vertices[52] = 0.36f; vertices[53] = 0.36f;
            }
            else {
                vertices[33] = 0.2f; vertices[34] = 0.3f; vertices[35] = 0.3f;
                vertices[39] = 0.2f; vertices[40] = 0.3f; vertices[41] = 0.3f;
                vertices[45] = 0.2f; vertices[46] = 0.3f; vertices[47] = 0.3f;
                vertices[51] = 0.2f; vertices[52] = 0.3f; vertices[53] = 0.3f;
            }
            if (points)
            {
                vertices[57] = 0.0f; vertices[58] = 1.0f; vertices[59] = 0.0f;
                vertices[63] = 0.0f; vertices[64] = 1.0f; vertices[65] = 0.0f;
                vertices[69] = 0.0f; vertices[70] = 1.0f; vertices[71] = 0.0f;
                vertices[75] = 0.0f; vertices[76] = 1.0f; vertices[77] = 0.0f;
                vertices[81] = 0.0f; vertices[82] = 1.0f; vertices[83] = 0.0f;
                vertices[87] = 0.0f; vertices[88] = 1.0f; vertices[89] = 0.0f;
            }
            else
            {
                vertices[57] = 0.2f; vertices[58] = 0.3f; vertices[59] = 0.3f;
                vertices[63] = 0.2f; vertices[64] = 0.3f; vertices[65] = 0.3f;
                vertices[69] = 0.2f; vertices[70] = 0.3f; vertices[71] = 0.3f;
                vertices[75] = 0.2f; vertices[76] = 0.3f; vertices[77] = 0.3f;
                vertices[81] = 0.2f; vertices[82] = 0.3f; vertices[83] = 0.3f;
                vertices[87] = 0.2f; vertices[88] = 0.3f; vertices[89] = 0.3f;
            }
            if (default_color)
            {
                vertices[3] = 0.0f; vertices[4] = 1.0f; vertices[5] = 0.0f;
                vertices[9] = 0.0f; vertices[10] = 0.0f; vertices[11] = 1.0f;
                vertices[15] = 1.0f; vertices[16] = 0.0f; vertices[17] = 0.0f;
            }
            else
            {
                for (int i = 0; i < 3; i++) {
                    vertices[i * 6 + 3] = color.x;
                    vertices[i * 6 + 4] = color.y;
                    vertices[i * 6 + 5] = color.z;
                }
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // 渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //渲染三角形
        ourShader.use();
        // 绑定顶点缓冲对象
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // 将顶点数据复制到当前绑定的缓冲中
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // 配置顶点属性
        // 位置属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 颜色属性
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_LINES, 3, 4);
        glDrawArrays(GL_POINTS, 7, 6);
        // 渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // 交换颜色双缓冲区并检查是否存在IO事件并触发相应函数
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // 删除分配的对象资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // 清除GUI资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // 删除所有之前分配的GLFW资源
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
