#ifndef MOON_SHADER_H
#define MOON_SHADER_H

#include "moon.hxx"

namespace Moon {
  class Shader {
  public:
    Shader(const char *vertexfile, const char *fragmentfile);
    ~Shader();

    void   use();
    GLuint get_program();
    GLint  get_attribute(const char *name);
    GLint  get_uniform(const char *name);

    static glm::mat4 projection_matrix; // TEMPORARY LOCATION, TODO MOVE TO RENDERER
  private:
    std::string file_read(const char *filePath);
    void   print_log(GLuint object);
    GLuint create_shader(const char* filename, GLenum type);
    GLuint create_program(const char *vertexfile, const char *fragmentfile);

    GLuint  _program;
    std::unordered_map<const char *,GLuint> _attributeList;
    std::unordered_map<const char *,GLuint> _uniformLocationList;
  };
};

#endif