#include "mrb.hxx"
#include "mrb_err.hxx"
#include "spritesheet.hxx"

namespace Moon {

  static mrb_sym id_opacity;
  static mrb_sym id_tone;
  static mrb_sym id_color;

  static void moon_mrb_spritesheet_deallocate(mrb_state *mrb, void *p) {
    delete((Spritesheet*)p);
  };

  const struct mrb_data_type spritesheet_data_type = {
    "Spritesheet", moon_mrb_spritesheet_deallocate,
  };

  static mrb_value
  moon_mrb_spritesheet_initialize(mrb_state *mrb, mrb_value self) {
    mrb_value filename;
    mrb_int tile_width, tile_height;
    mrb_get_args(mrb, "Sii", &filename, &tile_width, &tile_height);

    Spritesheet *spritesheet = new Spritesheet(mrb_string_value_ptr(mrb, filename),
                                               tile_width, tile_height);

    DATA_TYPE(self) = &spritesheet_data_type;
    DATA_PTR(self) = spritesheet;

    return mrb_nil_value();
  };

  static mrb_value
  moon_mrb_spritesheet_render(mrb_state *mrb, mrb_value self) {
    mrb_int index;
    mrb_float x, y, z;
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "fffi|H", &x, &y, &z, &index, &options);

    Spritesheet *spritesheet;
    Data_Get_Struct(mrb, self, &spritesheet_data_type, spritesheet);

    render_options render_op;
    render_op.color = { 1.0, 1.0, 1.0, 1.0 };
    render_op.tone = { 0.0, 0.0, 0.0, 1.0 };
    render_op.opacity = 1.0f;

    if (!mrb_nil_p(options)) {
      mrb_value keys = mrb_hash_keys(mrb, options);
      int len = mrb_ary_len(mrb, keys);
      mrb_value *keys_ary = RARRAY_PTR(keys);

      for (int i=0; i < len; ++i) {
        mrb_value key = keys_ary[i];

        if (mrb_symbol_p(key)) {
          if (mrb_symbol(key) == id_opacity) {
            render_op.opacity = mrb_to_flo(mrb, mrb_hash_get(mrb, options, key));

          } else if (mrb_symbol(key) == id_color) {
            std::shared_ptr<Color>* color_ptr;
            Data_Get_Struct(mrb, mrb_hash_get(mrb, options, key),
                                 &color_data_type, color_ptr);
            render_op.color = **color_ptr;

          } else if (mrb_symbol(key) == id_tone) {
            std::shared_ptr<Color>* color_ptr;
            Data_Get_Struct(mrb, mrb_hash_get(mrb, options, key),
                                 &color_data_type, color_ptr);
            render_op.tone = **color_ptr;
          }
        }
      }
    }

    spritesheet->render(x, y, z, index, render_op);
    return mrb_nil_value();
  };

  static mrb_value
  moon_mrb_spritesheet_cell_width(mrb_state *mrb, mrb_value self) {
    Spritesheet *spritesheet;
    Data_Get_Struct(mrb, self, &spritesheet_data_type, spritesheet);
    return mrb_fixnum_value((int)spritesheet->tile_width);
  }

  static mrb_value
  moon_mrb_spritesheet_cell_height(mrb_state *mrb, mrb_value self) {
    Spritesheet *spritesheet;
    Data_Get_Struct(mrb, self, &spritesheet_data_type, spritesheet);
    return mrb_fixnum_value((int)spritesheet->tile_height);
  }

  static mrb_value
  moon_mrb_spritesheet_cell_count(mrb_state *mrb, mrb_value self) {
    Spritesheet *spritesheet;
    Data_Get_Struct(mrb, self, &spritesheet_data_type, spritesheet);
    return mrb_fixnum_value((int)spritesheet->total_sprites);
  }

  struct RClass* moon_mrb_spritesheet_init(mrb_state *mrb) {
    struct RClass *spritesheet_class;
    spritesheet_class = mrb_define_class_under(mrb, moon_module, "Spritesheet", mrb->object_class);
    MRB_SET_INSTANCE_TT(spritesheet_class, MRB_TT_DATA);

    mrb_define_method(mrb, spritesheet_class, "initialize",  moon_mrb_spritesheet_initialize,     MRB_ARGS_REQ(3));
    mrb_define_method(mrb, spritesheet_class, "render",      moon_mrb_spritesheet_render,         MRB_ARGS_ARG(4,1));

    mrb_define_method(mrb, spritesheet_class, "cell_width",  moon_mrb_spritesheet_cell_width,     MRB_ARGS_NONE());
    mrb_define_method(mrb, spritesheet_class, "cell_height", moon_mrb_spritesheet_cell_height,    MRB_ARGS_NONE());
    mrb_define_method(mrb, spritesheet_class, "cell_count",  moon_mrb_spritesheet_cell_count,     MRB_ARGS_NONE());

    id_opacity = mrb_intern_cstr(mrb, "opacity");
    id_tone    = mrb_intern_cstr(mrb, "tone");
    id_color   = mrb_intern_cstr(mrb, "color");

    return spritesheet_class;
  };
};