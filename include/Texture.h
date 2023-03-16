#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>

class Texture
{
public:
  unsigned int id;

  unsigned int width, height;
  unsigned int internalFormat, imageFormat;

  unsigned int wrapS, wrapT;
  unsigned int filterMin, filterMax;

  Texture();
  Texture(std::string path);

  void generate(unsigned int width, unsigned int height, unsigned char* data);

  void bind() const;
};

#endif