#include "moon/engine.hxx"
#include "moon/sprite.hxx"
#include "moon/shader.hxx"
#include "moon/texture.hxx"
#include "moon/vector4.hxx"

namespace Moon {
  Sprite::Sprite() : m_vbo(GL_DYNAMIC_DRAW)
  {
    shader = NULL;
    use_clip = false;
    opacity = 1.0;
    angle = 0.0;
    origin = Vector2(0, 0);
    color = Vector4(1.0, 1.0, 1.0, 1.0);
    tone = Vector4(0.0, 0.0, 0.0, 1.0);
    m_texture = NULL;
  };

  Sprite::~Sprite() {

  };

  // change Sprite's texture
  void Sprite::SetTexture(Texture* tex) {
    m_texture = tex;
    GenerateBuffers();
  };

  Texture* Sprite::GetTexture() {
    return m_texture;
  };

  void Sprite::SetClipRect(IntRect clip) {
    m_clip_rect = std::move(clip); // passing by value already makes a copy
    GenerateBuffers();
  };

  IntRect Sprite::GetClipRect() {
    return m_clip_rect;
  };

  void Sprite::LoadTexture(Texture *texture) {
    m_texture = texture;
    GenerateBuffers();
  }

  bool Sprite::GenerateBuffers() {
    m_vbo.Clear();
    // If the texture exists
    if (m_texture->GetID() != 0) {
      //Texture coordinates
      GLfloat s0 = 0.f;
      GLfloat s1 = 1.f;
      GLfloat t0 = 0.f;
      GLfloat t1 = 1.f;

      //Vertex coordinates
      GLfloat width = m_texture->GetWidth();
      GLfloat height = m_texture->GetHeight();

      //Handle clipping
      if (use_clip) {
        //Texture coordinates
        s0 = (float)m_clip_rect.x / m_texture->GetWidth();
        s1 = (float)(m_clip_rect.x + m_clip_rect.w) / m_texture->GetWidth();
        t0 = (float)m_clip_rect.y / m_texture->GetHeight();
        t1 = (float)(m_clip_rect.y + m_clip_rect.h) / m_texture->GetHeight();
        //Vertex coordinates
        width = m_clip_rect.w;
        height = m_clip_rect.h;
      }

      Vertex vertices[4] = {
        { {0.f,   0.f},    {s0, t0}, Vector4(0, 0, 0, 0) },
        { {width, 0.f},    {s1, t0}, Vector4(0, 0, 0, 0) },
        { {width, height}, {s1, t1}, Vector4(0, 0, 0, 0) },
        { {0.f,   height}, {s0, t1}, Vector4(0, 0, 0, 0) }
      };
      GLuint indices[4] = {0, 1, 3, 2}; // rendering indices
      m_vbo.PushBack(vertices, 4, indices, 4);
      return true;
    };
    return false;
  };

  void Sprite::Render(const float x, const float y, const float z) {
    if (!m_texture) return;
    if (!shader) return;

    shader->Use();
    // rotation matrix - rotate the model around specified origin
    // really ugly, we translate the rotation origin to 0,0, rotate,
    // then translate back to original position
    glm::mat4 rotation_matrix = glm::translate(glm::rotate(
      glm::translate(glm::mat4(1.0f), glm::vec3(origin.x, origin.y, 0)),
      glm::radians(angle),
      glm::vec3(0, 0, 1)
    ), glm::vec3(-origin.x, -origin.y, 0));
    // model matrix - move it to the correct position in the world
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    // calculate the ModelViewProjection matrix (faster to do on CPU, once for all vertices instead of per vertex)
    glm::mat4 mvp_matrix = Shader::projection_matrix * Shader::view_matrix * model_matrix * rotation_matrix;
    glUniformMatrix4fv(shader->GetUniform("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniform1f(shader->GetUniform("opacity"), opacity);
    glUniform4fv(shader->GetUniform("color"), 1, glm::value_ptr(color));
    glUniform4fv(shader->GetUniform("tone"), 1, glm::value_ptr(tone));
    //Set texture ID
    glActiveTexture(GL_TEXTURE0);
    m_texture->Bind();
    glUniform1i(shader->GetUniform("tex"), /*GL_TEXTURE*/0);
    m_vbo.Render(GL_TRIANGLE_STRIP);
  };
};
