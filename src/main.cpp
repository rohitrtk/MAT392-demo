#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const std::string WINDOW_TITLE = "MAT392 - Mathematics in Computer Graphics Demo";
const float WINDOW_ASPECT_RATIO = WINDOW_WIDTH / WINDOW_HEIGHT;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH * 0.5f;
float lastY = WINDOW_HEIGHT * 0.5f;
bool firstMouse = false;

float skyboxVertices[] = {
  -1.0f,  1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

  -1.0f,  1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f
};

float deltaTime = 0.0f;
float lastFrame = 0.0f;
unsigned int frameCount = 0;

bool mouseLocked = false;
bool wireFrame = false;
bool useSkybox = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void processInput(GLFWwindow* window)
{
  // Movement
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(CameraMovement::LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.processKeyboard(CameraMovement::UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.processKeyboard(CameraMovement::DOWN, deltaTime);
}

int main(int argc, char** argv)
{
  if (!glfwInit())
  {
    std::cerr << "Failed to initiaize GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
  if (window == NULL)
  {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();

    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
  {
    std::cerr << "Failed to initiaize GLAD" << std::endl;
    return EXIT_FAILURE;
  }

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_WIDTH);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetInputMode(window, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_HIDDEN);

  glEnable(GL_DEPTH_TEST);

  // Model from https://free3d.com/3d-model/airplane-v2--549103.html
  Shader airplaneShader("./../shaders/airplane/vertex.glsl", "./../shaders/airplane/fragment.glsl");
  Model::Model airplaneModel("./../res/models/airplane/11805_airplane_v2_L2.obj");
  //Model::Model airplaneModel("./../res//models/tree-high/tree01.obj");

  glm::vec3 airplanePosition(0.0f, 0.0f, 0.0f);
  glm::vec3 airplaneRotation(0.0f, 0.0f, 0.0f);
  glm::vec3 airplaneScale(0.001f, 0.001f, 0.001f);
  //glm::vec3 airplaneScale(.1f, .1f, .1f);

  float ambientLight = 0.5;
  glm::vec3 ambientColour(1.0f, 1.0f, 1.0f);

  // Skybox
  Shader skyboxShader("./../shaders/skybox/vertex.glsl", "./../shaders/skybox/fragment.glsl");
  unsigned int skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));

  unsigned int skybox;
  glGenTextures(1, &skybox);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);

  // R L T B B F
  std::vector<std::string> faces =
  {
    "./../res/images/skyrender0005.bmp",
    "./../res/images/skyrender0001.bmp",
    "./../res/images/skyrender0003.bmp", // DON'T TOUCH!
    "./../res/images/skyrender0006.bmp", // DON'T TOUCH!
    "./../res/images/skyrender0004.bmp",
    "./../res/images/skyrender0002.bmp",
  };
  int width, height, nrChannels;
  unsigned char* data;
  for (unsigned int i = 0; i < faces.size(); i++)
  {
    data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else
    {
      std::cerr << "Failed to load skybox" << std::endl;
    }

    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  skyboxShader.use();
  skyboxShader.setInt("skybox", 0);

  std::cout << "Initializing imgui!" << std::endl;
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  //ImGuiIO& io = ImGui::GetIO();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  std::cout << "Starting Program!" << std::endl;
  int numFrames = 0;
  double timer = 0.0;
  // Main Loop
  while (!glfwWindowShouldClose(window))
  {
    numFrames++;
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;

    if (currentFrame - timer >= 1.0)
    {
      frameCount = static_cast<unsigned int>(1000.0 / static_cast<double>(numFrames));
      numFrames = 0;
      timer += 1.0;
    }
    lastFrame = currentFrame;

    processInput(window);

    glfwSetInputMode(window, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_HIDDEN);
    glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);

    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    //glClearColor(1.f, 1.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Creation of model, view, and projection matricies per frame */

    /* Model/Transformation Matrix */
    glm::mat4 model = glm::mat4(1.0f);

    /* View Matrix */
    glm::mat4 view = camera.getViewMatrix();

    /* Projection Matrix */
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.fov), WINDOW_ASPECT_RATIO, camera.nearPlane, camera.farPlane);

    /* Send model, view, and projection matrix to the vertex shader */
    airplaneShader.use();
    airplaneShader.setMat4("projection", projection);
    airplaneShader.setMat4("view", view);

    model = glm::translate(model, airplanePosition);
    model = glm::scale(model, airplaneScale);
    model = glm::rotate(model, glm::radians(airplaneRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(airplaneRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(airplaneRotation.z), glm::vec3(1.0f, 0.0f, 1.0f));
    airplaneShader.setMat4("model", model);
    airplaneModel.draw(airplaneShader);

    airplaneShader.setFloat("ambientStrength", ambientLight);
    airplaneShader.setVec3("ambientColour", ambientColour);

    if (useSkybox)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glDepthFunc(GL_LEQUAL);
      skyboxShader.use();
      view = glm::mat4(glm::mat3(view));
      skyboxShader.setMat4("view", view);
      skyboxShader.setMat4("projection", projection);
      glBindVertexArray(skyboxVAO);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
      glDepthFunc(GL_LESS);

      skyboxShader.setFloat("ambientStrength", ambientLight);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Menu
    ImGui::Begin("Menu :)");
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%dms/frame", frameCount);

    // Keybinds
    ImGui::Checkbox("Mouse Lock (M)", &mouseLocked);
    ImGui::Checkbox("Wireframe (N)", &wireFrame);
    ImGui::Checkbox("Skybox (B)", &useSkybox);

    // Airplane transform
    ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&airplaneScale), 0.001f, 0.01f);
    ImGui::SliderFloat3("Translate", reinterpret_cast<float*>(&airplanePosition), -1.0f, 1.0f);
    ImGui::SliderFloat3("Rotate", reinterpret_cast<float*>(&airplaneRotation), 0.0f, 359.0f);

    // Scene Lighting
    ImGui::SliderFloat("Ambient", reinterpret_cast<float*>(&ambientLight), 0.0f, 1.0f);
    ImGui::SliderFloat3("Ambient Colour", reinterpret_cast<float*>(&ambientColour), 0.0f, 1.0f);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glDeleteVertexArrays(1, &skyboxVAO);
  glDeleteBuffers(1, &skyboxVBO);

  glfwTerminate();

  return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
  if (!mouseLocked) return;

  float xPos = static_cast<float>(xPosIn);
  float yPos = static_cast<float>(yPosIn);

  if (firstMouse)
  {
    lastX = xPos;
    lastY = yPos;
    firstMouse = false;
  }

  float xOffset = xPos - lastX;
  float yOffset = lastY - yPos;

  lastX = xPos;
  lastY = yPos;

  camera.processMouseMovement(xOffset, yOffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // Close
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

  // Mouse locked
  if (key == GLFW_KEY_M && action == GLFW_PRESS) mouseLocked = !mouseLocked;

  // Wireframe
  if (key == GLFW_KEY_N && action == GLFW_PRESS) wireFrame = !wireFrame;

  // Skybox
  if (key == GLFW_KEY_B && action == GLFW_PRESS) useSkybox = !useSkybox;
}