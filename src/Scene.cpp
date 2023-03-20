#include "Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

Scene::Scene():
  width(INITIAL_WINDOW_WIDTH),
  height(INITIAL_WINDOW_HEIGHT)
{
  if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW\n";
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  this->window = glfwCreateWindow(this->width, this->height, WINDOW_TITLE.begin(), NULL, NULL);
  if (this->window == NULL)
  {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
  {
    std::cerr << "Failed to initiaize GLAD\n";
    return;
  }

  glViewport(0, 0, this->width, this->height);
  glEnable(GL_DEPTH_TEST);
  this->aspectRatio = this->width / this->height;

  //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  //glfwSetCursorPosCallback(window, mouse_callback);
  //glfwSetKeyCallback(window, key_callback);

  this->sceneControls.useMouseLock = false;
  this->sceneControls.useWireFrame = false;
  this->sceneControls.useSkybox = true;

  this->camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

  this->initImGui();
}

Scene::~Scene()
{
  this->shutdownImGui();

  glfwTerminate();
}

void Scene::start()
{
  float deltaTime = 0.0f, lastTime = 0.0f;
  double timer = 0.0;
  int numFrames = 0;

  while (!glfwWindowShouldClose(this->window))
  {
    // Frame rate & delta time
    numFrames++;
    float currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;

    if (currentTime - timer >= 1.0)
    {
      this->frameCount = static_cast<unsigned int>(1000.0 / static_cast<double>(numFrames));
      numFrames = 0;
      timer += 1.0;
    }
    lastTime = currentTime;

    // Process input
    this->processInput(deltaTime);

    glfwSetInputMode(this->window, GLFW_CURSOR, this->sceneControls.useMouseLock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_HIDDEN);
    glPolygonMode(GL_FRONT_AND_BACK, this->sceneControls.useWireFrame ? GL_LINE : GL_FILL);

    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* --------- RENDER ---------- */

    /* Creation of model, view, and projection matricies per frame */

    /* Model Matrix */
    glm::mat4 model = glm::mat4(1.0f);

    /* View Matrix */
    glm::mat4 view = this->camera->getViewMatrix();

    /* Projection Matrix */
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(this->camera->fov), this->aspectRatio, this->camera->nearPlane, this->camera->farPlane);

    this->renderImGui();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Scene::processInput(float deltaTime)
{
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) this->camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) this->camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) this->camera->processKeyboard(CameraMovement::LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) this->camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) this->camera->processKeyboard(CameraMovement::UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) this->camera->processKeyboard(CameraMovement::DOWN, deltaTime);
}

void Scene::initImGui()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  //ImGuiIO& io = ImGui::GetIO();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(this->window, true);
  ImGui_ImplOpenGL3_Init(GLSL_TARGET.begin());
}

void Scene::renderImGui()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Menu :)");
  ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%dms/frame", this->frameCount);
  ImGui::Checkbox("Mouse Lock (M)", &this->sceneControls.useMouseLock);
  ImGui::Checkbox("Wireframe (N)", &this->sceneControls.useWireFrame);
  ImGui::Checkbox("Skybox (B)", &this->sceneControls.useSkybox);

  //ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&airplaneScale), 0.001f, 0.01f);
  //ImGui::SliderFloat3("Translate", reinterpret_cast<float*>(&airplanePosition), -1.0f, 1.0f);
  //ImGui::SliderFloat3("Rotate", reinterpret_cast<float*>(&airplaneRotation), 0.0f, 359.0f);

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Scene::shutdownImGui()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Scene::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void Scene::mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{

}

void Scene::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // Close
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

  // Mouse locked
  if (key == GLFW_KEY_M && action == GLFW_PRESS) this->sceneControls.useMouseLock = !this->sceneControls.useMouseLock;

  // Wireframe
  if (key == GLFW_KEY_N && action == GLFW_PRESS) this->sceneControls.useWireFrame = !this->sceneControls.useWireFrame;

  // Skybox
  if (key == GLFW_KEY_B && action == GLFW_PRESS) this->sceneControls.useSkybox = !this->sceneControls.useSkybox;
}