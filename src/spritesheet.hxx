#ifndef MOON_SPRITESHEET_H
#define MOON_SPRITESHEET_H

#include "moon.hxx"
#include "texture.hxx"

namespace Moon {
  class Spritesheet {
  public:
    Spritesheet(std::string filename, int tile_width, int tile_height);
    ~Spritesheet();

    void render(int x, int y, int index);
  private:
    bool generate_buffers(); // use only in constructor

    GLfloat tileWidth;
    GLfloat tileHeight;
    // VBO data
    GLuint mVertexDataBuffer;
    GLuint* mIndexBuffers;

    int totalSprites;

    std::shared_ptr<Texture> texture;
  };
};

#endif