#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/**
 * Simply a base class
 */

class EmptyClass {};

/**
 * VirtualDestructor : an utility class to add a virtual destructor to any base class 
 */

template<typename BASE_CLASS>
class AddVirtualDestructor : public BASE_CLASS
{
public:

  /** simply a virtual destructor */
  virtual ~AddVirtualDestructor() {}
};

template<typename COND, typename BASE_CLASS = EmptyClass>
using ConditionnalAddVirtualDestructor = boost::mpl::eval_if <
  COND,
  AddVirtualDestructor<BASE_CLASS>,
  BASE_CLASS>::type;

/**
 * NoCopy : an utility class to suppress copy construction/operator
 */

template<typename BASE_CLASS = EmptyClass>
class NoCopy : public BASE_CLASS
{
public:

  /** keep default constructor */
  NoCopy() {}
  /** delete copy constructor */
  NoCopy(NoCopy const &) = delete;
  /** delete copy operator */
  NoCopy & operator = (NoCopy const &) = delete;
};






/** template */
template<bool VIRTUAL_DESTRUCTOR, bool CANCOPY>
class ClassBase {};



}; // namespace chaos
