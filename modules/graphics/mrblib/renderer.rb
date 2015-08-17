module Moon
  class Renderer
    def render(shader, vbo, texture, transform, mode, index = 0)
      apply_transform shader, transform
      bind_texture shader, texture
      vbo.render mode, index
    end

    class << self
      attr_accessor :instance
    end
  end
end