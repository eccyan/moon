/*
 * Moon Vector3, a wrapper around glm::vec3
 */
#include "moon/mrb/vector3.hxx"
#include "moon/mrb/vector2.hxx"
#include <glm/glm.hpp>
#include "moon/mrb/shared_types.hxx"

static struct RClass *vector3_class = NULL;

#define mrb_set_vector3_value_xyz(mrb, target, x, y, z)         \
  {                                                             \
    moon_vec3 *vector3 = new moon_vec3(new glm::vec3(x, y, z)); \
    DATA_TYPE(target) = &vector3_data_type;                     \
    DATA_PTR(target) = vector3;                                 \
  }

#define vec3_math_head(_func_)                               \
  moon_vec3* src_vec3;                                       \
  Data_Get_Struct(mrb, self, &vector3_data_type, src_vec3);  \
  glm::vec3 oth_vec3 = mmrb_vector3_extract_mrb_args(mrb);   \
  _func_

#define vec3_math_op(_op_)                                  \
  glm::vec3 oth_vec3 = mmrb_vector3_extract_mrb_args(mrb);  \
  mrb_value dest_vec = mrb_obj_dup(mrb, self);              \
  **((moon_vec3*)DATA_PTR(dest_vec)) _op_ ## = oth_vec3;    \
  return dest_vec;

static void
vector3_free(mrb_state *mrb, void *p)
{
  moon_vec3 *vec3 = (moon_vec3*)p;
  if (vec3) {
    delete(vec3);
  }
}

struct mrb_data_type vector3_data_type = { "Vector3", vector3_free };

/*
 * Black Magic
 */
static glm::vec3
mmrb_vector3_extract_mrb_vec3(mrb_state *mrb, mrb_value obj)
{
  moon_vec3* vec3;
  Data_Get_Struct(mrb, obj, &vector3_data_type, vec3);
  return **vec3;
}

static glm::vec3
mmrb_vector3_extract_mrb_num(mrb_state *mrb, mrb_value obj)
{
  double i = mrb_to_flo(mrb, obj);
  return glm::vec3(i, i, i);
}

static glm::vec3
mmrb_vector3_extract_mrb_array(mrb_state *mrb, mrb_value obj)
{
  glm::vec3 result;
  int _ary_len = mrb_ary_len(mrb, obj);
  if (_ary_len != 3) {
    mrb_raisef(mrb, E_ARGUMENT_ERROR,
               "wrong array size %d (expected 3)", _ary_len);
  } else {
    result.x = mrb_to_flo(mrb, RARRAY_PTR(obj)[0]);
    result.y = mrb_to_flo(mrb, RARRAY_PTR(obj)[1]);
    result.z = mrb_to_flo(mrb, RARRAY_PTR(obj)[2]);
  }
  return result;
}

static glm::vec3
mmrb_vector3_extract_mrb_to_vec3(mrb_state *mrb, mrb_value obj)
{
  return mmrb_vector3_extract_mrb_vec3(mrb, mrb_funcall(mrb, obj, "to_vec3", 0));
}

glm::vec3
mmrb_vector3_extract_args(mrb_state *mrb, int argc, mrb_value *vals)
{
  glm::vec3 result;

  switch (argc) {
  case 1:
    mrb_value val;
    val = vals[0];
    switch (mrb_type(val)) {
    case MRB_TT_FIXNUM:
    case MRB_TT_FLOAT:
      result = mmrb_vector3_extract_mrb_num(mrb, val);
      break;
    case MRB_TT_ARRAY:
      result = mmrb_vector3_extract_mrb_array(mrb, val);
      break;
    case MRB_TT_DATA:
      if (DATA_TYPE(val) == &vector3_data_type) {
        result = mmrb_vector3_extract_mrb_vec3(mrb, val);
        break;
      }
    default:
      if (mrb_respond_to(mrb, val, mrb_intern_cstr(mrb, "to_vec3"))) {
        result = mmrb_vector3_extract_mrb_to_vec3(mrb, val);
        break;
      } else {
        mrb_raisef(mrb, E_TYPE_ERROR,
                   "wrong type %S (expected Numeric, Array or Vector3)",
                   mrb_str_new_cstr(mrb, mrb_obj_classname(mrb, val)));
      }
    }
    break;
  case 2:
    int index;
    index = 0;
    for (int i=0; i < 2; ++i) {
      mrb_value val = vals[i];

      if (mrb_type(val) == MRB_TT_DATA) {
        if (DATA_TYPE(val) == &vector2_data_type) {
          moon_vec2* vec2;
          Data_Get_Struct(mrb, val, &vector2_data_type, vec2);

          result[index++] = (**vec2)[0]; if (index >= 3) break;
          result[index++] = (**vec2)[1];
        } else {
          mrb_raisef(mrb, E_TYPE_ERROR,
                     "wrong type %S (expected Vector2)",
                     mrb_str_new_cstr(mrb, mrb_obj_classname(mrb, val)));
        }
      } else if ((mrb_type(val) == MRB_TT_FIXNUM) || (mrb_type(val) == MRB_TT_FLOAT)) {
        result[index++] = mrb_to_flo(mrb, val);
      }
      if (index >= 3) break;
    };
    if (index < 3) {
      mrb_raisef(mrb, E_ARGUMENT_ERROR, "not enough parameters (required 3)");
    }
    break;
  case 3:
    result.x = mrb_to_flo(mrb, vals[0]);
    result.y = mrb_to_flo(mrb, vals[1]);
    result.z = mrb_to_flo(mrb, vals[2]);
    break;
  default:
    mrb_raisef(mrb, E_ARGUMENT_ERROR,
               //"wrong number of arguments (%d for 1 or 3)", argc);
               "wrong number of arguments (%d for 1, 2, or 3)", argc);
  }

  return result;
}

static glm::vec3
mmrb_vector3_extract_mrb_args(mrb_state *mrb)
{
  mrb_value *vals;
  int len;

  mrb_get_args(mrb, "*", &vals, &len);
  return mmrb_vector3_extract_args(mrb, len, vals);
}

static mrb_value
vector3_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_float x = 0.0;
  mrb_float y = 0.0;
  mrb_float z = 0.0;
  moon_vec3 *vec3;
  mrb_get_args(mrb, "|fff", &x, &y, &z);

  vec3 = (moon_vec3*)DATA_PTR(self);
  if (vec3) {
    vector3_free(mrb, (void*)vec3);
  }

  mrb_set_vector3_value_xyz(mrb, self, x, y, z);

  return self;
};

static mrb_value
vector3_initialize_copy(mrb_state *mrb, mrb_value self)
{
  mrb_value other;
  moon_vec3* src_vec3;

  mrb_get_args(mrb, "o", &other);

  Data_Get_Struct(mrb, other, &vector3_data_type, src_vec3);

  mrb_set_vector3_value_xyz(mrb, self,
                            (*src_vec3)->x,
                            (*src_vec3)->y,
                            (*src_vec3)->z);

  return self;
}

static mrb_value
vector3_coerce(mrb_state *mrb, mrb_value self)
{
  mrb_value other;
  mrb_get_args(mrb, "o", &other);
  mrb_value argv[2] = { self, other };
  return mrb_ary_new_from_values(mrb, 2, argv);
}

static mrb_value
vector3_x_getter(mrb_state *mrb, mrb_value self)
{
  moon_vec3* vec3;
  Data_Get_Struct(mrb, self, &vector3_data_type, vec3);

  return mrb_float_value(mrb, (*vec3)->x);
}

static mrb_value
vector3_y_getter(mrb_state *mrb, mrb_value self)
{
  moon_vec3* vec3;
  Data_Get_Struct(mrb, self, &vector3_data_type, vec3);

  return mrb_float_value(mrb, (*vec3)->y);
}

static mrb_value
vector3_z_getter(mrb_state *mrb, mrb_value self)
{
  moon_vec3* vec3;
  Data_Get_Struct(mrb, self, &vector3_data_type, vec3);

  return mrb_float_value(mrb, (*vec3)->z);
}

static mrb_value
vector3_x_setter(mrb_state *mrb, mrb_value self)
{
  moon_vec3* vec3;
  Data_Get_Struct(mrb, self, &vector3_data_type, vec3);

  mrb_float x;
  mrb_get_args(mrb, "f", &x);

  (*vec3)->x = x;

  return mrb_nil_value();
}

static mrb_value
vector3_y_setter(mrb_state *mrb, mrb_value self)
{
  moon_vec3* vec3;
  Data_Get_Struct(mrb, self, &vector3_data_type, vec3);

  mrb_float y;
  mrb_get_args(mrb, "f", &y);

  (*vec3)->y = y;

  return mrb_nil_value();
}

static mrb_value
vector3_z_setter(mrb_state *mrb, mrb_value self)
{
  moon_vec3* vec3;
  Data_Get_Struct(mrb, self, &vector3_data_type, vec3);

  mrb_float z;
  mrb_get_args(mrb, "f", &z);

  (*vec3)->z = z;

  return mrb_nil_value();
}

static mrb_value
vector3_negate(mrb_state *mrb, mrb_value self)
{
  moon_vec3* dvec3;
  moon_vec3* svec3;

  //mrb_value dest_vec = mrb_obj_new(mrb, vector3_class, 0, {});
  mrb_value dest_vec = mrb_obj_dup(mrb, self);

  Data_Get_Struct(mrb, dest_vec, &vector3_data_type, dvec3);
  Data_Get_Struct(mrb, self, &vector3_data_type, svec3);

  (**dvec3) = -(**svec3);

  return dest_vec;
}

static mrb_value
vector3_identity(mrb_state *mrb, mrb_value self)
{
  return mrb_obj_dup(mrb, self);
}

static mrb_value
vector3_add(mrb_state *mrb, mrb_value self)
{
  vec3_math_op(+)
}

static mrb_value
vector3_sub(mrb_state *mrb, mrb_value self)
{
  vec3_math_op(-)
}

static mrb_value
vector3_mul(mrb_state *mrb, mrb_value self)
{
  vec3_math_op(*)
}

static mrb_value
vector3_div(mrb_state *mrb, mrb_value self)
{
  vec3_math_op(/)
}

//static mrb_value //vector3_mod(mrb_state *mrb, mrb_value self) {
//  vec3_math_op(%)
//};

static mrb_value
vector3_dot(mrb_state *mrb, mrb_value self)
{
  vec3_math_head({
    return mrb_float_value(mrb, glm::dot(**src_vec3, oth_vec3));
  })
}

static mrb_value
vector3_cross(mrb_state *mrb, mrb_value self)
{
  vec3_math_head({
    //mrb_value dest_vec = mrb_obj_new(mrb, vector3_class, 0, {});
    mrb_value dest_vec = mrb_obj_dup(mrb, self);
    **((moon_vec3*)DATA_PTR(dest_vec)) = glm::cross(**src_vec3, oth_vec3);
    return dest_vec;
  })
}

static mrb_value
vector3_set(mrb_state *mrb, mrb_value self)
{
  glm::vec3 v3a = mmrb_vector3_extract_mrb_args(mrb);

  moon_vec3* mvec3;
  Data_Get_Struct(mrb, self, &vector3_data_type, mvec3);

  **mvec3 = v3a;

  return self;
}

static mrb_value
vector3_to_a(mrb_state *mrb, mrb_value self)
{
  moon_vec3* mvec3;
  Data_Get_Struct(mrb, self, &vector3_data_type, mvec3);
  mrb_value argv[3] = { mrb_float_value(mrb, (*mvec3)->x),
                        mrb_float_value(mrb, (*mvec3)->y),
                        mrb_float_value(mrb, (*mvec3)->z) };
  return mrb_ary_new_from_values(mrb, 3, argv);
}

static mrb_value
vector3_s_extract(mrb_state *mrb, mrb_value self)
{
  glm::vec3 v3a = mmrb_vector3_extract_mrb_args(mrb);

  mrb_value argv[3] = { mrb_float_value(mrb, v3a.x),
                        mrb_float_value(mrb, v3a.y),
                        mrb_float_value(mrb, v3a.z) };
  return mrb_ary_new_from_values(mrb, 3, argv);
}

static mrb_value
vector3_s_cast(mrb_state *mrb, mrb_value klass)
{
  glm::vec3 v3a = mmrb_vector3_extract_mrb_args(mrb);

  mrb_value dest_vec = mrb_obj_new(mrb, vector3_class, 0, {});

  moon_vec3* _vec3;
  Data_Get_Struct(mrb, dest_vec, &vector3_data_type, _vec3);

  **_vec3 = v3a;

  return dest_vec;
}

struct RClass*
mmrb_vector3_init(mrb_state *mrb)
{
  vector3_class = mrb_define_class_under(mrb, mmrb_Moon, "Vector3", mrb->object_class);
  MRB_SET_INSTANCE_TT(vector3_class, MRB_TT_DATA);

  mrb_define_method(mrb, vector3_class, "initialize",      vector3_initialize,      MRB_ARGS_OPT(3));
  mrb_define_method(mrb, vector3_class, "initialize_copy", vector3_initialize_copy, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, vector3_class, "coerce",          vector3_coerce,          MRB_ARGS_REQ(1));

  mrb_define_method(mrb, vector3_class, "x",               vector3_x_getter,        MRB_ARGS_NONE());
  mrb_define_method(mrb, vector3_class, "y",               vector3_y_getter,        MRB_ARGS_NONE());
  mrb_define_method(mrb, vector3_class, "z",               vector3_z_getter,        MRB_ARGS_NONE());
  mrb_define_method(mrb, vector3_class, "x=",              vector3_x_setter,        MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector3_class, "y=",              vector3_y_setter,        MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector3_class, "z=",              vector3_z_setter,        MRB_ARGS_REQ(1));

  mrb_define_method(mrb, vector3_class, "-@",              vector3_negate,          MRB_ARGS_NONE());
  mrb_define_method(mrb, vector3_class, "+@",              vector3_identity,        MRB_ARGS_NONE());

  mrb_define_method(mrb, vector3_class, "+",               vector3_add,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector3_class, "-",               vector3_sub,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector3_class, "*",               vector3_mul,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector3_class, "/",               vector3_div,             MRB_ARGS_REQ(1));
  //mrb_define_method(mrb, vector3_class, "%",               vector3_mod,             MRB_ARGS_REQ(1));

  mrb_define_method(mrb, vector3_class, "dot",             vector3_dot,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vector3_class, "cross",           vector3_cross,           MRB_ARGS_REQ(1));

  mrb_define_method(mrb, vector3_class, "set",             vector3_set,             MRB_ARGS_ANY());

  mrb_define_method(mrb, vector3_class, "to_a",            vector3_to_a,            MRB_ARGS_NONE());

  mrb_define_class_method(mrb, vector3_class, "[]",        vector3_s_cast,          MRB_ARGS_ANY());
  mrb_define_class_method(mrb, vector3_class, "extract",   vector3_s_extract,       MRB_ARGS_REQ(1));

  mrb_alias_method(mrb, vector3_class, mrb_intern_cstr(mrb, "r"), mrb_intern_cstr(mrb, "x"));
  mrb_alias_method(mrb, vector3_class, mrb_intern_cstr(mrb, "g"), mrb_intern_cstr(mrb, "y"));
  mrb_alias_method(mrb, vector3_class, mrb_intern_cstr(mrb, "b"), mrb_intern_cstr(mrb, "z"));

  mrb_alias_method(mrb, vector3_class, mrb_intern_cstr(mrb, "r="), mrb_intern_cstr(mrb, "x="));
  mrb_alias_method(mrb, vector3_class, mrb_intern_cstr(mrb, "g="), mrb_intern_cstr(mrb, "y="));
  mrb_alias_method(mrb, vector3_class, mrb_intern_cstr(mrb, "b="), mrb_intern_cstr(mrb, "z="));
  return vector3_class;
}

mrb_value
mmrb_vector3_wrap(mrb_state *mrb, glm::vec3 *ptr)
{
  moon_vec3 *vec3_ptr = new moon_vec3(ptr);
  mrb_value rvec3 = mrb_obj_value(Data_Wrap_Struct(mrb, vector3_class, &vector3_data_type, vec3_ptr));
  return rvec3;
}

mrb_value
mmrb_vector3_wrap(mrb_state *mrb, moon_vec3 moonv)
{
  moon_vec3 *vec3_ptr = new moon_vec3(moonv);
  mrb_value rvec3 = mrb_obj_value(Data_Wrap_Struct(mrb, vector3_class, &vector3_data_type, vec3_ptr));
  return rvec3;
}

mrb_value
mmrb_vector3_create(mrb_state *mrb, double x, double y, double z)
{
  return mmrb_vector3_wrap(mrb, new glm::vec3(x, y, z));
}
