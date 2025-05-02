#include <refl.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

/*#define DECLARE_STRUCT(Namespace, StructName, MEMBERS) \
namespace Namespace { \
struct StructName { \
MEMBERS(DECLARE_MEMBER) \
}; \
} \
REFL_TYPE(Namespace::StructName) \
MEMBERS(REFL_MEMBER) \
REFL_END

#define DECLARE_MEMBER(type, name) type name;
#define REFL_MEMBER(type, name) REFL_FIELD(name);

#define MY_STRUCT_MEMBERS(M) \
M(int, id) \
M(float, value) \
M(std::string, name)
DECLARE_STRUCT(MyNS, MyStruct, MY_STRUCT_MEMBERS)*/

#define APPLY_TO_ALL_1(M, a) M a
#define APPLY_TO_ALL_2(M, a, ...) M a APPLY_TO_ALL_1(M, __VA_ARGS__)
#define APPLY_TO_ALL_3(M, a, ...) M a APPLY_TO_ALL_2(M, __VA_ARGS__)
#define APPLY_TO_ALL_4(M, a, ...) M a APPLY_TO_ALL_3(M, __VA_ARGS__)
#define APPLY_TO_ALL_5(M, a, ...) M a APPLY_TO_ALL_4(M, __VA_ARGS__)
#define APPLY_TO_ALL_6(M, a, ...) M a APPLY_TO_ALL_5(M, __VA_ARGS__)
#define APPLY_TO_ALL_7(M, a, ...) M a APPLY_TO_ALL_6(M, __VA_ARGS__)
#define APPLY_TO_ALL_8(M, a, ...) M a APPLY_TO_ALL_7(M, __VA_ARGS__)

#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,NAME,...) NAME
#define APPLY_TO_ALL(M, ...) \
GET_MACRO(__VA_ARGS__, \
APPLY_TO_ALL_8, \
APPLY_TO_ALL_7, \
APPLY_TO_ALL_6, \
APPLY_TO_ALL_5, \
APPLY_TO_ALL_4, \
APPLY_TO_ALL_3, \
APPLY_TO_ALL_2, \
APPLY_TO_ALL_1)(M, __VA_ARGS__)

#define DECLARE_STRUCT(Namespace, StructName, ...) \
namespace Namespace { \
struct StructName { \
EXPAND_MEMBERS(__VA_ARGS__) \
}; \
} \
REFL_TYPE(Namespace::StructName) \
REFLECT_MEMBERS(__VA_ARGS__) \
REFL_END

#define EXPAND_MEMBERS(...) APPLY_TO_ALL(DECLARE_MEMBER, __VA_ARGS__)
#define REFLECT_MEMBERS(...) APPLY_TO_ALL(REFL_MEMBER, __VA_ARGS__)

#define DECLARE_MEMBER(type, name) type name;
#define REFL_MEMBER(type, name) REFL_FIELD(name);

DECLARE_STRUCT(MyNS, MyStruct,
    (int, id),
    (float, value),
    (std::string, name)
)

TEST(DefTests, structDef)
{
    MyNS::MyStruct s;
    s.name = "test";
}