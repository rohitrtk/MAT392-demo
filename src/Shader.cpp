#include "Shader.h"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath)
{
  std::string vertexShaderCode, fragmentShaderCode;
  std::ifstream vShaderFile, fShaderFile;

  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try
  {
    vShaderFile.open(vertexShaderPath);
    fShaderFile.open(fragmentShaderPath);

    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexShaderCode = vShaderStream.str();
    fragmentShaderCode = fShaderStream.str();
  }
  catch (std::ifstream::failure& e)
  {
    std::cerr << "ERROR: Failed to read file: " << e.what() << std::endl;
  }

  const char* vShaderCode = vertexShaderCode.c_str();
  const char* fShaderCode = fragmentShaderCode.c_str();

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vShaderCode, NULL);
  glCompileShader(vertexShader);
  this->checkCompileErrors(vertexShader, "VERTEX");

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
  glCompileShader(fragmentShader);
  this->checkCompileErrors(fragmentShader, "FRAGMENT");

  this->id = glCreateProgram();
  glAttachShader(this->id, vertexShader);
  glAttachShader(this->id, fragmentShader);
  glLinkProgram(this->id);
  this->checkCompileErrors(this->id, "PROGRAM");

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::use()
{
  glUseProgram(this->id);;
}

void Shader::setBool(const std::string& name, bool value) const
{
  glUniform1i(glGetUniformLocation(this->id, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const
{
  glUniform1i(glGetUniformLocation(this->id, name.c_str()), (value));
}

void Shader::setFloat(const std::string& name, float value) const
{
  glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
  glUniform2fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
  glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
  glUniform4fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
  glUniformMatrix2fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
  glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
  int success;
  char infoLog[1024];
  if (type != "PROGRAM")
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cerr << "ERROR: Failed to compile " << type << " shader.\n" << infoLog << std::endl;
    }
    else
    {
      std::cout << "The " << type << " shader was compiled successfully." << std::endl;
    }
  }
  else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cerr << "ERROR: Failed to link " << type << " program.\n" << infoLog << std::endl;
    }
    else
    {
      std::cout << "The shader was linked successfully." << std::endl;
    }
  }
}