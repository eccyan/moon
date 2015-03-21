/*
 * Moon Vector2, a wrapper around Moon::Vector2
 */
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/numeric.h>
#include "moon/mrb/vector1.hxx"
#include "moon/mrb/vector2.hxx"
#include "vec_helper.h"

#define m_vector_operator(__op__) \
  return mmrb_vector2_value(mrb, get_vector2_value(mrb, self) __op__ vector2_from_mrb_args(mrb));

static struct RClass *vector2_class = NULL;

static void
vector2_free(mrb_state *mrb, void *p)
{
  Moon::Vector2 *vec2 = (Moon::Vector2*)p;
  if (vec2) {
    delete(vec2);
  }
}

MOON_C_API const struct mrb_data_type vector2_data_type = { "Vector2", vector2_free };

DEF_VEC_HELPERS(vector2, Moon::Vector2, vector2_class, &vector2_data_type);

static Moon::Vector2
mmrb_vector2_extract_mrb_num(mrb_state *mrb, mrb_value obj)
{
  double i = mmrb_to_flo(mrb, obj);
  return Moon::Vector2(i, i);
}

static Moon::Vector2
mmrb_vector2_extract_mrb_array(mrb_state *mrb, mrb_value obj)
{
  Moon::Vector2 result;
  int _ary_len = mrb_ary_len(mrb, obj);
  if (_ary_len != 2) {
    mrb_raisef(mrb, E_ARGUMENT_ERROR,
               "wrong array size %d (expected 2)", _ary_len);
  } else {
    result.x = mmrb_to_flo(mrb, RARRAY_PTR(obj)[0]);
    result.y = mmrb_to_flo(mrb, RARRAY_PTR(obj)[1]);
  }
  return result;
}

static Moon::Vector2
mmrb_vector2_extract_mrb_to_vec2(mrb_state *mrb, mrb_value obj)
{
  return get_vector2_value(mrb, mrb_funcall(mrb, obj, "to_vec2", 0));
}

static Moon::Vector2
mmrb_vector2_extract_args(mrb_state *mrb, int argc, mrb_value *vals)
{
  Moon::Vector2 result;
  switch (argc) {
  case 1:
    mrb_value val;
    val = vals[0];
    switch (mrb_type(val)) {
    case MRB_TT_FIXNUM:
    case MRB_TT_FLOAT:
      result = mmrb_vector2_extract_mrb_num(mrb, val);
      break;
    case MRB_TT_ARRAY:
      result = mmrb_vector2_extract_mrb_array(mrb, val);
      break;
    case MRB_TT_DATA:
      if (DATA_TYPE(val) == &vector1_data_type) {
        result.y = result.x = mmrb_to_flo(mrb, val);
        break;
      } else if (DATA_TYPE(val) == &vector2_data_type) {
        result = get_vector2_value(mrb, val);
        break;
      }
    default:
      if (mrb_respond_to(mrb, val, mrb_intern_cstr(mrb, "to_vec2"))) {
        result = mmrb_vector2_extract_mrb_to_vec2(mrb, val);
        break;
      } else {
        mrb_raisef(mrb, E_TYPE_ERROR,
                   "wrong type %S (expected Numeric, Array or Vector2)",
                   mrb_str_new_cstr(mrb, mrb_obj_classname(mrb, val)));
      }
    }
    break;
  case 2:
    result.x = mmrb_to_flo(mrb, vals[0]);
    result.y = mmrb_to_flo(mrb, vals[1]);
    break;
  default:
    mrb_raisef(mrb, E_ARGUMENT_ERROR,
               "wrong number of arguments (%d for 1..2)", argc);
  }
  return result;
}

static Moon::Vector2
vector2_from_mrb_args(mrb_state *mrb)
{
  mrb_value *vals;
  int len;
  mrb_get_args(mrb, "*", &vals, &len);
  return mmrb_vector2_extract_args(mrb, len, vals);
}

MOON_C_API Moon::Vector2
mmrb_to_vector2(mrb_state *mrb, mrb_value obj)
{
  return mmrb_vector2_extract_args(mrb, 1, &obj);
}

MOON_C_API mrb_value
mmrb_vector2_value(mrb_state *mrb, Moon::Vector2 vec)
{
  return set_vector2(mrb, new_vector2(mrb), vec);
}

static mrb_value
vector2_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_float x = 0.0;
  mrb_float y = 0.0;
  mrb_get_args(mrb, "|ff", &x, &y);
  cleanup_vector2(mrb, self);
  mrb_data_init(self, new Moon::Vector2(x, y), &vector2_data_type);
  return self;
}

static mrb_value
vector2_initialize_copy(mrb_state *mrb, mrb_value self)
{
  cleanup_vector2(mrb, self);
  mrb_data_init(self, new Moon::Vector2(vector2_from_mrb_args(mrb)), &vector2_data_type);
  return self;
}

static mrb_value
vector2_coerce(mrb_state *mrb, mrb_value self)
{
  mrb_value other;
  mrb_get_args(mrb, "o", &other);
  mrb_value argv[2] = { self, other };
  return mrb_ary_new_from_values(mrb, 2, argv);
}

static mrb_value
vector2_get_x(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, get_vector2(mrb, self)->x);
}

static mrb_value
vector2_get_y(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, get_vector2(mrb, self)->y);
}

static mrb_value
vector2_set_x(mrb_state *mrb, mrb_value self)
{
  mrb_float x;
  mrb_get_args(mrb, "f", &x);
  get_vector2(mrb, self)->x = x;
  return mrb_nil_value();
}

static mrb_value
vector2_set_y(mrb_state *mrb, mrb_value self)
{
  mrb_float y;
  mrb_get_args(mrb, "f", &y);
  get_vector2(mrb, self)->y = y;
  return mrb_nil_value();
}

static mrb_value
vector2_negate(mrb_state *mrb, mrb_value self)
{
  return mmrb_vector2_value(mrb, -get_vector2_value(mrb, self));
}

static mrb_value
vector2_identity(mrb_state *mrb, mrb_value self)
{
  return mrb_obj_dup(mrb, self);
}

static mrb_value
vector2_add(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(+);
}

static mrb_value
vector2_sub(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(-);
}

static mrb_value
vector2_mul(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(*);
}

static mrb_value
vector2_div(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(/);
}

/* // Uncomment this when integer vectors are supported
static mrb_value
vector2_not(mrb_state *mrb, mrb_value self)
{
  m_vector_mutate_copy(~(moon_vector_ref(result_vec)));
}

static mrb_value
vector2_modulo(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(%);
}

static mrb_value
vector2_shl(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(<<);
}

static mrb_value
vector2_shr(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(>>);
}

static mrb_value
vector2_and(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(&);
}

static mrb_value
vector2_or(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(|);
}

static mrb_value
vector2_xor(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(^);
}
*/

static mrb_value
vector2_dot(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, glm::dot(get_vector2_value(mrb, self), vector2_from_mrb_args(mrb)));
}

static mrb_value
vector2_normalize(mrb_state *mrb, mrb_value self)
{
  return mmrb_vector2_value(mrb, glm::normalize(get_vector2_value(mrb, self)));
}

static mrb_value
vector2_length(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, glm::length(get_vector2_value(mrb, self)));
}

static mrb_value
vector2_distance(mrb_state *mrb, mrb_value self)
{
  Moon::Vector2 diff = get_vector2_value(mrb, self) - vector2_from_mrb_args(mrb);
  return mrb_float_value(mrb, glm::dot(diff, diff));
}

static mrb_value
vector2_rotate(mrb_state *mrb, mrb_value self)
{
  mrb_float angle;
  mrb_get_args(mrb, "f", &angle);
  return mmrb_vector2_value(mrb, glm::rotate(get_vector2_value(mrb, self), (float)angle));
}

static mrb_value
vector2_lerp(mrb_state *mrb, mrb_value self)
{
  Moon::Vector2 *other;
  mrb_float delta;
  mrb_get_args(mrb, "df", &other, &vector2_data_type, &delta);
  return mmrb_vector2_value(mrb, glm::lerp(get_vector2_value(mrb, self), *other, (float)delta));
}

static mrb_value
vector2_set(mrb_state *mrb, mrb_value self)
{
  set_vector2(mrb, self, vector2_from_mrb_args(mrb));
  return self;
}

static mrb_value
vector2_to_a(mrb_state *mrb, mrb_value self)
{
  Moon::Vector2 *mvec2 = get_vector2(mrb, self);
  mrb_value argv[2] = { mrb_float_value(mrb, mvec2->x),
                        mrb_float_value(mrb, mvec2->y) };
  return mrb_ary_new_from_values(mrb, 2, argv);
}

static mrb_value
vector2_s_extract(mrb_state *mrb, mrb_value self)
{
  Moon::Vector2 src_vect = vector2_from_mrb_args(mrb);
  mrb_value argv[2] = { mrb_float_value(mrb, src_vect.x),
                        mrb_float_value(mrb, src_vect.y) };
  return mrb_ary_new_from_values(mrb, 2, argv);
}

static mrb_value
vector2_s_cast(mrb_state *mrb, mrb_value klass)
{
  return mmrb_vector2_value(mrb, vector2_from_mrb_args(mrb));
}

static mrb_value
vector2_s_circular_rand(mrb_state *mrb, mrb_value klass)
{
  mrb_float radius;
  mrb_get_args(mrb, "f", &radius);
  return mmrb_vector2_value(mrb, glm::circularRand((float)radius));
}

static mrb_value
vector2_s_disk_rand(mrb_state *mrb, mrb_value klass)
{
  mrb_float radius;
  mrb_get_args(mrb, "f", &radius);
  return mmrb_vector2_value(mrb, glm::diskRand((float)radius));
}

MOON_C_API void
mmrb_vector2_init(mrb_state *mrb, struct RClass *mod)
{
  vector2_class = mrb_define_class_under(mrb, mod, "Vector2", mrb->object_class);
  MRB_SET_INSTANCE_TT(vector2_class, MRB_TT_DATA);

  mrb_define_method(mrb, vector2_class, "initialize",      vector2_initialize,      MRB_ARGS_OPT(2));
  mrb_define_method(mrb, vector2_class, "initialize_copy", vector2_initialize_copy, MRB_ARGS_REQ(1));
  /* coercion */
  mrb_define_method(mrb, vector2_class, "coerce",          vector2_coerce,          MRB_ARGS_REQ(1));
  /* attribute setters */
  mrb_define_method(mrb, vector2_class, "x",               vector2_get_x,        MRB_ARGS_NONE());
  mrb_define_method(mrb, vector2_class, "y",               vector2_get_y,        MRB_ARGS_NONE());
  mrb_define_method(mrb, vector2_class, "x=",              vector2_set_x,        MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector2_class, "y=",              vector2_set_y,        MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector2_class, "set",             vector2_set,             MRB_ARGS_ANY());
  /* arithmetic */
  mrb_define_method(mrb, vector2_class, "-@",              vector2_negate,          MRB_ARGS_NONE());
  mrb_define_method(mrb, vector2_class, "+@",              vector2_identity,        MRB_ARGS_NONE());
  mrb_define_method(mrb, vector2_class, "+",               vector2_add,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector2_class, "-",               vector2_sub,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector2_class, "*",               vector2_mul,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector2_class, "/",               vector2_div,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector2_class, "dot",             vector2_dot,             MRB_ARGS_REQ(1));
  /*mrb_define_method(mrb, vector2_class, "cross",           vector2_cross,           MRB_ARGS_REQ(1));*/
  /* */
  mrb_define_method(mrb, vector2_class, "normalize",       vector2_normalize,       MRB_ARGS_NONE());
  mrb_define_method(mrb, vector2_class, "length",          vector2_length,          MRB_ARGS_NONE());
  mrb_define_method(mrb, vector2_class, "distance",        vector2_distance,        MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector2_class, "rotate",          vector2_rotate,          MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector2_class, "lerp",            vector2_lerp,            MRB_ARGS_REQ(2));
  /* bitwise operators */ /* If and only if we ever support integer based vectors */
  /*mrb_define_method(mrb, vector2_class, "~@",              vector2_not,             MRB_ARGS_NONE());*/
  /*mrb_define_method(mrb, vector2_class, "%",               vector2_modulo,          MRB_ARGS_REQ(1));*/
  /*mrb_define_method(mrb, vector2_class, "<<",              vector2_shl,             MRB_ARGS_REQ(1));*/
  /*mrb_define_method(mrb, vector2_class, ">>",              vector2_shr,             MRB_ARGS_REQ(1));*/
  /*mrb_define_method(mrb, vector2_class, "&",               vector2_and,             MRB_ARGS_REQ(1));*/
  /*mrb_define_method(mrb, vector2_class, "|",               vector2_or,              MRB_ARGS_REQ(1));*/
  /*mrb_define_method(mrb, vector2_class, "^",               vector2_xor,             MRB_ARGS_REQ(1));*/
  /* conversion */
  mrb_define_method(mrb, vector2_class, "to_a",            vector2_to_a,            MRB_ARGS_NONE());
  /* cast */
  mrb_define_class_method(mrb, vector2_class, "[]",        vector2_s_cast,          MRB_ARGS_ANY());
  mrb_define_class_method(mrb, vector2_class, "extract",   vector2_s_extract,       MRB_ARGS_REQ(1));
  /* gen */
  mrb_define_method(mrb, vector2_class, "circular_rand",   vector2_s_circular_rand, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector2_class, "disk_rand",       vector2_s_disk_rand,     MRB_ARGS_REQ(1));
}