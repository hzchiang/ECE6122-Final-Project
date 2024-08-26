/*
Author: Haozhe Jiang
Class: ECE6122 QSZ
Last Date Modified: 2023/12/5
Description: Solution to Final Project. This is main application file.
Rembember to replace the paths of the model file and texture file before running the program.
Reference: https://learnopengl.com/Model-Loading/Assimp
*/


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <thread>
#include <random>


// function declaration
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);
glm::vec3 calculateBallCenter(const vector<Vertex>& vertices);
float calculateBallRadius(const vector<Vertex>& vertices, glm::vec3 center);
void handleCollisionWithFloor(glm::vec3& position, glm::vec3& velocity, float radius);
void handleCollisionWithBoundary(glm::vec3& position, glm::vec3& velocity);
bool checkCollisionWithBall(const glm::vec3& pos1, float radius1, const glm::vec3& pos2, float radius2);
void handleCollisionWithBall(glm::vec3& pos1, glm::vec3& vel1, float radius1, glm::vec3& pos2, glm::vec3& vel2, float radius2);

// window settings
const unsigned int SCR_WIDTH = 800; // window width
const unsigned int SCR_HEIGHT = 600; // window height

// camera setting
Camera camera(glm::vec3(0.0f, 0.0f, 60.0f)); // The initial position of the camera in world coordinates

// timing settings
float deltaTime = 0.0f; // The time difference between the current frame and the previous frame
float lastFrame = 0.0f; // time of previous frame


// sphere settings
glm::vec3 ballPosition1 = glm::vec3(-5.0f, 0.0f, 0.0f); // Initial position of the first sphere
glm::vec3 ballPosition2 = glm::vec3(5.0f, 0.0f, 0.0f); // Initial position of the second sphere
glm::vec3 ballPosition3 = glm::vec3(0.0f, 0.0f, 5.0f); // Initial position of the third sphere
glm::vec3 ballPosition4 = glm::vec3(0.0f, 0.0f, -5.0f); // Initial position of the fourth sphere
glm::vec3 ballVelocity1 = glm::vec3(-5.0f, 3.0f, -3.5f); // The moving speed of the first sphere
glm::vec3 ballVelocity2 = glm::vec3(5.0f, 3.0f, 3.5f); // The moving speed of the second sphere
glm::vec3 ballVelocity3 = glm::vec3(-5.0f, 3.0f, 3.5f); // The moving speed of the third sphere
glm::vec3 ballVelocity4 = glm::vec3(5.0f, 3.0f, -3.5f); // The moving speed of the fourth sphere
float rotationAngle1 = 0.0f; // The rotation angle of the first sphere
float rotationAngle2 = 0.0f; // The rotation angle of the second sphere
float rotationAngle3 = 0.0f; // The rotation angle of the third sphere
float rotationAngle4 = 0.0f; // The rotation angle of the fourth sphere
float rotationSpeed = 45.0f; // The rotation speed of the sphere


// floor and space boundary settings
const float floorHeight = 0.0f; // floor height
const float boundaryRadius = 15.0f; // radius of hemispherical space boundary


// G key control settings
bool isMoving = false; // Flag variable that controls the movement of the sphere
float lastKeyPressTime = 0.0f; // The time when the G key was last pressed
float keyPressDelay = 0.2f; // Key delay time


// Main Function
int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FinalProject", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST); // enable depth testing and use z-buffer

    // build and compile shaders
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

    // load models
    // Remember to replace the following path with your own model file path
    // Model ourModel(FileSystem::getPath("resources/objects/sphere2/wooden_sphere.obj"));
    Model ourModel("C:/ECE6122FinalProjects/LearnOpenGL-master/LearnOpenGL-master/resources/objects/sphere2/pumpkin_sphere.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Get the vertex data of the first mesh of each model
    const vector<Vertex>& ballVertices = ourModel.meshes[0].vertices;

    // Calculate center and radius of sphere
    glm::vec3 originalBallCenter = calculateBallCenter(ballVertices); // local coordinates of sphere center
    float originalRadius = calculateBallRadius(ballVertices, originalBallCenter); // radius of sphere

    // Print the local coordinates of sphere center and the radius of the sphere
    // std::cout << "Original ball model - center: (" << originalBallCenter.x << ", " << originalBallCenter.y << ", " << originalBallCenter.z << "), radius: " << originalRadius << std::endl;


    // set up floor vertex data (and buffer(s)) and configure vertex attributes
    float floorVertices[] = 
    {
        // positions          // normals         // texture coords
        25.0f,  0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        -25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  0.0f,   0.0f,
        -25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  0.0f,   1.0f,

        25.0f,  0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        -25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  0.0f,   1.0f,
        25.0f,  0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  1.0f,  1.0f
    };

    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // texture coord attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);


    // load and create the floor texture
    // Remember to replace the following path with your own texture file path
    unsigned int floorTexture = loadTexture("C:/ECE6122FinalProjects/LearnOpenGL-master/LearnOpenGL-master/resources/textures/container.jpg");

    
    // set up cube vertex data (and buffer(s)) and configure vertex attributes
    float cubeVertices[] = 
    {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // Set the position of each cube
    glm::vec3 cubePositions[] = 
    {
        glm::vec3(0.0f,  15.0f, -25.0f),
        glm::vec3(-15.0f,  15.0f, -15.0f),
        glm::vec3(15.0f,  15.0f, 0.0f),
        glm::vec3(-15.0f,  15.0f, 15.0f),
        glm::vec3(15.0f,  15.0f, 25.0f)
    };


    // positions of the point lights
    glm::vec3 pointLightPositions[] = 
    {
        glm::vec3(0.0f,  0.0f, 0.0f),
        glm::vec3(0.0f,  0.0f, 0.0f),
        glm::vec3(0.0f,  0.0f, 0.0f),
        glm::vec3(0.0f,  0.0f, 0.0f)
    };

    // configure the cube's VAO and VBO
    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // shader configuration
    // --------------------
    ourShader.use();
    ourShader.setInt("material.diffuse", 0);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime()); // Get current time
        deltaTime = currentFrame - lastFrame; // Update delta time
        lastFrame = currentFrame; // Update last frame

        // input
        processInput(window);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // Set the background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color buffer and depth buffer


        // enable shader before setting uniforms
        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);
        

        // directional light
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        //ourShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        //ourShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
        

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        // render the floor
        // set the model matrix of the floor
        glm::mat4 floorModel = glm::mat4(1.0f);
        ourShader.setMat4("model", floorModel);

        glBindTexture(GL_TEXTURE_2D, floorTexture); // bind the floor texture
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6); // draw the floor


        // if the G key is pressed, start the movement of the sphere
        if (isMoving) 
        {
            // Update the sphere's position
            ballPosition1 += ballVelocity1 * deltaTime;
            ballPosition2 += ballVelocity2 * deltaTime;
            ballPosition3 += ballVelocity3 * deltaTime;
            ballPosition4 += ballVelocity4 * deltaTime;

            // Update the rotation angle of the sphere
            rotationAngle1 += rotationSpeed * deltaTime;
            rotationAngle2 += rotationSpeed * deltaTime;
            rotationAngle3 += rotationSpeed * deltaTime; 
            rotationAngle4 += rotationSpeed * deltaTime;

            // The collision and rebound of the sphere with the floor
            handleCollisionWithFloor(ballPosition1, ballVelocity1, originalRadius);
            handleCollisionWithFloor(ballPosition2, ballVelocity2, originalRadius);
            handleCollisionWithFloor(ballPosition3, ballVelocity3, originalRadius);
            handleCollisionWithFloor(ballPosition4, ballVelocity4, originalRadius);

            // Collision and rebound between sphere and hemispherical space boundary
            handleCollisionWithBoundary(ballPosition1, ballVelocity1);
            handleCollisionWithBoundary(ballPosition2, ballVelocity2);
            handleCollisionWithBoundary(ballPosition3, ballVelocity3);
            handleCollisionWithBoundary(ballPosition4, ballVelocity4);

            // Collision detection and rebound between spheres
            if (checkCollisionWithBall(ballPosition1, originalRadius, ballPosition2, originalRadius)) 
            {
                handleCollisionWithBall(ballPosition1, ballVelocity1, originalRadius, ballPosition2, ballVelocity2, originalRadius);
            }

            if (checkCollisionWithBall(ballPosition1, originalRadius, ballPosition3, originalRadius))
            {
                handleCollisionWithBall(ballPosition1, ballVelocity1, originalRadius, ballPosition3, ballVelocity3, originalRadius);
			}

            if (checkCollisionWithBall(ballPosition1, originalRadius, ballPosition4, originalRadius))
            {
                handleCollisionWithBall(ballPosition1, ballVelocity1, originalRadius, ballPosition4, ballVelocity4, originalRadius);
            }

            if (checkCollisionWithBall(ballPosition2, originalRadius, ballPosition3, originalRadius))
            {
                handleCollisionWithBall(ballPosition2, ballVelocity2, originalRadius, ballPosition3, ballVelocity3, originalRadius);
			}

            if (checkCollisionWithBall(ballPosition2, originalRadius, ballPosition4, originalRadius))
            {
                handleCollisionWithBall(ballPosition2, ballVelocity2, originalRadius, ballPosition4, ballVelocity4, originalRadius);
			}

            if (checkCollisionWithBall(ballPosition3, originalRadius, ballPosition4, originalRadius))
            {
                handleCollisionWithBall(ballPosition3, ballVelocity3, originalRadius, ballPosition4, ballVelocity4, originalRadius);
			}
        }

        
        // Use new sphere position and rotation
        glm::mat4 model1 = glm::translate(glm::mat4(1.0f), ballPosition1);
        model1 = glm::rotate(model1, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the y axis

        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), ballPosition2);
        model2 = glm::rotate(model2, glm::radians(rotationAngle2), glm::vec3(0.0f, 1.0f, 0.0f)); 

        glm::mat4 model3 = glm::translate(glm::mat4(1.0f), ballPosition3);
        model3 = glm::rotate(model3, glm::radians(rotationAngle3), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 model4 = glm::translate(glm::mat4(1.0f), ballPosition4);
        model4 = glm::rotate(model4, glm::radians(rotationAngle4), glm::vec3(0.0f, 1.0f, 0.0f));

        // Render the 1st sphere
        // Calculate the position of the 1st sphere center and the radius
        // Only works with uniformly scaled spheres
        glm::vec3 ballCenterWorld1 = glm::vec3(model1 * glm::vec4(originalBallCenter, 1.0f));
        float ballRadiusWorld1 = originalRadius * glm::length(glm::vec3(model1[0]));

        // Print the coordinates of the 1st sphere center
        //std::cout << "Position of the sphere center 1: (" << ballCenterWorld1.x << ", " << ballCenterWorld1.y << ", " << ballCenterWorld1.z << ")" << std::endl;

        // point light 1 
        float testTime = static_cast<float>(sin(glfwGetTime()) / 2.0 + 0.5);
        pointLightPositions[0] = ballCenterWorld1; 
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]); // Set the position of the point light to the sphere center
        //ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[0].ambient", testTime, 0.0f, 0.0f); // Set the ambient light intensity of the point light
        ourShader.setVec3("pointLights[0].diffuse", testTime, 0.0f, 0.0f); // Set the diffuse light intensity of the point light
        //ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09f);
        ourShader.setFloat("pointLights[0].quadratic", 0.032f);
        
        ourShader.setMat4("model", model1); 
        ourModel.Draw(ourShader); // Draw the 1st sphere

        // Render the 2nd sphere
        // Calculate the position of the 2nd sphere center and the radius
        // Only works with uniformly scaled spheres
        glm::vec3 ballCenterWorld2 = glm::vec3(model2 * glm::vec4(originalBallCenter, 1.0f));
        float ballRadiusWorld2 = originalRadius * glm::length(glm::vec3(model2[0]));

        // Print the coordinates of the 2nd sphere center
        //std::cout << "Position of the sphere center 2: (" << ballCenterWorld2.x << ", " << ballCenterWorld2.y << ", " << ballCenterWorld2.z << ")" << std::endl;

        // point light 2
        pointLightPositions[1] = ballCenterWorld2;
        ourShader.setVec3("pointLights[1].position", pointLightPositions[1]); // Set the position of the point light to the sphere center
        //ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f); 
        ourShader.setVec3("pointLights[1].ambient", 0.0f, testTime, 0.0f); // Set the ambient light intensity of the point light
        ourShader.setVec3("pointLights[1].diffuse", 0.0f, testTime, 0.0f); // Set the diffuse light intensity of the point light
        //ourShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setFloat("pointLights[1].constant", 1.0f);
        ourShader.setFloat("pointLights[1].linear", 0.09f);
        ourShader.setFloat("pointLights[1].quadratic", 0.032f);
        
        ourShader.setMat4("model", model2);
        ourModel.Draw(ourShader); // Draw the 2nd sphere


        // Render the 3rd sphere
        glm::vec3 ballCenterWorld3 = glm::vec3(model3 * glm::vec4(originalBallCenter, 1.0f)); // Calculate the position of the 3rd sphere center
        
        // Print the coordinates of the 3rd sphere center
        //std::cout << "Position of the sphere center 3: (" << ballCenterWorld3.x << ", " << ballCenterWorld3.y << ", " << ballCenterWorld3.z << ")" << std::endl;
        
        // point light 3
        pointLightPositions[2] = ballCenterWorld3;
        ourShader.setVec3("pointLights[2].position", pointLightPositions[2]); // Set the position of the point light to the sphere center
        //ourShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[2].ambient", testTime, 0.0f, 0.0f); // Set the ambient light intensity of the point light
        ourShader.setVec3("pointLights[2].diffuse", testTime, 0.0f, 0.0f); // Set the diffuse light intensity of the point light
        //ourShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setFloat("pointLights[2].constant", 1.0f);
        ourShader.setFloat("pointLights[2].linear", 0.09f);
        ourShader.setFloat("pointLights[2].quadratic", 0.032f);

        ourShader.setMat4("model", model3);
        ourModel.Draw(ourShader); // Draw the 3rd sphere


        // Render the 4th sphere
        glm::vec3 ballCenterWorld4 = glm::vec3(model4 * glm::vec4(originalBallCenter, 1.0f)); // Calculate the position of the 4th sphere center
        
        
        // Print the coordinates of the 4th sphere center
        //std::cout << "Position of the sphere center 4: (" << ballCenterWorld4.x << ", " << ballCenterWorld4.y << ", " << ballCenterWorld4.z << ")" << std::endl;
        
        // point light 4
        pointLightPositions[3] = ballCenterWorld4;
        ourShader.setVec3("pointLights[3].position", pointLightPositions[3]); // Set the position of the point light to the sphere center
        //ourShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[3].ambient", 0.0f, testTime, 0.0f); // Set the ambient light intensity of the point light
        ourShader.setVec3("pointLights[3].diffuse", 0.0f, testTime, 0.0f); // Set the diffuse light intensity of the point light
        //ourShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setFloat("pointLights[3].constant", 1.0f);
        ourShader.setFloat("pointLights[3].linear", 0.09f);
        ourShader.setFloat("pointLights[3].quadratic", 0.032f);

        ourShader.setMat4("model", model4);
        ourModel.Draw(ourShader);


        // Render containers
        glBindTexture(GL_TEXTURE_2D, floorTexture); // use the floor texture
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 5; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 cubeModel = glm::mat4(1.0f);
            cubeModel = glm::translate(cubeModel, cubePositions[i]);
            float angle = 20.0f * i;
            cubeModel = glm::rotate(cubeModel, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            cubeModel = glm::scale(cubeModel, glm::vec3(3.0f, 3.0f, 3.0f));
            ourShader.setMat4("model", cubeModel);

            glDrawArrays(GL_TRIANGLES, 0, 36); // draw the container
        }


        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up resources
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}


// Function Definition
// keyboard control function
void processInput(GLFWwindow* window)
{
    float orbitSpeed = 30.0f; // camera rotation speed
    float moveSpeed = 15.0f; // camera forward and backward speed
    float currentKeyPressTime = static_cast<float>(glfwGetTime()); // current key press time

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.MoveForward(moveSpeed, deltaTime); // forward
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.MoveForward(-moveSpeed, deltaTime); // backward

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.Orbit(orbitSpeed, 0.0f, deltaTime); // rotate to the left
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.Orbit(-orbitSpeed, 0.0f, deltaTime); // rotate to the right
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        camera.Orbit(0.0f, -orbitSpeed, deltaTime); // rotate up
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.Orbit(0.0f, +orbitSpeed, deltaTime); // rotate down

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && (currentKeyPressTime - lastKeyPressTime) > keyPressDelay) 
    {
        isMoving = !isMoving; // Switch objects status
        lastKeyPressTime = currentKeyPressTime; // Update the time of the last key press
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// utility function for loading a 2D texture from file
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
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
    }
    else 
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// Calculate the coordinates of the center of the sphere
glm::vec3 calculateBallCenter(const vector<Vertex>& vertices)
{
    glm::vec3 sum(0.0f, 0.0f, 0.0f);
    for (const Vertex& vertex : vertices) 
    {
        sum += vertex.Position;
    }
    return sum / static_cast<float>(vertices.size());
}
 
// Calculate the radius of the sphere
float calculateBallRadius(const vector<Vertex>& vertices, glm::vec3 center)
{
    float maxDistance = 0.0f;
    for (const Vertex& vertex : vertices) 
    {
        float distance = glm::length(vertex.Position - center);
        if (distance > maxDistance) 
        {
            maxDistance = distance;
        }
    }
    return maxDistance;
}

// Collision detection and rebound between sphere and floor
void handleCollisionWithFloor(glm::vec3& position, glm::vec3& velocity, float radius) 
{
    if (position.y - radius < floorHeight) 
    {
        position.y = floorHeight + radius; // Prevent balls from passing through the floor
        velocity.y *= -1; // Reverse the velocity in the y-axis direction
    }
}

// Collision detection and rebound between sphere and hemispherical space boundary
void handleCollisionWithBoundary(glm::vec3& position, glm::vec3& velocity) 
{
    float distanceFromCenter = glm::length(position);
    if (distanceFromCenter > boundaryRadius) 
    {
        // Calculate reflection direction
        glm::vec3 normal = glm::normalize(position);
        velocity -= 2 * glm::dot(velocity, normal) * normal;
    }
}

// Collision detection function between spheres
bool checkCollisionWithBall(const glm::vec3& pos1, float radius1, const glm::vec3& pos2, float radius2)
{
    float distance = glm::length(pos1 - pos2);
    return distance < (radius1 + radius2);
}

// Collision response function between spheres
void handleCollisionWithBall(glm::vec3& pos1, glm::vec3& vel1, float radius1, glm::vec3& pos2, glm::vec3& vel2, float radius2) 
{
    // Get the collision normal
    glm::vec3 collisionNormal = glm::normalize(pos1 - pos2);

    // Calculate relative speed
    glm::vec3 relativeVelocity = vel1 - vel2;

    // Calculate the velocity along the collision normal
    float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

    // If the velocity is detached, there is no need to solve
    if (velocityAlongNormal > 0) return;

    // Calculate rebound speed
    glm::vec3 reboundVelocity = velocityAlongNormal * collisionNormal;

    // Update Speed
    vel1 -= reboundVelocity;
    vel2 += reboundVelocity;
}