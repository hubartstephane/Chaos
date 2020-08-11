#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FilePath.h>
#include <chaos/Class.h>

namespace chaos
{
	class ClassLoader
	{
	public:

		/** load one class by its path */
		virtual Class const * LoadClass(FilePathParam const& path);
		/** load all classes in one directory */
		virtual bool LoadClassesInDirectory(FilePathParam const& path);

	protected:

		/** load one class by its path internal method */
		virtual Class const * DoLoadClass(FilePathParam const& path, bool accept_unknown_parent);
	};

}; // namespace chaos
