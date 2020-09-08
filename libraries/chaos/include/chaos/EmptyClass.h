#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	/** an empty base class */
	using EmptyClass = boost::mpl::empty_base;

	/** an 'empty' base class with a VTABLE */
	class EmptyClassVTable
	{
	public:

		virtual ~EmptyClassVTable() = default;
	};

	/** a selector for EmpyClass with or without VTABLE */
	template<bool VTABLE = false>
	using EmptyClassExt = typename boost::mpl::if_c<VTABLE, EmptyClassVTable, EmptyClass>::type;

}; // namespace chaos
