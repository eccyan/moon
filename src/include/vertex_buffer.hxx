#ifndef MOON_VERTBUFFER_H
#define MOON_VERTBUFFER_H

#include "moon.hxx"
#include "graphics.hxx"
#include <vector>
#include <algorithm>

namespace Moon {
  class VertexBuffer {
  public:
    VertexBuffer(GLenum usage); // STATIC_DRAW, DYNAMIC_DRAW...
    ~VertexBuffer();

    void render(GLenum mode, GLint vertex_pos, GLint texcoord, GLint color);
    void push_back(vertex v);
    void push_back(vertex *v, int vertex_count, GLuint i[], int index_count);
    void clear();
    void upload();
  private:
    GLuint vbo_id;
    GLuint ibo_id;

    GLenum usage;
    bool dirty; // do we need to reupload?

    std::vector<vertex> vertices;
    std::vector<GLuint> indices;
  };
};

#endif