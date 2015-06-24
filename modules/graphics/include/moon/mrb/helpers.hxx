#ifndef MOON_MRB_HELPERS
#define MOON_MRB_HELPERS

#include <mruby.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include "moon/shader.hxx"
#include "moon/texture.hxx"
#include "moon/mrb/shader.hxx"
#include "moon/mrb/texture.hxx"

#define IVget(_name_) mrb_iv_get(mrb, self, mrb_intern_lit(mrb, _name_))
#define IVset(_name_, _value_) mrb_iv_set(mrb, self, mrb_intern_lit(mrb, _name_), _value_)

#define KEY_TEXTURE "@texture"
#define KEY_SHADER "@shader"

#define TEXTURE_CLASS mrb_module_get_under(mrb, mrb_module_get(mrb, "Moon"), "Texture")
;

static inline Moon::Texture*
get_texture(mrb_state *mrb, mrb_value self)
{
  return static_cast<Moon::Texture*>(mrb_data_get_ptr(mrb, self, &texture_data_type));
}

static inline Moon::Shader*
get_shader(mrb_state *mrb, mrb_value self)
{
  return static_cast<Moon::Shader*>(mrb_data_get_ptr(mrb, self, &shader_data_type));
}

#endif