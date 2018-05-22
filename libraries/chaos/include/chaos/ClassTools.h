#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/AllocatorTools.h>

namespace chaos
{
	class ClassTools
	{
	public:

		// the inheritance values
		static int const INHERITANCE_UNKNOWN = -1;
		static int const INHERITANCE_NO = 0;
		static int const INHERITANCE_YES = 1;

	protected:

		/** a registration block for one class */
		class ClassRegistration
		{
		public:

			/** the parent of the class */
			ClassRegistration * parent = nullptr;
			/** whether the class has been registered */
			bool registered = false;
		};

		/** a fake class to declare that a class has no parent */
		class NoParent {};

		/** internal method to have the registration instance for a given class */
		template<typename T>
		static ClassRegistration * GetClassRegistration()
		{
			static ClassRegistration registration;
			return &registration;
		}

		/** initialize a class registration when no parent */
		static void InitializeRegistration(ClassRegistration * registration, boost::mpl::identity<NoParent>)
		{
			registration->registered = true;
		}

		/** initialize a class registration when one parent */
		template<typename PARENT>
		static void InitializeRegistration(ClassRegistration * registration, boost::mpl::identity<PARENT>)
		{
			registration->parent = GetClassRegistration<PARENT>();
			registration->registered = true;
		}

	public:

		/** declare a class and its parent */
		template<typename T, typename PARENT = NoParent>
		static int DeclareClass()
		{
			ClassRegistration * registration = GetClassRegistration<T>();
			assert(!registration->registered);
			InitializeRegistration(registration, boost::mpl::identity<PARENT>());
			return 0;
		}

		/** returns true whether a class has been declared */
		template<typename T>
		static bool IsClassDeclared()
		{
			ClassRegistration * registration = GetClassRegistration<T>();
			return registration->registered;
		}

		/** returns whether 2 classes are known to be parents of one another */
		template<typename T, typename PARENT>
		static int InheritsFrom(bool accept_equal = false)
		{
			ClassRegistration * class_registration = GetClassRegistration<T>(); // class not registered, cannot known
			if (!class_registration->registered)
				return INHERITANCE_UNKNOWN;

			ClassRegistration * parent_registration = GetClassRegistration<PARENT>(); // class not registered, cannot known
			if (!parent_registration->registered)
				return INHERITANCE_UNKNOWN;

			// returns no if classes are same and we don't accept that as a valid result
			if (class_registration == parent_registration && !accept_equal)
				return INHERITANCE_NO;

			// from top to root in the hierarchy
			for (class_registration = class_registration->parent; class_registration != nullptr; class_registration = class_registration->parent)
			{
				// found the searched parent
				if (class_registration == parent_registration)
					return INHERITANCE_YES;
				// unintialized class
				if (!class_registration->registered)
					return INHERITANCE_UNKNOWN;
			}
			return INHERITANCE_NO;
		}

		/** GetClassID : returns a unique ID given a class */
		template<typename T>
		static uintptr_t GetClassID()
		{
			return (uintptr_t)GetClassRegistration<T>();
		};

		/** Aligned16 : base class for children that want to be 16 aligned */
		class Aligned16
		{
		public:

			/** the new operator */
			void * operator new (size_t size){ return AllocatorTools::Aligned16Alloc(size);}
			/** the delete operator */
			void operator delete(void * ptr){ AllocatorTools::Aligned16Free(ptr);}
		};

	}; // namespace ClassTools

}; // namespace chaos
