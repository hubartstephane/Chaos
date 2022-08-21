#include "chaos/Chaos.h"

namespace chaos
{
	namespace TiledMap
	{
		// ==========================================
		// Property methods
		// ==========================================
		
		int const* Property::GetPropertyInt() const 
		{ 
			PropertyInt const * prop = auto_cast(this);
			if (prop != nullptr)
				return &prop->value;
			return nullptr;
		}
		
		float const* Property::GetPropertyFloat() const 
		{ 
			PropertyFloat const * prop = auto_cast(this);
			if (prop != nullptr)
				return &prop->value;
			return nullptr;
		}

		bool const* Property::GetPropertyBool() const 
		{ 
			PropertyBool const* prop = auto_cast(this);
			if (prop != nullptr)
				return &prop->value;
			return nullptr;
		}
		
		std::string const* Property::GetPropertyString() const 
		{ 
			PropertyString const * prop = auto_cast(this);
			if (prop != nullptr)
				return &prop->value;
			return nullptr;
		}
		
		glm::vec4 const* Property::GetPropertyColor() const 
		{ 
			PropertyColor const* prop = auto_cast(this);
			if (prop != nullptr)
				return &prop->value;
			return nullptr;
		}

		int const* Property::GetPropertyObject() const
		{
			PropertyObject const* prop = auto_cast(this);
			if (prop != nullptr)
				return &prop->value;
			return nullptr;
		}

		// ==========================================
		// PropertyOwner methods
		// ==========================================

		Property const * PropertyOwner::FindProperty(char const * name, PropertyType type_id) const
		{
			return FindInternalProperty(name, type_id);
		}

		Property const * PropertyOwner::FindInternalProperty(char const * name, PropertyType type_id) const
		{
			assert(name != nullptr);
			for (auto & property : properties)
				if (type_id == PropertyType::ANY || type_id == property->GetPropertyType())
					if (StringTools::Stricmp(property->GetName(), name) == 0)
						return property.get();
			return nullptr;
		}

		int const * PropertyOwner::FindPropertyInt(char const * name) const
		{
			Property const * property = FindProperty(name, PropertyType::INT);
			if (property == nullptr)
				return nullptr;
			return property->GetPropertyInt();
		}

		float const * PropertyOwner::FindPropertyFloat(char const * name) const
		{
			Property const * property = FindProperty(name, PropertyType::FLOAT);
			if (property == nullptr)
				return nullptr;
			return property->GetPropertyFloat();
		}

		bool const * PropertyOwner::FindPropertyBool(char const * name) const
		{
			Property const * property = FindProperty(name, PropertyType::BOOL);
			if (property == nullptr)
				return nullptr;
			return property->GetPropertyBool();
		}

		std::string const * PropertyOwner::FindPropertyString(char const * name) const
		{
			Property const * property = FindProperty(name, PropertyType::STRING);
			if (property == nullptr)
				return nullptr;
			return property->GetPropertyString();
		}

		glm::vec4 const* PropertyOwner::FindPropertyColor(char const* name) const
		{
			Property const* property = FindProperty(name, PropertyType::COLOR);
			if (property == nullptr)
				return nullptr;
			return property->GetPropertyColor();
		}

		int const* PropertyOwner::FindPropertyObject(char const* name) const
		{
			Property const* property = FindProperty(name, PropertyType::OBJECT);
			if (property == nullptr)
				return nullptr;
			return property->GetPropertyObject();
		}

#define CHAOS_FIND_PROPERTY_WITH_DEFAULT(suffix, return_type, arg_type)\
		return_type PropertyOwner::GetPropertyValue##suffix(char const * name, arg_type default_value) const\
		{\
			return_type const * result = FindProperty##suffix(name);\
			if (result == nullptr)\
				return default_value;\
			return *result;\
		}		
		CHAOS_FIND_PROPERTY_WITH_DEFAULT(Int, int, int);			
		CHAOS_FIND_PROPERTY_WITH_DEFAULT(Float, float, float);
		CHAOS_FIND_PROPERTY_WITH_DEFAULT(Bool, bool, bool);
		CHAOS_FIND_PROPERTY_WITH_DEFAULT(String, std::string, char const*);
		CHAOS_FIND_PROPERTY_WITH_DEFAULT(Color, glm::vec4, glm::vec4 const&);
		CHAOS_FIND_PROPERTY_WITH_DEFAULT(Object, int, int);
#undef CHAOS_FIND_PROPERTY_WITH_DEFAULT
		
		bool PropertyOwner::DoLoad(tinyxml2::XMLElement const * element)
		{
			assert(element != nullptr);

			return DoLoadProperties(GetPropertiesChildNode(element));
		}

		tinyxml2::XMLElement const * PropertyOwner::GetPropertiesChildNode(tinyxml2::XMLElement const * element) const
		{
			return element->FirstChildElement("properties");
		}

		bool PropertyOwner::DoLoadProperties(tinyxml2::XMLElement const * properties_element)
		{
			// no properties is not an error
			if (properties_element == nullptr)
				return true;

			tinyxml2::XMLElement const * node = properties_element->FirstChildElement("property");
			for (; node != nullptr; node = node->NextSiblingElement("property"))
			{
				tinyxml2::XMLAttribute const * name_attribute = node->FindAttribute("name"); // name is mandatory
				if (name_attribute == nullptr)
					continue;
				char const * property_name = name_attribute->Value();
				if (FindInternalProperty(property_name, PropertyType::ANY) != nullptr) // and must be unique
					continue;

				tinyxml2::XMLAttribute const * value_attribute = node->FindAttribute("value"); // value is NOT mandatory (for multiline strings, we use node->GetText())
				if (value_attribute == nullptr)
					value_attribute = node->FindAttribute("default"); // for ObjectTypeSets, that use the keyword 'default' instead of 'value'

				tinyxml2::XMLAttribute const * type_attribute = node->FindAttribute("type"); // type is NOT mandatory (default is string)

				char const * property_type = (type_attribute != nullptr) ?
					type_attribute->Value() : nullptr;

				if (property_type == nullptr || StringTools::Stricmp(property_type, "string") == 0)
				{
					char const * value = (value_attribute != nullptr) ? value_attribute->Value() : node->GetText();
					if (value != nullptr)
						CreatePropertyString(property_name, value);
				}
				else if (value_attribute != nullptr) // now, to this point, value_attribute becomes MANDATORY
				{
					if (StringTools::Stricmp(property_type, "int") == 0)
					{
						CreatePropertyInt(property_name, value_attribute->IntValue());
					}
					else if (StringTools::Stricmp(property_type, "float") == 0)
					{
						CreatePropertyFloat(property_name, value_attribute->FloatValue());
					}
					else if (StringTools::Stricmp(property_type, "bool") == 0)
					{
						CreatePropertyBool(property_name, value_attribute->BoolValue());
					}
					else if (StringTools::Stricmp(property_type, "color") == 0)
					{
						glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
						if (ReadXMLColor(value_attribute->Value(), color))
							CreatePropertyColor(property_name, color);
					}
					else if (StringTools::Stricmp(property_type, "object") == 0)
					{
						CreatePropertyObject(property_name, value_attribute->IntValue());
					}
				}
			}
			return true;
		}

#define CHAOS_CREATE_PROPERTY(suffix, result_type, arg_type)\
		result_type * PropertyOwner::CreateProperty##suffix(char const * name, arg_type value)\
		{\
			result_type* result = new result_type(this);\
			if (result != nullptr)\
			{\
				result->name = name;\
				result->value = value;\
				properties.push_back(result);\
			}\
			return result;\
		}
		CHAOS_CREATE_PROPERTY(Int, PropertyInt, int);
		CHAOS_CREATE_PROPERTY(Float, PropertyFloat, float);
		CHAOS_CREATE_PROPERTY(Bool, PropertyBool, bool);
		CHAOS_CREATE_PROPERTY(String, PropertyString, char const *);
		CHAOS_CREATE_PROPERTY(Color, PropertyColor, glm::vec4 const &);
		CHAOS_CREATE_PROPERTY(Object, PropertyObject, int);
#undef CHAOS_CREATE_PROPERTY

		bool PropertyOwner::IsObjectOfType(char const* in_type) const
		{
			Property const* property = FindProperty(in_type);
			if (property != nullptr)
			{
				bool const* property_bool = property->GetPropertyBool();
				if (property_bool != nullptr)
					return *property_bool;

				int const* property_int = property->GetPropertyInt();
				if (property_int != nullptr)
					return (*property_int > 0);
			}
			return false;
		}

		// ==========================================
		// TypedObject methods
		// ==========================================

		bool TypedObject::IsObjectOfType(char const* in_type) const
		{
			if (StringTools::Stricmp(type, in_type) == 0)
				return true;
			return PropertyOwner::IsObjectOfType(in_type);
		}

		Property const * TypedObject::FindProperty(char const * name, PropertyType type_id) const
		{
			Property const * result = PropertyOwner::FindProperty(name, type_id);
			if (result == nullptr && !StringTools::IsEmpty(type))
			{
				Manager const * manager = GetManager();
				if (manager != nullptr)
					result = manager->FindObjectProperty(type.c_str(), name, type_id);
			}
			return result;
		}

		bool TypedObject::DoLoad(tinyxml2::XMLElement const* element)
		{
			if (!PropertyOwner::DoLoad(element))
				return false;
			XMLTools::ReadAttribute(element, "type", type);
			return true;
		}

	};  // namespace TiledMap

}; // namespace chaos
