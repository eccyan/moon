#ifndef MOON_VERTBUFFER_H
#define MOON_VERTBUFFER_H

#include <vector>
#include <algorithm>
#include "moon/intern.h"
#include "moon/gl.h"
#include "moon/graphics.hxx"

namespace Moon {
  class VertexBuffer {
  public:
    VertexBuffer(GLenum usage); // STATIC_DRAW, DYNAMIC_DRAW...
    ~VertexBuffer();

    void render(GLenum mode);
    void render_with_offset(GLenum mode, const int &offset); // adds offset to the indices used (draws vertex at vbo[indice + offset])
    void push_back(Vertex v);
    void push_back_vertices(Vertex *v, int vertex_count);
    void push_back_indices(GLuint i[], int index_count);
    void push_back(Vertex *v, int vertex_count, GLuint i[], int index_count); // indices need to be relative to the vertices pushed in
    void clear();
    void upload();
  private:
    void setup(); // set up the VAO
    GLuint vao_id;
    GLuint vbo_id;
    GLuint ibo_id;

    GLenum usage;
    bool dirty; // do we need to reupload?

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
  };
};

#endif