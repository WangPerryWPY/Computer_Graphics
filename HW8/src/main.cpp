//
//  main.cpp
//  BezierCurve
//
//  Created by peiyu wang on 2019/5/29.
//  Copyright © 2019 peiyu wang. All rights reserved.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

using namespace std;
const unsigned int SCR_WIDTH = 750;
const unsigned int SCR_HEIGHT = 750;

glm::vec2 Q(double t);
int combine(int c, int k);
int factorial(int n);

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//global param
vector<glm::vec2> p;
bool draw = false;
double running_t = 0.0f;


int main() {
    //glfw: initialize and configure
    //------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    //glfw window creation
    //--------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW8_WPY", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    //glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    
    //shader
    Shader curveShader("bezier.vs", "bezier.fs");
    
    //VAO,VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    curveShader.use();
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        curveShader.setVec3("curveColor", glm::vec3(1, 1, 1));
        curveShader.use();
        //把直线画起来
        glPointSize(10.0f);
        vector<float> controlPoints;
        for (int i = 0; i < p.size(); ++i) {
            vector<float> renderP = { p[i].x, p[i].y, 0.0f };
            controlPoints.insert(controlPoints.end(), renderP.begin(), renderP.end());
            //一条一条画
            if (i > 0) {
                float* ptr = controlPoints.data() + (i - 1) * 3;
                glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), ptr, GL_STATIC_DRAW);
                glDrawArrays(GL_LINES, 0, 2);
            }
        }
        //画出控制点
        float *ptr = controlPoints.data();
        glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(float), ptr, GL_STATIC_DRAW);
        glDrawArrays(GL_POINTS, 0, p.size());
        
        //bezier 曲线
        if (p.size() > 2)
        {
            vector<float> bezierPoints;
            int count = 0;
            for (double i = 0.0f; i < 1.0f; i+=0.0005)
            {
                glm::vec2 Bpoint(Q(i));
                vector<float> point{
                    Bpoint.x, Bpoint.y, 0.0f
                };
                bezierPoints.insert(bezierPoints.end(), point.begin(), point.end());
                if (count > 0)
                {
                    float *ptr = bezierPoints.data() + (count - 1) * 3;
                    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), ptr, GL_STATIC_DRAW);
                    glDrawArrays(GL_LINES, 0, 2);
                }
                count++;
            }
            
            //进行动画
            if (draw)
            {
                vector<glm::vec2> runningPoints(p);
                while (runningPoints.size() > 1)
                {
                    vector<glm::vec2> nextLevel;
                    for (int i = 0; i < runningPoints.size()-1; i++)
                    {
                        float presPoint[] = {
                            runningPoints[i].x, runningPoints[i].y, 0.0f,
                            runningPoints[i + 1].x, runningPoints[i + 1].y, 0.0f
                        };
                        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), presPoint, GL_STATIC_DRAW);
                        glDrawArrays(GL_LINES, 0, 2);
                        glDrawArrays(GL_POINTS, 0, 2);
                        glm::vec2 nextPoint;
                        nextPoint.x = (1-running_t)*runningPoints[i].x + (running_t) * runningPoints[i + 1].x;
                        nextPoint.y = (1-running_t)*runningPoints[i].y + (running_t) * runningPoints[i + 1].y;
                        nextLevel.push_back(nextPoint);
                    }
                    runningPoints = nextLevel;
                }
                running_t += 0.0006f;
                if (running_t >= 1.0f)
                {
                    draw = false;
                }
            }
            
        }
        else {
            draw = false;
        }
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        draw = true;
        running_t = 0.0f;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (draw)
    {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        p.push_back(glm::vec2((float(xpos) / float(SCR_WIDTH) * 2.0f) - 1, -((float(ypos) / float(SCR_HEIGHT) * 2.0f) - 1)));
    }
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        
        p.pop_back();
    }
}

//求Q(t)
glm::vec2 Q(double t) {
    int n = p.size() - 1;
    double x = 0, y = 0;
    double p1 = pow((1 - t), n), p2 = 1;
    for (int i = 0; i <= n; i++) {
        x += combine(n, i)*p1*p2*p[i].x;
        y += combine(n, i)*p1*p2*p[i].y;
        p1 /= (1 - t);
        p2 *= t;
    }
    return glm::vec2(x, y);
}

int factorial(int n) {
    if (n == 1 || n==0) return 1;
    return n * factorial(n - 1);
}

//求组合数
int combine(int n, int k) {
    return factorial(n) / (factorial(k)*(factorial(n - k)));
}



