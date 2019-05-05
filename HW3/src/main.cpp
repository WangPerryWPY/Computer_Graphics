//
//  main.cpp
//  Bresenham
//
//  Created by peiyu wang on 2019/3/20.
//  Copyright © 2019 peiyu wang. All rights reserved.
//

#include "shader.h"
#include <cmath>
#include <vector>
#include <algorithm>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// 定义坐标结构体
struct point {
    int x;
    int y;
    point(int x0, int y0):x(x0), y(y0) {}
};

// 异或交换两个变量的值
inline void swap(int *a, int *b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

//p0是圆心
inline void circle_8_points(vector<point> &points, point p0, point p)
{
    points.push_back(point(p0.x + p.x, p0.y + p.y));
    points.push_back(point(p0.x - p.x, p0.y + p.y));
    points.push_back(point(p0.x + p.x, p0.y - p.y));
    points.push_back(point(p0.x - p.x, p0.y - p.y));
    points.push_back(point(p0.x + p.y, p0.y + p.x));
    points.push_back(point(p0.x - p.y, p0.y + p.x));
    points.push_back(point(p0.x + p.y, p0.y - p.x));
    points.push_back(point(p0.x - p.y, p0.y - p.x));
}

vector<point> Bresenham_Line (point p1, point p2)
{
    vector<point> temp;
    //temp.push_back(p1);
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    // 弦值大于45度, 进行坐标变换
    bool direction = 1;
    if (dx < dy)
    {
        direction = 0;
        swap(&p1.x, &p1.y);
        swap(&p2.x, &p2.y);
        swap(&dx, &dy);
    }
    int ix = (p2.x - p1.x) > 0 ? 1 : -1;
    int iy = (p2.y - p1.y) > 0 ? 1 : -1;
    int d = dy * 2 - dx;
    int x = p1.x;
    int y = p1.y;
    if (direction)
    {
        while (x != p2.x)
        {
            if (d < 0)
            {
                d += dy * 2;
            }
            else
            {
                d += (dy - dx) * 2;
                y+=iy;
            }
            temp.push_back(point(x, y));
            x+=ix;
        }
    }
    else
    {
        while (x != p2.x)
        {
            if (d < 0)
            {
                d += dy * 2;
            }
            else
            {
                d += (dy - dx) * 2;
                y+=iy;
            }
            temp.push_back(point(y, x));
            x+=ix;
        }
    }
    //temp.push_back(p2);
    return temp;
}

// 根据三个点画出三条直线并获取这三条直线组成的三角形的x所有的点
vector<point> Draw_Triangle(point p1, point p2, point p3)
{
    vector<point> vec, vec1, vec2, vec3;
    vec1 = Bresenham_Line(p1, p2);
    vec2 = Bresenham_Line(p1, p3);
    vec3 = Bresenham_Line(p2, p3);
    vec.insert(vec.end(), vec1.begin(), vec1.end());
    vec1.clear();
    vec.insert(vec.end(), vec2.begin(), vec2.end());
    vec2.clear();
    vec.insert(vec.end(), vec3.begin(), vec3.end());
    vec3.clear();
    vec.push_back(p1);
    vec.push_back(p2);
    vec.push_back(p3);
    return vec;
}

// p0为圆心，r是半径，fill代表是否进行填充
vector<point> Draw_Circle(point p0, int r, bool fill)
{
    vector<point> points;
    int x = 0, y = r, yi, d;
    d = 3 - 2 * r;
    if (fill)
    {
        // 填充的话画实心圆
        while (x <= y)
        {
            for (yi = x; yi <= y; yi++)
            {
                circle_8_points(points, p0, point(x, yi));
            }
            if (d < 0)
            {
                d = d + 4 * x + 6;
            }
            else
            {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }
    }
    // 如果不填充
    else
    {
        while (x <= y)
        {
            circle_8_points(points, p0, point(x, y));
            if (d < 0)
            {
                d = d + 4 * x + 6;
            }
            else
            {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }
    }
    return points;
}

// 三角形的光栅化(扫描法)
vector<point> Rasterize(point p1, point p2, point p3)
{
    vector<point> points;
    float k1 = (float)(p3.y - p2.y) / (float)(p3.x - p2.x);
    float k2 = (float)(p1.y - p3.y) / (float)(p1.x - p3.x);
    float k3 = (float)(p2.y - p1.y) / (float)(p2.x - p1.x);
    float b1 = (float)p2.y - k1 * p2.x;
    float b2 = (float)p3.y - k2 * p3.x;
    float b3 = (float)p1.y - k3 * p1.x;
    int min_x = min(p1.x, min(p2.x, p3.x));
    int max_x = max(p1.x, max(p2.x, p3.x));
    int min_y = min(p1.y, min(p2.y, p3.y));
    int max_y = max(p1.y, max(p2.y, p3.y));
    for (int i = min_x; i <= max_x; i++)
    {
        for (int j = min_y; j <= max_y; j++)
        {
            // 判断三角形内的点和直线对的角点在直线的同侧
            float t1 = (i * k1 + b1 - j) * (p1.x * k1 + b1 - p1.y);
            float t2 = (i * k2 + b2 - j) * (p2.x * k2 + b2 - p2.y);
            float t3 = (i * k3 + b3 - j) * (p3.x * k3 + b3 - p3.y);
            if (t1 >= 0 && t2 >= 0 && t3 >= 0)
            {
                points.push_back(point(i, j));
            }
        }
    }
    return points;
}

float* getVec(vector<point> points)
{
    int num = points.size();
    int N = num * 6;
    float *vertices = new float [N];
    for (int i = 0; i < num; i++) {
        // 位置, 归一化操作
        vertices[i * 6 + 0] = (float)points[i].x / ((float)SCR_WIDTH / 2);
        vertices[i * 6 + 1] = (float)points[i].y / ((float)SCR_HEIGHT / 2);
        vertices[i * 6 + 2] = 0.0f;
        // 颜色
        vertices[i * 6 + 3] = 1.0f;
        vertices[i * 6 + 4] = 1.0f;
        vertices[i * 6 + 5] = 0.0f;
    }
    return vertices;
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW3_WPY", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
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
    Shader ourShader("shader.vs", "shader.fs");
    
    unsigned int VBO, VAO;
    // 生成对象
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // 输入顶点
    bool circle = false;
    bool active = false;
    bool fill = false, fill1 = false;
    int r = 100;
    ImVec4 change_rgb = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        
        // 启动ImGui的框架
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("HW3_WPY", &active, ImGuiWindowFlags_MenuBar);
            ImGui::SetWindowSize(ImVec2(400, 180));
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    if (ImGui::MenuItem("Triangle")) {
                        circle = false;
                    }
                    if (ImGui::MenuItem("Circle")) {
                        circle = true;
                    }
                    if (ImGui::MenuItem("Close")) {
                        active = false;
                        glfwSetWindowShouldClose(window, true);//关闭窗口
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            if (circle)
            {
                ImGui::Text("Wether fill the Circle or not");
                ImGui::Checkbox("Fill", &fill);
                ImGui::SliderInt("radius", &r, 0, 300);
                ImGui::ColorEdit3("change color", (float*)&change_rgb); // 设置圆的填充色
            }
            else
            {
                ImGui::Text("Wether fill the Triangle or not");
                ImGui::Checkbox("Fill", &fill1);
                ImGui::ColorEdit3("change color", (float*)&change_rgb); // 设置三角形颜色
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        vector<point> points;
        if (circle)
        {
            points = Draw_Circle(point(0, 0), r, fill);
        }
        else
        {
            if (fill1)
            {
                points = Rasterize(point(0, 200), point(200, -100), point(-200, -100));
            }
            else
            {
                points = Draw_Triangle(point(0, 200), point(200, -100), point(-200, -100));
            }
        }
        float *vertices = getVec(points);
        for (int k = 0; k < points.size(); k++)
        {
            vertices[6*k+3] = change_rgb.x;
            vertices[6*k+4] = change_rgb.y;
            vertices[6*k+5] = change_rgb.z;
        }
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ourShader.use();
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (points.size() * 6) * sizeof(float), vertices, GL_STREAM_DRAW);
        // 位置
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 颜色
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        //Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(VAO);
        delete vertices;
        
        glDrawArrays(GL_POINTS, 0, points.size());
        // 渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
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
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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
