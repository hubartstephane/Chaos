#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>

namespace chaos
{
	/**
	 * CHAOS_REGISTER_CLASS : a macro that helps register classes automatically
	 */

	// ideally this should be replaced with __VA_OPT__, but not supported yet by VS2019

#define CHAOS_REGISTER_CLASS1(classname) inline chaos::ClassRegistration * classname##_register = chaos::ClassTools::InsertClassRegistration<classname>(#classname);
#define CHAOS_REGISTER_CLASS2(classname, parent_classname) inline chaos::ClassRegistration * classname##_register = chaos::ClassTools::InsertClassRegistration<classname, parent_classname>(#classname);

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
		void* CreateInstance() const;
		/** returns whether the class has been registered */
		bool IsRegistered() const;

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
		static ClassRegistration* GetClassRegistration(char const* class_name);

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
			assert((std::is_same_v<PARENT_CLASS_TYPE, chaos::EmptyClass> || std::is_base_of_v<PARENT_CLASS_TYPE, CLASS_TYPE>));

			ClassRegistration* result = GetClassRegistration<CLASS_TYPE>();
			if (result != nullptr)
			{
				result->class_name = class_name;
				result->size = sizeof(CLASS_TYPE);
				result->create_instance_func = []() { return new CLASS_TYPE; };

				if (!std::is_same_v<PARENT_CLASS_TYPE, chaos::EmptyClass>)
					result->parent = GetClassRegistration<PARENT_CLASS_TYPE>(); // the parent is accessed, but not necessaraly initialized yet

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
		static InheritanceType InheritsFrom(ClassRegistration const* child_registration, ClassRegistration const* parent_registration, bool accept_equal = false);

	protected:

		/** get the list of all registrations */
		static std::vector<ClassRegistration*>& GetClassRegistrationList();

	};

}; // namespace chaos
