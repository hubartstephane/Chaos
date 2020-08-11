#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/EmptyClass.h>

namespace chaos
{
	class Object;

	/**
	 * CHAOS_REGISTER_CLASS : a macro that helps register classes automatically
	 */

	// ideally this should be replaced with __VA_OPT__, but not supported yet by VS2019

#define CHAOS_REGISTER_CLASS1(classname) inline chaos::Class const * classname##_class = chaos::Class::DeclareClass<classname>(#classname);
#define CHAOS_REGISTER_CLASS2(classname, parent_classname) inline chaos::Class const * classname##_class = chaos::Class::DeclareClass<classname, parent_classname>(#classname);

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
	 * Class : a registered class
	 */
	class Class
	{
		friend class ClassLoader;

	public:

		/** method to create an instance of the object */
		Object * CreateInstance() const;
		/** returns whether the class has been registered */
		bool IsDeclared() const;
		/** gets the class size */
		size_t GetClassSize() const { return class_size; }
		/** gets the parent class */
		Class const* GetParentClass() const { return parent; }
		/** gets the class name */
		std::string const & GetClassName() const { return class_name; }
		/** returns whether we can create instances */
		bool CanCreateInstance() const { return create_instance_func != nullptr; }

	public:

		/** find a class by name */
		static Class const* FindClass(char const* class_name);

		/** find a class by type */
		template<typename CLASS_TYPE>
		static Class const* FindClass()
		{
			Class const* result = FindClassImpl<CLASS_TYPE>();
			if (!result->IsDeclared())
				return nullptr;
			return result;
		}

		/** declare a class */
		template<typename CLASS_TYPE, typename PARENT_CLASS_TYPE = chaos::EmptyClass>
		static Class const* DeclareClass(char const* class_name)
		{
			// check parameter and not already registered
			assert(class_name != nullptr && strlen(class_name) > 0);
			assert(FindClass(class_name) == nullptr);
			assert((std::is_same_v<PARENT_CLASS_TYPE, chaos::EmptyClass> || std::is_base_of_v<PARENT_CLASS_TYPE, CLASS_TYPE>));

			Class* result = FindClassImpl<CLASS_TYPE>();
			if (result != nullptr)
			{
				result->class_name = class_name;
				result->class_size = sizeof(CLASS_TYPE);

				// instance constructible only if derives from Object
				if constexpr (std::is_base_of_v<Object, CLASS_TYPE>)
					result->create_instance_func = []() { return new CLASS_TYPE; }; 
				// the parent is accessed, but not necessaraly initialized yet
				if (!std::is_same_v<PARENT_CLASS_TYPE, chaos::EmptyClass>)
					result->parent = FindClassImpl<PARENT_CLASS_TYPE>(); 

				GetClassesList().push_back(result);
			}
			return result;
		}

		/** declare a pseudo class, that is a class with additionnal json initialization */
		static Class const* DeclareSpecialClass(char const* class_name, nlohmann::json const & json);

		/** static inheritance method */
		static InheritanceType InheritsFrom(Class const* child_class, Class const* parent_class, bool accept_equal = false);
		/** returns whether the class inherits from parent */
		InheritanceType InheritsFrom(Class const* parent_class, bool accept_equal = false) const;

		/** get the list of all classes */
		static std::vector<Class const*>& GetClassesList();

	protected:

		/** internal method to declare a class without finding yet its parent (used for directory iteration) */
		static Class const* DoDeclareSpecialClass(char const* class_name, nlohmann::json const & json, bool accept_unknown_parent);

	protected:

		/** return the class for a type even if not initialized */
		template<typename CLASS_TYPE>
		static Class * FindClassImpl()
		{
			static Class result;
			return &result;
		}

	protected:

		/** the parent of the class */
		Class const* parent = nullptr;
		/** get class size */
		size_t class_size = 0;
		/** the optional name of the class */
		std::string class_name;
		/** whether instance of the class may be created (derives from Object) */
		bool inherit_from_object = false;
		/** create an instance of the object delegate */
		std::function<Object * ()> create_instance_func;
		/** additionnal initialization for JSONSerializable objects */
		nlohmann::json json_data;
	};

}; // namespace chaos
