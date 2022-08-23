#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

namespace chaos
{
	namespace TiledMap
	{
		// ==========================================
		// ObjectTypeDefinition methods
		// ==========================================

		bool ObjectTypeDefinition::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!PropertyOwner::DoLoad(element))
				return false;
			XMLTools::ReadAttribute(element, "name", name);
			ReadXMLColor(element, "color", color);
			return true;
		}

		// ==========================================
		// ObjectTypeSet methods
		// ==========================================

		bool ObjectTypeSet::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!ManagerObject::DoLoad(element))
				return false;
			if (!DoLoadObjectTypes(element))
				return false;
			return true;
		}

		bool ObjectTypeSet::DoLoadObjectTypes(tinyxml2::XMLElement const * element)
		{
			if (!DoLoadObjectListHelper(element, object_types, "objecttype", nullptr, this))
				return false;
			return true;
		}

		bool ObjectTypeSet::DoLoadMembers(tinyxml2::XMLElement const * element)
		{
			return true;
		}

		ObjectTypeDefinition * ObjectTypeSet::FindObjectType(char const * name)
		{
			size_t count = object_types.size();
			for (size_t i = 0; i < count; ++i)
			{
				ObjectTypeDefinition * definition = object_types[i].get();
				if (definition == nullptr)
					continue;
				if (StringTools::Stricmp(definition->name, name) == 0)
					return definition;
			}
			return nullptr;
		}

		ObjectTypeDefinition const * ObjectTypeSet::FindObjectType(char const * name) const
		{
			size_t count = object_types.size();
			for (size_t i = 0; i < count; ++i)
			{
				ObjectTypeDefinition const * definition = object_types[i].get();
				if (definition == nullptr)
					continue;
				if (StringTools::Stricmp(definition->name, name) == 0)
					return definition;
			}
			return nullptr;
		}

		Property const * ObjectTypeSet::FindObjectProperty(char const * type, char const * name, PropertyType type_id) const
		{
			if (StringTools::IsEmpty(type))
				return nullptr;
			ObjectTypeDefinition const * definition = FindObjectType(type);
			if (definition == nullptr)
				return nullptr;
			return definition->FindProperty(name, type_id);
		}

	};  // namespace TiledMap

}; // namespace chaos
