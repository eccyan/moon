/*
 * Moon Vector1, a wrapper around glm::vec1
 */
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/numeric.h>
#include "moon/mrb/vector1.hxx"
#include "vec_helper.h"

#define m_vector_operator(__op__) \
  return mmrb_vector1_value(mrb, get_vector1_value(mrb, self) __op__ vector1_from_mrb_args(mrb));

#define m_vector_int_operator(__op__) \
  return mmrb_vector1_value(mrb, Moon::Vector1(Moon::Vector1i(get_vector1_value(mrb, self)) __op__ Moon::Vector1i(vector1_from_mrb_args(mrb))));

static void
vector1_free(mrb_state *mrb, void *p)
{
  Moon::Vector1 *vec = (Moon::Vector1*)p;
  if (vec) {
    delete(vec);
  }
}

MOON_C_API const struct mrb_data_type vector1_data_type = { "Moon::Vector1", vector1_free };

DEF_VEC_HELPERS(vector1, Moon::Vector1, mmrb_get_vector1_class(mrb), &vector1_data_type);

static Moon::Vector1
mmrb_vector1_extract_mrb_num(mrb_state *mrb, mrb_value obj)
{
  double i = mmrb_to_flo(mrb, obj);
  return Moon::Vector1(i);
}

static Moon::Vector1
mmrb_vector1_extract_mrb_array(mrb_state *mrb, mrb_value obj)
{
  Moon::Vector1 result;
  int _ary_len = mrb_ary_len(mrb, obj);
  if (_ary_len != 1) {
    mrb_raisef(mrb, E_ARGUMENT_ERROR,
               "wrong array size %d (expected 1)", _ary_len);
  } else {
    result.x = mrb_to_flo(mrb, RARRAY_PTR(obj)[0]);
  }
  return result;
}

static Moon::Vector1
mmrb_vector1_extract_mrb_to_vec1(mrb_state *mrb, mrb_value obj)
{
  return get_vector1_value(mrb, mrb_funcall(mrb, obj, "to_vec1", 0));
}

static Moon::Vector1
mmrb_vector1_extract_args(mrb_state *mrb, int argc, mrb_value *vals)
{
  Moon::Vector1 result;
  switch (argc) {
  case 1:
    mrb_value val;
    val = vals[0];
    switch (mrb_type(val)) {
    case MRB_TT_FIXNUM:
    case MRB_TT_FLOAT:
      result = mmrb_vector1_extract_mrb_num(mrb, val);
      break;
    case MRB_TT_ARRAY:
      result = mmrb_vector1_extract_mrb_array(mrb, val);
      break;
    case MRB_TT_DATA:
      if (DATA_TYPE(val) == &vector1_data_type) {
        result = get_vector1_value(mrb, val);
        break;
      }
    default:
      if (mrb_respond_to(mrb, val, mrb_intern_cstr(mrb, "to_vec1"))) {
        result = mmrb_vector1_extract_mrb_to_vec1(mrb, val);
        break;
      } else {
        mrb_raisef(mrb, E_TYPE_ERROR,
                   "wrong type %S (expected Numeric, Array or Vector1)",
                   mrb_str_new_cstr(mrb, mrb_obj_classname(mrb, val)));
      }
    }
    break;
  default:
    mrb_raisef(mrb, E_ARGUMENT_ERROR,
               "wrong number of arguments (%d for 1)", argc);
  }
  return result;
}

static Moon::Vector1
vector1_from_mrb_args(mrb_state *mrb)
{
  mrb_value *vals;
  int len;
  mrb_get_args(mrb, "*", &vals, &len);
  return mmrb_vector1_extract_args(mrb, len, vals);
}

MOON_C_API Moon::Vector1
mmrb_to_vector1(mrb_state *mrb, mrb_value obj)
{
  return mmrb_vector1_extract_args(mrb, 1, &obj);
}

MOON_C_API mrb_value
mmrb_vector1_value(mrb_state *mrb, Moon::Vector1 vec)
{
  return set_vector1(mrb, new_vector1(mrb), vec);
}

static mrb_value
vector1_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_float x = 0.0;
  mrb_get_args(mrb, "|f", &x);
  cleanup_vector1(mrb, self);
  mrb_data_init(self, new Moon::Vector1(x), &vector1_data_type);
  return self;
}

static mrb_value
vector1_initialize_copy(mrb_state *mrb, mrb_value self)
{
  cleanup_vector1(mrb, self);
  mrb_data_init(self, new Moon::Vector1(vector1_from_mrb_args(mrb)), &vector1_data_type);
  return self;
}

static mrb_value
vector1_coerce(mrb_state *mrb, mrb_value self)
{
  mrb_value other;
  mrb_get_args(mrb, "o", &other);
  mrb_value argv[2] = { self, other };
  return mrb_ary_new_from_values(mrb, 2, argv);
}

/*
 * @return [Boolean]
 */
static mrb_value
vector1_eq(mrb_state *mrb, mrb_value self)
{
  mrb_value other;
  mrb_get_args(mrb, "o", &other);
  if (mrb_obj_is_kind_of(mrb, other, mmrb_get_vector1_class(mrb))) {
    return mrb_bool_value((*mmrb_vector1_ptr(mrb, self)) == (*mmrb_vector1_ptr(mrb, other)));
  }
  return mrb_bool_value(false);
}

static mrb_value
vector1_x_getter(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, mmrb_vector1_ptr(mrb, self)->x);
}

static mrb_value
vector1_x_setter(mrb_state *mrb, mrb_value self)
{
  mrb_float x;
  mrb_get_args(mrb, "f", &x);
  mmrb_vector1_ptr(mrb, self)->x = x;
  return mrb_nil_value();
}

static mrb_value
vector1_negate(mrb_state *mrb, mrb_value self)
{
  return mmrb_vector1_value(mrb, -get_vector1_value(mrb, self));
}

static mrb_value
vector1_identity(mrb_state *mrb, mrb_value self)
{
  return mrb_obj_dup(mrb, self);
}

static mrb_value
vector1_add(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(+);
}

static mrb_value
vector1_sub(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(-);
}

static mrb_value
vector1_mul(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(*);
}

static mrb_value
vector1_div(mrb_state *mrb, mrb_value self)
{
  m_vector_operator(/);
}

static mrb_value
vector1_not(mrb_state *mrb, mrb_value self)
{
  return mmrb_vector1_value(mrb, Moon::Vector1(~(Moon::Vector1i(*mmrb_vector1_ptr(mrb, self)))));
}

static mrb_value
vector1_modulo(mrb_state *mrb, mrb_value self)
{
  m_vector_int_operator(%);
}

static mrb_value
vector1_shl(mrb_state *mrb, mrb_value self)
{
  m_vector_int_operator(<<);
}

static mrb_value
vector1_shr(mrb_state *mrb, mrb_value self)
{
  m_vector_int_operator(>>);
}

static mrb_value
vector1_and(mrb_state *mrb, mrb_value self)
{
  m_vector_int_operator(&);
}

static mrb_value
vector1_or(mrb_state *mrb, mrb_value self)
{
  m_vector_int_operator(|);
}

static mrb_value
vector1_xor(mrb_state *mrb, mrb_value self)
{
  m_vector_int_operator(^);
}

static mrb_value
vector1_dot(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, glm::dot(get_vector1_value(mrb, self), vector1_from_mrb_args(mrb)));
}

static mrb_value
vector1_normalize(mrb_state *mrb, mrb_value self)
{
  return mmrb_vector1_value(mrb, glm::normalize(get_vector1_value(mrb, self)));
}

static mrb_value
vector1_length(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, glm::length(get_vector1_value(mrb, self)));
}

static mrb_value
vector1_distance(mrb_state *mrb, mrb_value self)
{
  Moon::Vector1 diff = get_vector1_value(mrb, self) - vector1_from_mrb_args(mrb);
  return mrb_float_value(mrb, glm::dot(diff, diff));
}

static mrb_value
vector1_lerp(mrb_state *mrb, mrb_value self)
{
  Moon::Vector1 *other;
  mrb_float delta;
  mrb_get_args(mrb, "df", &other, &vector1_data_type, &delta);
  return mmrb_vector1_value(mrb, Moon::Vector1(glm::lerp(mmrb_vector1_ptr(mrb, self)->x, other->x, (float)delta)));
}

static mrb_value
vector1_set(mrb_state *mrb, mrb_value self)
{
  return set_vector1(mrb, self, vector1_from_mrb_args(mrb));
}

static mrb_value
vector1_to_int(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value((mrb_int)mmrb_vector1_ptr(mrb, self)->x);
}

static mrb_value
vector1_to_f(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, (mrb_float)mmrb_vector1_ptr(mrb, self)->x);
}

static mrb_value
vector1_to_a(mrb_state *mrb, mrb_value self)
{
  mrb_value argv[1] = { mrb_float_value(mrb, mmrb_vector1_ptr(mrb, self)->x) };
  return mrb_ary_new_from_values(mrb, 1, argv);
}

static mrb_value
vector1_s_extract(mrb_state *mrb, mrb_value self)
{
  Moon::Vector1 src_vect = vector1_from_mrb_args(mrb);
  mrb_value argv[1] = { mrb_float_value(mrb, src_vect.x) };
  return mrb_ary_new_from_values(mrb, 1, argv);
}

static mrb_value
vector1_s_cast(mrb_state *mrb, mrb_value klass)
{
  return set_vector1(mrb, new_vector1(mrb), vector1_from_mrb_args(mrb));
}

MOON_C_API void
mmrb_vector1_init(mrb_state *mrb, struct RClass *mod)
{
  struct RClass *vector1_class = mrb_define_class_under(mrb, mod, "Vector1", mrb->object_class);
  MRB_SET_INSTANCE_TT(vector1_class, MRB_TT_DATA);

  mrb_define_method(mrb, vector1_class, "initialize",      vector1_initialize,      MRB_ARGS_OPT(1));
  mrb_define_method(mrb, vector1_class, "initialize_copy", vector1_initialize_copy, MRB_ARGS_REQ(1));
  /* coercion */
  mrb_define_method(mrb, vector1_class, "coerce",          vector1_coerce,          MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "==",              vector1_eq,              MRB_ARGS_REQ(1));
  /* attribute setters */
  mrb_define_method(mrb, vector1_class, "x",               vector1_x_getter,        MRB_ARGS_NONE());
  mrb_define_method(mrb, vector1_class, "x=",              vector1_x_setter,        MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "set",             vector1_set,             MRB_ARGS_ANY());
  /* arithmetic */
  mrb_define_method(mrb, vector1_class, "-@",              vector1_negate,          MRB_ARGS_NONE());
  mrb_define_method(mrb, vector1_class, "+@",              vector1_identity,        MRB_ARGS_NONE());
  mrb_define_method(mrb, vector1_class, "+",               vector1_add,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "-",               vector1_sub,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "*",               vector1_mul,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "/",               vector1_div,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "dot",             vector1_dot,             MRB_ARGS_REQ(1));
  /* */
  mrb_define_method(mrb, vector1_class, "normalize",       vector1_normalize,       MRB_ARGS_NONE());
  mrb_define_method(mrb, vector1_class, "length",          vector1_length,          MRB_ARGS_NONE());
  mrb_define_method(mrb, vector1_class, "distance",        vector1_distance,        MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "lerp",            vector1_lerp,            MRB_ARGS_REQ(2));
  /* bitwise operators */
  mrb_define_method(mrb, vector1_class, "~@",              vector1_not,             MRB_ARGS_NONE());
  mrb_define_method(mrb, vector1_class, "%",               vector1_modulo,          MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "<<",              vector1_shl,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, ">>",              vector1_shr,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "&",               vector1_and,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "|",               vector1_or,              MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector1_class, "^",               vector1_xor,             MRB_ARGS_REQ(1));
  /* conversion */
  mrb_define_method(mrb, vector1_class, "to_int",          vector1_to_int,          MRB_ARGS_NONE());
  mrb_define_method(mrb, vector1_class, "to_i",            vector1_to_int,          MRB_ARGS_NONE());
  mrb_define_method(mrb, vector1_class, "to_f",            vector1_to_f,            MRB_ARGS_NONE());
  mrb_define_method(mrb, vector1_class, "to_a",            vector1_to_a,            MRB_ARGS_NONE());
  /* cast */
  mrb_define_class_method(mrb, vector1_class, "[]",        vector1_s_cast,          MRB_ARGS_ANY());
  mrb_define_class_method(mrb, vector1_class, "extract",   vector1_s_extract,       MRB_ARGS_REQ(1));
}
