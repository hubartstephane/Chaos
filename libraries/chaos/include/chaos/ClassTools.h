#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>

namespace chaos
{
	/**
	 * CHAOS_REGISTER_CLASS : a macro that helps register classes automatically
	 */

#define CHAOS_REGISTER_CLASS(classname) inline chaos::ClassRegistration * classname##_register = chaos::ClassTools::InsertClassRegistration<classname>(#classname);

	/**
	 * InheritanceType : the kind if inheritance that can exist between 2 classes
	 */

	enum class InheritanceType : int
	{
		UNKNOWN = -1,
		NO = 0,
		YES = 1
	};

	/** 
	 * ClassRegistration : a registration block for one class 
	 */
	class ClassRegistration
	{
		friend class ClassTools;

	public:

		/** method to create an instance of the object */
		void* CreateInstance() const
		{
			if (create_instance_func)
				return create_instance_func();
			return nullptr;
		}

		/** returns whether the class has been registered */
		bool IsRegistered() const
		{
			return (class_name.length() > 0);
		}

	protected:

		/** the parent of the class */
		ClassRegistration const* parent = nullptr;
		/** get class size */
		size_t size = 0;
		/** the optional name of the class */
		std::string class_name;
		/** create an instance of the object delegate */
		std::function<void* ()> create_instance_func;
	};

	/**
	 * ClassTools
	 */

	class ClassTools
	{
	public:

		/** find a registration by name */
		static ClassRegistration* GetClassRegistration(char const * class_name)
		{
			assert(class_name != nullptr && strlen(class_name) > 0);
			for (ClassRegistration* registration : GetClassRegistrationList())
				if (StringTools::Strcmp(class_name, registration->class_name) == 0)
					return registration;
			return nullptr;
		}

		/** find a registration by type */
		template<typename CLASS_TYPE>
		static ClassRegistration* GetClassRegistration()
		{
			static ClassRegistration result;
			return &result;
		}

		/** register a class */
		template<typename CLASS_TYPE, typename PARENT_CLASS_TYPE = chaos::EmptyClass>
		static ClassRegistration* InsertClassRegistration(char const* class_name)
		{
			// check parameter and not already registered
			assert(class_name != nullptr && strlen(class_name) > 0);
			assert(GetClassRegistration(class_name) == nullptr);

			ClassRegistration* result = GetClassRegistration<CLASS_TYPE>();
			if (result != nullptr)
			{
				result->class_name = class_name;
				result->size = sizeof(CLASS_TYPE);
				result->create_instance_func = []() { return new CLASS_TYPE; };

				GetClassRegistrationList().push_back(result);
			}		
			return result;
		}
		
		/** returns true whether a class has been declared */
		template<typename T>
		static bool IsClassDeclared()
		{
			ClassRegistration const * registration = GetClassRegistration<T>();
			return registration->IsRegistered();
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
		static InheritanceType InheritsFrom(ClassRegistration const * child_registration, ClassRegistration const * parent_registration, bool accept_equal = false)
		{
			assert(child_registration != nullptr);
			assert(parent_registration != nullptr);

			// fast test on the size
			if (child_registration->size < parent_registration->size)
				return InheritanceType::NO;

			// class not registered, cannot known result
			if (!child_registration->IsRegistered() || !parent_registration->IsRegistered())
				return InheritanceType::UNKNOWN;

			// returns no if classes are same and we don't accept that as a valid result
			if (child_registration == parent_registration)
			{
				if (!accept_equal)
					return InheritanceType::NO;
				else
					return InheritanceType::YES;
			}
			// from top to root in the hierarchy
			for (child_registration = child_registration->parent; child_registration != nullptr; child_registration = child_registration->parent)
			{
				// found the searched parent
				if (child_registration == parent_registration)
					return InheritanceType::YES;
				// fast test on the size
				if (child_registration->size < parent_registration->size)
					return InheritanceType::NO;
				// unintialized class
				if (!child_registration->IsRegistered())
					return InheritanceType::UNKNOWN;
			}
			return InheritanceType::NO;
		}

	protected:

		/** get the list of all registrations */
		static std::vector<ClassRegistration*>& GetClassRegistrationList()
		{
			static std::vector<ClassRegistration*> result;
			return result;
		}

	};

}; // namespace chaos
