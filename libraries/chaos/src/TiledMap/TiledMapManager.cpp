#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

namespace chaos
{
	namespace TiledMap
	{
			// ==========================================
			// Manager methods
			// ==========================================

#define CHAOS_IMPL_MANAGER_LOAD(function_name, find_function_name, return_type, func_params, call_args)\
return_type * Manager::function_name(func_params, bool store_object)\
{\
	return_type * result = find_function_name(path);\
	if (result != nullptr)\
		return result;\
	return Do##function_name(call_args, store_object);\
}

#define CHAOS_IMPL_MANAGER_LOAD_ALL(function_name, find_function_name, return_type)\
	CHAOS_IMPL_MANAGER_LOAD(function_name, find_function_name, return_type, FilePathParam const & path, path)\
	CHAOS_IMPL_MANAGER_LOAD(function_name, find_function_name, return_type, FilePathParam const & path BOOST_PP_COMMA() Buffer<char> buffer, path BOOST_PP_COMMA() buffer)\
	CHAOS_IMPL_MANAGER_LOAD(function_name, find_function_name, return_type, FilePathParam const & path BOOST_PP_COMMA() tinyxml2::XMLDocument const * doc, path BOOST_PP_COMMA() doc)

	CHAOS_IMPL_MANAGER_LOAD_ALL(LoadMap, FindMap, Map);
	CHAOS_IMPL_MANAGER_LOAD_ALL(LoadTileSet, FindTileSet, TileSet);
	CHAOS_IMPL_MANAGER_LOAD_ALL(LoadObjectTypeSet, FindObjectTypeSet, ObjectTypeSet);

#undef CHAOS_IMPL_MANAGER_LOAD_ALL
#undef CHAOS_IMPL_MANAGER_LOAD

#define CHAOS_IMPL_MANAGER_FIND(funcname, return_type, member_name, constness)\
return_type constness * Manager::funcname(FilePathParam const & path) constness\
{\
	size_t count = member_name.size();\
	for (size_t i = 0; i < count; ++i)\
		if (member_name[i]->IsMatchingName(path.GetResolvedPath()))\
			return member_name[i].get();\
	return nullptr;\
}
		CHAOS_IMPL_MANAGER_FIND(FindMap, Map, maps, BOOST_PP_EMPTY());
		CHAOS_IMPL_MANAGER_FIND(FindMap, Map, maps, const);
		CHAOS_IMPL_MANAGER_FIND(FindTileSet, TileSet, tile_sets, BOOST_PP_EMPTY());
		CHAOS_IMPL_MANAGER_FIND(FindTileSet, TileSet, tile_sets, const);
		CHAOS_IMPL_MANAGER_FIND(FindObjectTypeSet, ObjectTypeSet, object_type_sets, BOOST_PP_EMPTY());
		CHAOS_IMPL_MANAGER_FIND(FindObjectTypeSet, ObjectTypeSet, object_type_sets, const);
#undef CHAOS_IMPL_MANAGER_FIND

#define CHAOS_IMPL_MANAGER_DOLOAD(funcname, return_type, member_name)\
return_type * Manager::funcname(FilePathParam const & path, bool store_object)\
{\
	if (Buffer<char> buffer = FileTools::LoadFile(path, LoadFileFlag::ASCII | LoadFileFlag::NO_ERROR_TRACE))\
		return funcname(path, buffer, store_object);\
	Log::Error("Manager::" #funcname ": fail to load [%s]", path.GetResolvedPath().string().c_str());\
	return nullptr;\
}\
return_type * Manager::funcname(FilePathParam const & path, Buffer<char> buffer, bool store_object)\
{\
	return_type * result = nullptr;\
	tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();\
	if (doc != nullptr)\
	{\
		tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);\
		if (error == tinyxml2::XML_SUCCESS)\
			result = funcname(path, doc, store_object);\
		delete(doc);\
	}\
	return result;\
}\
return_type * Manager::funcname(FilePathParam const & path, tinyxml2::XMLDocument const * doc, bool store_object)\
{\
	assert(doc != nullptr);\
	return_type * result = new return_type(this, path.GetResolvedPath());\
	if (result != nullptr)\
	{\
		if (result->DoLoadDocument(doc))\
		{\
			if (store_object)\
				member_name.push_back(result);\
		}\
		else\
		{\
			delete(result);\
			result = nullptr;\
		}\
	}\
	return result;\
}

		CHAOS_IMPL_MANAGER_DOLOAD(DoLoadTileSet, TileSet, tile_sets);
		CHAOS_IMPL_MANAGER_DOLOAD(DoLoadObjectTypeSet, ObjectTypeSet, object_type_sets);
		CHAOS_IMPL_MANAGER_DOLOAD(DoLoadMap, Map, maps);

#undef CHAOS_IMPL_MANAGER_DOLOAD

		Property const * Manager::FindObjectProperty(char const * type, char const * name, PropertyType type_id) const
		{
			if (StringTools::IsEmpty(type))
				return nullptr;
			size_t count = object_type_sets.size();
			for (size_t i = 0 ; i < count ; ++i)
			{
				ObjectTypeSet const * ots = object_type_sets[i].get();
				if (ots == nullptr)
					return nullptr;
				Property const * result = ots->FindObjectProperty(type, name, type_id);
				if (result != nullptr)
					return result;
			}
			return nullptr;
		}

	};  // namespace TiledMap

}; // namespace chaos
