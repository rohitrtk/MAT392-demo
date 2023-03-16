#include "Texture.h"

#include "stb_image.h"
#include <iostream>

Texture::Texture():
  width(0),
  height(0),
  internalFormat(GL_RGB),
  imageFormat(GL_RGB),
  wrapS(GL_REPEAT),
  wrapT(GL_REPEAT),
  filterMin(GL_LINEAR),
  filterMax(GL_LINEAR)
{
  glGenTextures(1, &this->id);
}

Texture::Texture(std::string path): Texture()
{
  int width, height, nrChannels;
  unsigned char* textureData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  if (!textureData)
  {
    std::cerr << "Error: Failed to load texture from: " << path << std::endl;
  }

  this->generate(width, height, textureData);
  stbi_image_free(textureData);
}

void Texture::generate(unsigned int width, unsigned int height, unsigned char* data)
{
  if (!data)
  {
    std::cerr << "Error: Unable to generate texture" << std::endl;
  }

  this->width = width;
  this->height = height;

  glBindTexture(GL_TEXTURE_2D, this->id);
  glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, width, height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMax);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const
{
  glBindTexture(GL_TEXTURE_2D, this->id);
}