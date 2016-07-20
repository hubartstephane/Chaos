#ifndef CHAOS_DEFERREDCALLABLE_H
#define CHAOS_DEFERREDCALLABLE_H

// ==============================================================================================
// DeferredCallable framework is a way to encode a function call with its arguments into a buffer
// This permits to record some actions into a buffer, actions that will be called later 
// This deserve to be a fast framework so we do not use virtual destructor by default
// Arguments of such function should so be POD
// ==============================================================================================

#include <cstddef>
#include <assert.h>

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

namespace chaos
{

#define CHAOS_DEFERRABLE_CALLABLE_SIZE 15

// ==============================================================================================
// DeferredCallableBase : no more than a pointer of function
// It has no virtual destructor for efficiency
// ==============================================================================================
class DeferredCallableBase
{
public:
  inline size_t Call(){ return callfunc(this); } // returns the size of the full object (child class of DeferredCallableBase)
protected:
  size_t (*callfunc)(DeferredCallableBase * src);
};

// ==============================================================================================
// DeferredCallable0 to DeferredCallable15
// ==============================================================================================

#define CHAOS_DEFERRABLE_CALLABLE_members(z, n, unused)\
    BOOST_PP_CAT(P,n) BOOST_PP_CAT(param, n);

#define CHAOS_DEFERRABLE_CALLABLE_construct_members(z, n, unused)\
    BOOST_PP_COMMA_IF(n) BOOST_PP_CAT(param,n) (BOOST_PP_CAT(p, n))

#define CHAOS_DEFERRABLE_CALLABLE_class(z, n, unused)\
template<typename RET BOOST_PP_ENUM_TRAILING_PARAMS(n, typename P)>\
class BOOST_PP_CAT(DeferredCallable, n) : public DeferredCallableBase\
{\
public:\
  typedef BOOST_PP_CAT(DeferredCallable, n)<RET BOOST_PP_ENUM_TRAILING_PARAMS(n, P)> MyType;\
  BOOST_PP_CAT(DeferredCallable, n)(RET (*in_func)(BOOST_PP_ENUM_PARAMS(n, P)) BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, P, p)) :\
  func(in_func) BOOST_PP_COMMA_IF(n) BOOST_PP_REPEAT(n, CHAOS_DEFERRABLE_CALLABLE_construct_members, ~)\
  {\
    callfunc = DoCall;\
  }\
protected:\
  static size_t DoCall(DeferredCallableBase * src)\
  {\
    assert(src);\
    MyType * pThis = (MyType *)src;\
    pThis->func(BOOST_PP_ENUM_PARAMS(n, pThis->param));\
    return sizeof(MyType);\
  }\
  RET (*func)(BOOST_PP_ENUM_PARAMS(n, P));\
  BOOST_PP_REPEAT(n, CHAOS_DEFERRABLE_CALLABLE_members, ~)\
};

BOOST_PP_REPEAT(CHAOS_DEFERRABLE_CALLABLE_SIZE, CHAOS_DEFERRABLE_CALLABLE_class, ~)

// ==============================================================================================
// Function MakeCallable a function to make type erasure
// ==============================================================================================

#define CHAOS_MAKE_CALLABLE_func(z, n, unused)\
template<typename RET BOOST_PP_ENUM_TRAILING_PARAMS(n, typename P) BOOST_PP_ENUM_TRAILING_PARAMS(n, typename PP)>\
BOOST_PP_CAT(DeferredCallable, n)<RET BOOST_PP_ENUM_TRAILING_PARAMS(n, P)> MakeCallable(\
  RET (*in_func)(BOOST_PP_ENUM_PARAMS(n, P)) BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, PP, pp))\
{\
  return BOOST_PP_CAT(DeferredCallable, n)<RET BOOST_PP_ENUM_TRAILING_PARAMS(n, P)>(in_func BOOST_PP_ENUM_TRAILING_PARAMS(n, pp));\
}
BOOST_PP_REPEAT(CHAOS_DEFERRABLE_CALLABLE_SIZE, CHAOS_MAKE_CALLABLE_func, ~)

// ==============================================================================================
// Examples
// ==============================================================================================
//
//    void f()
//    {
//      std::cout << "I m in f()" << std::endl;
//    }
//
//    void f(double k)
//    {
//      std::cout << "I m in f(double) : " << k << std::endl;
//    }
//
//    ...
//
//    MakeCallable(&f).Call();
//    MakeCallable(&f, 666.6).Call();
//
//    Note: 
//      -this accepts functions with same names
//      -there is a cast from double 666.6 into float
// ==============================================================================================


// ==============================================================================================
// DeferredObjectCallableBase : no more than a pointer of method
// It has no virtual destructor for efficiency
// ==============================================================================================
template<typename OBJ>
class DeferredObjectCallableBase
{
public:
  inline size_t Call(OBJ * target){ return callfunc(target, this); } // returns the size of the full object (child class of DeferredCallableBase)
  inline size_t Call(OBJ & target){ return callfunc(&target, this); } // returns the size of the full object (child class of DeferredCallableBase)
protected:
  size_t (*callfunc)(OBJ * target, DeferredObjectCallableBase<OBJ> * src);
};


// ==============================================================================================
// DeferredObjectCallable0 to DeferredObjectCallable15
// ==============================================================================================

#define CHAOS_DEFERRABLE_OBJECT_CALLABLE_class(z, n, unused)\
template<typename OBJ, typename RET BOOST_PP_ENUM_TRAILING_PARAMS(n, typename P)>\
class BOOST_PP_CAT(DeferredObjectCallable, n) : public DeferredObjectCallableBase<OBJ>\
{\
public:\
  typedef BOOST_PP_CAT(DeferredObjectCallable, n)<OBJ, RET BOOST_PP_ENUM_TRAILING_PARAMS(n, P)> MyType;\
  BOOST_PP_CAT(DeferredObjectCallable, n)(RET (OBJ::*in_func)(BOOST_PP_ENUM_PARAMS(n, P)) BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, P, p)) :\
  func(in_func) BOOST_PP_COMMA_IF(n) BOOST_PP_REPEAT(n, CHAOS_DEFERRABLE_CALLABLE_construct_members, ~)\
  {\
    callfunc = DoCall;\
  }\
protected:\
  static size_t DoCall(OBJ * target, DeferredObjectCallableBase<OBJ> * src)\
  {\
    assert(target != NULL);\
    assert(src);\
    MyType * pThis = (MyType *)src;\
    (target->*(pThis->func))(BOOST_PP_ENUM_PARAMS(n, pThis->param));\
    return sizeof(MyType);\
  }\
  RET (OBJ::*func)(BOOST_PP_ENUM_PARAMS(n, P));\
  BOOST_PP_REPEAT(n, CHAOS_DEFERRABLE_CALLABLE_members, ~)\
};

BOOST_PP_REPEAT(CHAOS_DEFERRABLE_CALLABLE_SIZE, CHAOS_DEFERRABLE_OBJECT_CALLABLE_class, ~)

// ==============================================================================================
// Function MakeCallable a function to make type erasure
// ==============================================================================================

#define CHAOS_MAKE_OBJECT_CALLABLE_func(z, n, unused)\
template<typename OBJ, typename RET BOOST_PP_ENUM_TRAILING_PARAMS(n, typename P) BOOST_PP_ENUM_TRAILING_PARAMS(n, typename PP)>\
BOOST_PP_CAT(DeferredObjectCallable, n)<OBJ, RET BOOST_PP_ENUM_TRAILING_PARAMS(n, P)> MakeObjectCallable(\
  RET (OBJ::*in_func)(BOOST_PP_ENUM_PARAMS(n, P)) BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, PP, pp))\
{\
  return BOOST_PP_CAT(DeferredObjectCallable, n)<OBJ, RET BOOST_PP_ENUM_TRAILING_PARAMS(n, P)>(in_func BOOST_PP_ENUM_TRAILING_PARAMS(n, pp));\
}
BOOST_PP_REPEAT(CHAOS_DEFERRABLE_CALLABLE_SIZE, CHAOS_MAKE_OBJECT_CALLABLE_func, ~)

// ==============================================================================================
// Examples
// ==============================================================================================
//
//   class A
//   {
//      public:
//      void f()
//      {
//        std::cout << "I m in A::f()" << std::endl;
//      }
//
//      void f(double k)
//      {
//        std::cout << "I m in A::f(double) : " << k << std::endl;
//      }
//    };
//
//    ...
//
//    A * a = new A;
//    MakeObjectCallable(&A::f).Call(a);
//    MakeObjectCallable(&A::f, 666.6).Call(a);
//
// ==============================================================================================

}; // namespace chaos

#endif // #ifndef CHAOS_DEFERREDCALLABLE_H