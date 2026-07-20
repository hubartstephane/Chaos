#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ClassFindResult::ClassFindResult(ClassManager* in_class_manager, iterator_type in_iterator, ClassMatchType in_match_type, FindClassFlags in_find_flags) :
		class_manager(in_class_manager),
		iterator(in_iterator),
		match_type(in_match_type),
		find_flags(in_find_flags)
	{
	}

	bool ClassFindResult::operator == (nullptr_t) const
	{
		return (result == nullptr) && (class_manager == nullptr); // no cached result, nothing more starting point for searching
	}

	bool ClassFindResult::operator != (nullptr_t) const
	{
		return !operator == (nullptr);
	}

	ClassFindResult::operator ClassBase* () const
	{
		if (result != nullptr || class_manager == nullptr)
			return result;

		if (iterator != class_manager->classes.end())
			return iterator->get();

		return nullptr;
	}

}; // namespace chaos
