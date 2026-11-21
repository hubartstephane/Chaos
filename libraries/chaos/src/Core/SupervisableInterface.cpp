#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	SupervisableInterface::SupervisableInterface() :
		supervised_ref_count(0)
	{
	}

	SupervisableInterface::~SupervisableInterface()
	{
		assert(supervised_ref_count == 0);
	}

	SupervisableInterface::SupervisableInterface(SupervisableInterface const&) :
		SupervisableInterface()
	{
		// do not copy reference count related members 
	}

	SupervisableInterface::SupervisableInterface(SupervisableInterface&&) noexcept :
		SupervisableInterface()
	{
		// do not copy reference count related members 
	}

	SupervisableInterface& SupervisableInterface::operator = (SupervisableInterface&&) noexcept
	{
		return *this; // do not copy reference count related members 
	}
	SupervisableInterface& SupervisableInterface::operator = (SupervisableInterface const&)
	{
		return *this; // do not copy reference count related members 
	}

	void SupervisableInterface::AddSuperviserReference()
	{
		++supervised_ref_count;
	}

	void SupervisableInterface::SubSuperviserReference()
	{
		assert(supervised_ref_count > 0);
		if (--supervised_ref_count == 0)
			RemoveFromSuperviser();
	}

	void IntrusiveAddSupervisedReference(SupervisableInterface* obj)
	{
		obj->AddSuperviserReference();
	}

	void IntrusiveSubSupervisedReference(SupervisableInterface* obj)
	{
		obj->SubSuperviserReference();
	}

}; // namespace chaos
