#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ClassBase::SetShortName(std::string in_shorname)
	{
		assert(short_name.empty()); // this should only be called/set a single time
		short_name = std::move(in_shorname);
	}

	bool ClassBase::InheritsFrom(ClassBase const* base, bool accept_same) const
	{
		if (base == this)
			return accept_same;

		ClassBase const* cls = parent_class;
		while (cls != nullptr)
		{
			if (cls == base)
				return true;
			cls = cls->parent_class;
		}
		return false;
	}

	template<typename CPP_TYPE>
	bool ClassBase::InheritsFrom(bool accept_same) const
	{
		return InheritsFrom(Class<CPP_TYPE>::GetNativeClassInstance(), accept_same);
	}

}; // namespace chaos
