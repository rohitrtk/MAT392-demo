#include "Camera.h"

#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch):
  front(glm::vec3(0.0f, 0.0f, -1.0f)),
  movementSpeed(SPEED),
  mouseSenesitivity(SENSITIVITY),
  nearPlane(NEAR_PLANE),
  farPlane(FAR_PLANE),
  fov(FOV)
{
  this->position = position;
  this->worldUp = up;
  this->yaw = yaw;
  this->pitch = pitch;

  this->updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
  return glm::lookAt(this->position, this->position + this->front, this->up);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
  float velocity = this->movementSpeed * deltaTime;

  if (direction == CameraMovement::FORWARD) this->position += this->front * velocity;
  if (direction == CameraMovement::BACKWARD) this->position -= this->front * velocity;
  if (direction == CameraMovement::LEFT) this->position -= this->right * velocity;
  if (direction == CameraMovement::RIGHT) this->position += this->right * velocity;
  if (direction == CameraMovement::UP) this->position += this->up * velocity;
  if (direction == CameraMovement::DOWN) this->position -= this->up * velocity;
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
  xOffset *= this->mouseSenesitivity;
  yOffset *= this->mouseSenesitivity;

  this->yaw += xOffset;
  this->pitch += yOffset;

  if (constrainPitch) this->pitch = std::clamp(this->pitch, -89.0f, 89.0f);

  this->updateCameraVectors();
}

void Camera::updateCameraVectors()
{
  glm::vec3 front;
  front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  front.y = sin(glm::radians(this->pitch));
  front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  this->front = front;
  this->right = glm::normalize(glm::cross(this->front, this->worldUp));
  this->up = glm::normalize(glm::cross(this->right, this->front));
}