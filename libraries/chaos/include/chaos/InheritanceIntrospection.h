#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/**
 * InheritanceIntrospection : a class to handle chain of inheritance 
 * XXX : beware, this is not RTTI, we can only gets informations we have declared before
 */

class InheritanceIntrospection
{

  /** this does correspond to ClassRuntimeID whose const modifier has been removed (useful in DeclareInheritance(...) )*/
  typedef std::pair<void *, void *> * ModifiableClassRuntimeID;

public:

  /** 
   * A chain of inheritance can be provided by couple of pointer on ClassRuntimeID (base, parent)
   *   first  = pointer on himself
   *   second = pointer on parent class (must be manually set)
   */
  typedef std::pair<void *, void *> const * ClassRuntimeID;

  /** this function is used to get a node for inheritance chain (this that not initialize the second term of the chain) */
  template<typename T>
  static ClassRuntimeID GetClassRuntimeID()
  {
    static std::pair<void *, void *> result = std::make_pair((void*)&result, (void*)nullptr);
    return &result;
  }

  /** initialize inheritance chain (XXX : in real life, BASE as not to derives from PARENT !) */
  template<typename BASE, typename PARENT>
  static void DeclareInheritance()
  {
    ModifiableClassRuntimeID base   = (ModifiableClassRuntimeID)GetClassRuntimeID<BASE>();
    ModifiableClassRuntimeID parent = (ModifiableClassRuntimeID)GetClassRuntimeID<PARENT>(); // remove the const modifier

    assert(base->second == nullptr || base->second == (void *)parent); // accept multiple call of this method if it does not change inheritance chain

    base->second = (void *)parent;
  }

  /** returns true whether the base has been declared inheriting from the parent */
  static bool InheritsFrom(ClassRuntimeID base,  ClassRuntimeID parent)
  {
    // wants to know if base inherits from "void" ??? => yes
    if (parent == nullptr)
      return true;  
    // goes throw the hierarchy until base has no parent
    while (base != nullptr)
    {
      if (base == parent)
        return true;
      base = (ClassRuntimeID)base->second;
    }
    return false;
  }

  /** function can be used to provide an ID for a class */
  // the address of this function can serve has an ID for a class, while unique
  // XXX : do not use it for serialization while it can change from one compilation to another
  template<typename T>
  static void FakeFunction(){}

  /** a function that helps converting a pointer of function into a void pointer */
  template<typename FUNC_PTR>
  static void * FuntionPtrToVoidPtr(FUNC_PTR func_ptr)
  {
    union 
    {
      FUNC_PTR    func_ptr;
      void      * ptr;
    } result;

    result.func_ptr = func_ptr;
    return result.ptr;
  }
};

}; // namespace chaos

