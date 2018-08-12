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

		/** a registration block for one class */
		class ClassRegistration
		{
			friend class chaos::ClassTools;

		protected:

			/** constructor */
			ClassRegistration(size_t in_size):
				size(in_size){}

			/** the parent of the class */
			ClassRegistration const * parent = nullptr;
			/** whether the class has been registered */
			bool registered = false;
			/** get class size */
			size_t size = 0;
			/** the optional name of the class */
			std::string class_name;
		};

	protected:

		/** a fake class to declare that a class has no parent */
		class NoParent {};

		/** internal method to have the registration instance for a given class */
		template<typename T>
		static ClassRegistration * GetClassRegistrationInstanceHelper()
		{
			static ClassRegistration registration(sizeof(T));
			return &registration;
		}

		/** internal method to have the registration instance for a given class */
		template<typename T>
		static ClassRegistration * GetClassRegistrationInstance()
		{
			return GetClassRegistrationInstanceHelper<boost::remove_const<T>::type>();
		}

		/** initialize a class registration when no parent */
		static void InitializeRegistration(ClassRegistration * registration, char const * in_class_name, boost::mpl::identity<NoParent>)
		{
			registration->registered = true;
			if (in_class_name != nullptr)
				registration->class_name = in_class_name;
		}

		/** initialize a class registration when one parent */
		template<typename PARENT>
		static void InitializeRegistration(ClassRegistration * registration, char const * in_class_name, boost::mpl::identity<PARENT>)
		{
			registration->parent = GetClassRegistration<PARENT>();
			registration->registered = true;
			if (in_class_name != nullptr)
				registration->class_name = in_class_name;
		}

	public:

		/** internal method to have the registration instance for a given class */
		template<typename T>
		static ClassRegistration const * GetClassRegistration()
		{
			return GetClassRegistrationInstance<T>();
		}

		/** declare a class and its parent */
		template<typename T, typename PARENT = NoParent>
		static int DeclareClass(char const * in_class_name = nullptr)
		{
			assert(sizeof(T) >= sizeof(PARENT)); // NoParent is the sorted class as possible. This should work too

			ClassRegistration * registration = GetClassRegistrationInstance<T>();
			assert(!registration->registered);
			InitializeRegistration(registration, in_class_name, boost::mpl::identity<PARENT>());
			return 0;
		}

		/** returns true whether a class has been declared */
		template<typename T>
		static bool IsClassDeclared()
		{
			ClassRegistration const * registration = GetClassRegistration<T>();
			return registration->registered;
		}

		/** returns whether 2 classes are known to be parents of one another */
		template<typename T, typename PARENT>
		static int InheritsFrom(bool accept_equal = false)
		{
			return InheritsFrom(
				GetClassRegistration<T>(), 
				GetClassRegistration<PARENT>(), 
				accept_equal);
		}

		/** returns whether 2 classes are known to be parents of one another */
		static int InheritsFrom(ClassRegistration const * child_registration, ClassRegistration const * parent_registration, bool accept_equal = false)
		{
			assert(child_registration != nullptr);
			assert(parent_registration != nullptr);

			// fast test on the size
			if (child_registration->size < parent_registration->size)
				return INHERITANCE_NO;

			// class not registered, cannot known result
			if (!child_registration->registered)
				return INHERITANCE_UNKNOWN;

			// class not registered, cannot known result
			if (!parent_registration->registered)
				return INHERITANCE_UNKNOWN;

			// returns no if classes are same and we don't accept that as a valid result
			if (child_registration == parent_registration)
			{
				if (!accept_equal)
					return INHERITANCE_NO;
				else
					return INHERITANCE_YES;
			}
			// from top to root in the hierarchy
			for (child_registration = child_registration->parent; child_registration != nullptr; child_registration = child_registration->parent)
			{
				// fast test on the size
				if (child_registration->size < parent_registration->size)
					return INHERITANCE_NO;
				// found the searched parent
				if (child_registration == parent_registration)
					return INHERITANCE_YES;
				// unintialized class
				if (!child_registration->registered)
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
