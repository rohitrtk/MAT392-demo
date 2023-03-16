#include "Mesh.h"

namespace Model
{
  Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
  {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    this->setupMesh();
  }

  void Mesh::setupMesh()
  {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int),
      &this->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
  }

  void Mesh::draw(Shader& shader)
  {
    unsigned int numDiffuse = 1;
    unsigned int numSpecular = 1;

    for (unsigned int i = 0; i < this->textures.size(); i++)
    {
      glActiveTexture(GL_TEXTURE0 + i);

      std::stringstream ss;
      std::string name = this->textures[i].type;

      if ("texture_diffuse" == name)
      {
        ss << numDiffuse++;
      }
      else if ("texture_specular" == name)
      {
        ss << numSpecular++;
      }

      std::string number = ss.str();

      shader.setInt(("material." + name + number).c_str(), i);
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
}
