#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/LuaState.h>
#include <chaos/InheritanceIntrospection.h>

// ===================
// Notes on LuaBinding
// ===================
//
// Names
// -----
//
//  - MT   : metatable
//  - TOMT : table of metatables
//
// Binding classes and functions
// -----------------------------
//
// For binding a class in lua we have to define two things:
//
//   - A table  "MyClass" with a method New
//
//   - A MT with some functions
//      => __index
//      => __newindex
//      => __gc
//
//   (_index does some strcmp(...) to know what to put as a result on the stack (function, member ...)
//
//
//
// In a binding function :   int myfunction(lua_State *) => that calls in facts   RET orignfunction(ARG1, ARG2, ARG3)
//
// we have to test each types of arguments on the stack (userdata), make some casting ...
// Normally this is done by
//
//    lua_getmetatable(state, idx);         => get MT of userdata on the stack
//
//    luaL_getmetatable(state, "MyClass");  => get MT for a class that has been registered by luaL_newmetatable(...);
//
// If we use meta programming techniques, we don't know the "name" of the class searched 
// (except maybe with RTTI, but this is not always enabled in a project and that requires that target structure have a V-TABLE)
//
// So we cannot call : luaL_getmetatable(state, "MyClass");
//
//
//
// The solution is to use something else than a string to search for the MT !!
//
// If we have a unique ID for a given class this does the tricks !! Two solutions at least
//
//   -> use address of a unique function
//
//       template<typename T> 
//       void F()                       => The address of this function is unique for a given specialization
//       {
//       } 
//
//   -> use address of a singleton
//
//       template<typename T>
//       ANYOBJECT * GetSingletionObject()
//       {
//         static ANYOBJECT result;     => same remarks
//         return &result;
//       }
//
//  For any given class A,  
//    - &F<A>   
//    - &GetSingletonObject<A>()          => are both unique ID => say    Unique<T>() == some void *
//
//
// So what are we going to do ?
//
//   - do not use native functions anymore
//
//       luaL_getmetatable(... "MyClass");
//       luaL_newmetatable(... "MyClass");
//
//   - create a table in REGISTRY that will contains all MTs : TOMT
//     for a given class we can get the MT by
//
//       TOMT[ Unique<T>() ]
//
//
//
//
// Argument testing and inheritance:
// --------------------------------
//
// Take a class hierarchy chain :
//
//   class A : public B {};
//
// In a function that require an argument of type B, an argument of type A is good as well
//
// So, if we consider a binding functions:
//
//  int MyFuncBinding(lua_State * state)
//  {
//    arg_mt      = lua_getmetatable(index = -1);
//
//    expected_mt = TOMT[ Unique<B>() ]
//
//    ...
//  }
//
// => we cannot test    (arg_mt == expected_mt)      because if argument is an instance of A, this should work too
//
//
// SOLUTION :
// --------
//
//   - the unique key ID by class is given by the ANYOBJECT singleton address method
//
//   - ANYOBJECT could be any types (even integers, as soon as its address is unique)
//
//   - we defines ANYOBJECT has    std::pair<void *, void *>
//
//   - ID->first  == ID     (explained later => obsolete choice ?)
//
//   - ID->second == PARENT_ID
//
//   As soon as we have an ID, this is an address that give us access to PARENT_ID and so on ...
//   We can now know if two ID's comes from an inheritance chain.
//
//        obsolete XXX : why  ID.first == &ID ???
//
//              => because if we make the call         
//
//                   std::pair<void *, void *> copy_of_ID = UniqueID<A>()
//
//                 the address of ID is not relevant because it is a local copy of the singleton ID. that why we store the address
//                 of the singleton inside ID.first. We can now make any test we want
//
//  - when we call    arg_mt = lua_getmetatable(index = -1);
//
//    we get the MT of the ARGUMENT, not its ID
//
//    we can simply store the ID inside the MT with  lua_rawset/lua_rawget methods (...)
//
//
//
// XXX : while ANYOBJECT is a pointer, we can simply use it as a LIGHTUSERDATA. Indexation in a table becomes simple
//  


namespace chaos
{

/** 
 * LuaBinding a class to help generate binding between C++ and LUA
 */

class LuaBinding : public LuaState
{

protected:

  /** constant string of indexing the REGISTRY */
  static char const * TOMT_KEY;
  /** constant string of indexing the MT */
  static char const * ID_KEY;

public:

  /** initialization constructor */
  LuaBinding(lua_State * in_state) : LuaState(in_state){ assert(in_state != nullptr); }
  /** copy constructor */
  LuaBinding(LuaBinding const & other) : LuaState(other.state){}

  /** create the binding for a given function and register it into the table with given index */
  template<typename RET>
  void AddFunction(char const * name, RET (*func)(), int index = 0)
  {
    lua_pushstring(state, name);
    lua_pushcfunction(state, FunctionBinding(func));
    lua_settable(state, index);
  }




  /*
  template<typename BASE, typename PARENT>
  ClassBinding AddClass(char const * name, int index)
  {

  }
  */




protected:

  /** generate a binding function */
  template<typename RET>
  lua_CFunction FunctionBinding(RET (*func)())
  {

    return nullptr;
  }

protected:

  /** create (if necessary) and get (result on the stack) the TOMT */
  void GetTOMT();
  /** create an empty MT and register it in the TOMT that is on the stack (result is put on the stack */
  void CreateEmptyMT(InheritanceIntrospection::ClassRuntimeID ID, int tomt_index);
  /** get the MT from a ClassID : result put on the stack */
  void GetMT(InheritanceIntrospection::ClassRuntimeID ID);
  /** return the ClassID of a MT on the stack */
  InheritanceIntrospection::ClassRuntimeID GetClassRuntimeIDFromMT(int mt_index);
  /** return the ClassID of a user data on the stack */
  InheritanceIntrospection::ClassRuntimeID GetClassRuntimeID(int index);








};

}; // namespace chaos

