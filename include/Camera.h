#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN
};

const float YAW = -90.f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 100.f;
const float FOV = 45.0f;

const glm::vec3 START_POSITION(0.0f, 0.0f, 0.0f);
const glm::vec3 UP(0.0f, 1.0f, 0.0f);

class Camera
{
public:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  float yaw;
  float pitch;

  float movementSpeed;
  float mouseSenesitivity;

  float nearPlane;
  float farPlane;
  float fov;

  Camera(glm::vec3 position = START_POSITION, glm::vec3 up = UP, float yaw = YAW, float pitch = PITCH);
  glm::mat4 getViewMatrix() const;

  void processKeyboard(CameraMovement direction, float deltaTime);
  void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

private:
  void updateCameraVectors();
};

#endif