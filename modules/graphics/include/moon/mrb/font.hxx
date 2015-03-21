#ifndef MMRB_FONT_H
#define MMRB_FONT_H

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include "moon/api.h"

MOON_C_API const struct mrb_data_type font_data_type;
MOON_C_API void mmrb_font_init(mrb_state *mrb, struct RClass* mod);

#endif