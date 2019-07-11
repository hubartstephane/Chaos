#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	// ============================================================
	// NameFilter : an object that handles enabled/disabled name lists
	// ============================================================

	class NameFilter
	{
	public:
	
		/** add a name in the enabled list */
		void AddEnabledName(char const * name);
		/** add a name in the disabled list */
		void AddDisabledName(char const * name);
		/** remove a name in the enabled list */
		void RemoveEnabledName(char const * name);
		/** remove a name in the disabled list */
		void RemoveDisabledName(char const * name);	

		/** check whether the name passes the filter */
		bool IsNameEnabled(char const * name) const;

	protected:

		/** utility method to insert a name in the enabled/disabled array */
		void AddNameImpl(char const * name, std::vector<std::string> & target_list);
		/** utility method to remove a name in the enabled/disabled array */
		void RemoveNameImpl(char const * name, std::vector<std::string> & target_list);

	protected:

		/** the list of enabled names */
		std::vector<std::string> enabled_names;
		/** the list of disabled names */
		std::vector<std::string> disabled_names;
	};

}; // namespace chaos
