#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/EmptyClass.h>

namespace chaos
{
	// ==============================================================
	// DataOwner
	// ==============================================================

	// DataOwner is a base class to be used for inheritance
	// They can be chained
	//
	// class A : DataOwner<int, DataOwner<float>>
	// {
	// };
	//
	// They are used to give data member to a class
	// 
	// With dynamic casting* it is possible to get the data of a given type
	//
	// Data

	// the template
	template<typename TYPE, typename BASE_CLASS = EmptyClass>
	class DataOwner : public BASE_CLASS
	{
	public:

		using type = TYPE;

		/** constructor */
		DataOwner() = default;
		/** constructor */
		DataOwner(DataOwner const& src) = default;
		/** constructor */
		DataOwner(type const& in_type) :
			data(in_type) {}

		/** gets the data */
		type& GetData() { return data; }
		/** gets the data */
		type const& GetData() const { return data; }

	public:

		/** the data */
		type data;
	};

	// the empty specialization
	template<typename BASE_CLASS>
	class DataOwner<EmptyClass, BASE_CLASS> : public BASE_CLASS
	{
	public:
		/** constructor */
		DataOwner() = default;
		/** constructor */
		DataOwner(DataOwner const& src) = default;
		/** constructor */
		DataOwner(EmptyClass const& in_type) {}
	};

}; // namespace chaos

