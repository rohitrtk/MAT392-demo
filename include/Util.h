#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#define log(x) std::cout << x << std::endl

unsigned int m_glCheckError_(const char* file, int line)
{
  unsigned int errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR)
  {
    std::cerr << "Error: OpenGL encountered error " << errorCode << " in " << file << " | Line: " << line << std::endl;
  }

  return errorCode;
}

#define m_glCheckError() m_glCheckError_(__FILE__, __LINE__)
