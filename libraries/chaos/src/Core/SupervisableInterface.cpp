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

	void SupervisableInterface::OnRemoveFromSuperviser()
	{
		assert(0);
	}

	void SupervisableInterface::SubReference()
	{
		assert(shared_ref_count > 0);
		if (--shared_ref_count == 0)
		{
			if (supervised_ref_count == 0)
				OnLastReferenceLost();
			else
				OnRemoveFromSuperviser();
		}
	}

	void SupervisableInterface::AddSuperviserReference()
	{
		++supervised_ref_count;
	}

	void SupervisableInterface::SubSuperviserReference()
	{
		assert(supervised_ref_count > 0);
		if (--supervised_ref_count == 0)
		{
			if (shared_ref_count == 0)
				OnLastReferenceLost();
		}
	}

}; // namespace chaos
