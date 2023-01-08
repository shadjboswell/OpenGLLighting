#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float mixAmount = 0.5f;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 1250, lastY = 1000;
bool firstMouse = true;
float fov = 45.0f;
float lightVelocity = 2.5f;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

glm::mat4 getLookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
    glm::vec3 direction = glm::normalize(position - target);
    glm::vec3 right = glm::normalize(glm::cross(glm::normalize(up), direction));
    up = glm::cross(direction, right);
    glm::mat4 RUD(1.0f);
    glm::vec4 cameraDirection = glm::vec4(direction, 0);
    glm::vec4 cameraUp = glm::vec4(up, 0);
    glm::vec4 cameraRight = glm::vec4(right, 0);
    RUD[0][0] = cameraRight.x;
    RUD[1][0] = cameraRight.y;
    RUD[2][0] = cameraRight.z;
    RUD[0][1] = cameraUp.x;
    RUD[1][1] = cameraUp.y;
    RUD[2][1] = cameraUp.z;
    RUD[0][2] = cameraDirection.x;
    RUD[1][2] = cameraDirection.y;
    RUD[2][2] = cameraDirection.z;

    glm::transpose(RUD);

    glm::mat4 positionMat(1.0f);
    positionMat[3][0] = -position.x;
    positionMat[3][1] = -position.y;
    positionMat[3][2] = -position.z;

    return RUD * positionMat;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixAmount -= 0.001f;
        if (mixAmount < 0.0f)
            mixAmount = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixAmount += 0.001f;
        if (mixAmount > 1.0f)
            mixAmount = 1.0f;
    }

    float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        lightPos.z += lightVelocity * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        lightPos.z -= lightVelocity * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        lightPos.x -= lightVelocity * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        lightPos.x += lightVelocity * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        lightPos.y -= lightVelocity * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        lightPos.y += lightVelocity * deltaTime;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    float screenHeight = 2500.0f;
    float screenWidth = 2000.0f;

    GLFWwindow* window = glfwCreateWindow(screenHeight, screenWidth, "AS2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, screenHeight, screenWidth);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader shaderProgram("Dependencies/include/vertexShader.txt", "Dependencies/include/fragmentShader.txt");
    Shader lightShader("Dependencies/include/lightVertexShader.txt", "Dependencies/include/lightFragmentShader.txt");


    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
    };

    glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f)
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); //ubind VAO

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    unsigned int lightVBO;
    glGenBuffers(1, &lightVBO);

    unsigned int lightEBO;
    glGenBuffers(1, &lightEBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glm::vec3 objectColor = glm::vec3(0.5f, 0.68f, 0.95f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    float kS = 0.5;
    float kD = 1.0;

    std::cout << objectColor.x << std::endl;
    shaderProgram.use();
    shaderProgram.setUniformVec3("objectColor", objectColor.x, objectColor.y, objectColor.z);
    shaderProgram.setUniformVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
    shaderProgram.setFloat("kS", kS);
    shaderProgram.setFloat("kD", kD);


    glEnable(GL_DEPTH_TEST);

    const float radius = 10.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        /*lightPos.x = sin(glfwGetTime()) * 10.0f;
        lightPos.z = cos(glfwGetTime()) * 10.0f;*/

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Do everything with the shader
        shaderProgram.use(); // use new program
        shaderProgram.setUniformVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        shaderProgram.setUniformVec3("lightPosUni", lightPos.x, lightPos.y, lightPos.z);

        glm::mat4 model = glm::mat4(1.0f);
        //Set model matrix
        unsigned int modelLoc = glGetUniformLocation(shaderProgram.ID, "model"); //get uniform location in shader program
        modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glm::mat4 view;
        view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        unsigned int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 2500.0f / 2000.0f, 0.1f, 100.0f);
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 2; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * (i) * (float)glfwGetTime();
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        //Transform and draw light box
        lightShader.use();
        //Set model matrix
        model = glm::mat4(1.0f); //create 4X4 identity matrix
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

        //Set view matrix
        viewLoc = glGetUniformLocation(lightShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        //Set projection matrix
        projectionLoc = glGetUniformLocation(lightShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //set uniform values in shader program

        //Draw the light box
        glBindVertexArray(lightVAO); //bind the VAO
        glDrawArrays(GL_TRIANGLES, 0, 36); // draw the light box
        glBindVertexArray(0); //unbind the VAO

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}