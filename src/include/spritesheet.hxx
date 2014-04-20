#ifndef MOON_SPRITESHEET_H
#define MOON_SPRITESHEET_H

#include "moon.hxx"
#include "texture.hxx"

namespace Moon {
  class Spritesheet {
  public:
    GLfloat tile_width;
    GLfloat tile_height;

    float opacity;
    int total_sprites;

    std::shared_ptr<Color> color;

    Spritesheet(std::string filename, int tile_width, int tile_height);
    ~Spritesheet();

    void render(const int &x, const int &y, const float &z, const int &index);
  private:
    VertexBuffer VBO;

    std::shared_ptr<Texture> texture;
    std::shared_ptr<Shader>  shader;

    bool generate_buffers(); // use only in constructor
  };
};

#endif