#pragma once

// XXX : the component/component owner such as Camera/CameraComponent or Player/PlayerComponent is such a common paradigm that it is worth
//       having some macros preventing code duplication

/** a macro to help inserting code for component owning in class declaration */
#define DEATH_DECLARE_COMPONENT_OWNER(COMPONENT_CLASS, FUNC_COMPONENT_NAME, VECTOR_NAME)\
	size_t Get##FUNC_COMPONENT_NAME##Count() const;\
	COMPONENT_CLASS * Get##FUNC_COMPONENT_NAME(size_t index);\
	COMPONENT_CLASS const * Get##FUNC_COMPONENT_NAME(size_t index) const;\
	COMPONENT_CLASS * Find##FUNC_COMPONENT_NAME(chaos::NamedObjectRequest request);\
	COMPONENT_CLASS const * Find##FUNC_COMPONENT_NAME(chaos::NamedObjectRequest request) const;\
	template<typename T>\
	T * Find##FUNC_COMPONENT_NAME##ByClass()\
	{\
		T * result = nullptr;\
		size_t count = VECTOR_NAME.size();\
		for (size_t i = 0 ; i < count ; ++i)\
		{\
			result = auto_cast(VECTOR_NAME[i].get());\
			if (result != nullptr)\
				return result;\
		}\
		return nullptr;\
	}\
	template<typename T>\
	T const * Find##FUNC_COMPONENT_NAME##ByClass() const\
	{\
		T const * result = nullptr;\
		size_t count = VECTOR_NAME.size();\
		for (size_t i = 0 ; i < count ; ++i)\
		{\
			result = auto_cast(VECTOR_NAME[i].get());\
			if (result != nullptr)\
				return result;\
		}\
		return nullptr;\
	}\
	bool Add##FUNC_COMPONENT_NAME(COMPONENT_CLASS * component);\
	bool Remove##FUNC_COMPONENT_NAME(COMPONENT_CLASS * component);\
	bool Remove##FUNC_COMPONENT_NAME(chaos::TagType tag);\
	bool Remove##FUNC_COMPONENT_NAME(char const * name);

/** a macro to help inserting code implementation for component owning classes */
#define DEATH_IMPLEMENT_COMPONENT_OWNER(OWNER_CLASS, COMPONENT_CLASS, FUNC_COMPONENT_NAME, VECTOR_NAME, OWNER_POINTER_NAME)\
size_t OWNER_CLASS::Get##FUNC_COMPONENT_NAME##Count() const\
{\
	return VECTOR_NAME.size();\
}\
COMPONENT_CLASS * OWNER_CLASS::Get##FUNC_COMPONENT_NAME(size_t index)\
{\
	if (index >= VECTOR_NAME.size())\
		return nullptr;\
	return VECTOR_NAME[index].get();\
}\
COMPONENT_CLASS const * OWNER_CLASS::Get##FUNC_COMPONENT_NAME(size_t index) const\
{\
	if (index >= VECTOR_NAME.size())\
		return nullptr;\
	return VECTOR_NAME[index].get();\
}\
COMPONENT_CLASS * OWNER_CLASS::Find##FUNC_COMPONENT_NAME(chaos::NamedObjectRequest request)\
{\
	return chaos::NamedObject::FindNamedObject(VECTOR_NAME, request);\
}\
COMPONENT_CLASS const * OWNER_CLASS::Find##FUNC_COMPONENT_NAME(chaos::NamedObjectRequest request) const\
{\
	return chaos::NamedObject::FindNamedObject(VECTOR_NAME, tag);\
}\
bool OWNER_CLASS::Add##FUNC_COMPONENT_NAME(COMPONENT_CLASS * component)\
{\
	if (component == nullptr)\
		return false;\
	assert(component->OWNER_POINTER_NAME == nullptr);\
	VECTOR_NAME.push_back(component);\
	component->OWNER_POINTER_NAME = this;\
	component->OnInsertedInto(this);\
	return true;\
}\
bool OWNER_CLASS::Remove##FUNC_COMPONENT_NAME(COMPONENT_CLASS * component)\
{\
	if (component == nullptr)\
		return false;\
	size_t count = VECTOR_NAME.size();\
	for (size_t i = 0 ; i < count ; ++i)\
	{\
		if (VECTOR_NAME[i].get() == component)\
		{\
			chaos::shared_ptr<COMPONENT_CLASS> c = VECTOR_NAME[i];\
			VECTOR_NAME.erase(VECTOR_NAME.begin() + i);\
			c->OWNER_POINTER_NAME = nullptr;\
			c->OnRemovedFrom(this);\
		}\
  }\
	return true;\
}\
bool OWNER_CLASS::Remove##FUNC_COMPONENT_NAME(chaos::TagType tag)\
{\
	size_t count = VECTOR_NAME.size();\
	for (size_t i = 0 ; i < count ; ++i)\
	{\
		if (Match(*VECTOR_NAME[i].get(), tag))\
		{\
			chaos::shared_ptr<COMPONENT_CLASS> c = VECTOR_NAME[i];\
			VECTOR_NAME.erase(VECTOR_NAME.begin() + i);\
			c->OWNER_POINTER_NAME = nullptr;\
			c->OnRemovedFrom(this);\
		}\
  }\
	return true;\
}\
bool OWNER_CLASS::Remove##FUNC_COMPONENT_NAME(char const * name)\
{\
	size_t count = VECTOR_NAME.size();\
	for (size_t i = 0 ; i < count ; ++i)\
	{\
		if (Match(*VECTOR_NAME[i].get(), name))\
		{\
			chaos::shared_ptr<COMPONENT_CLASS> c = VECTOR_NAME[i];\
			VECTOR_NAME.erase(VECTOR_NAME.begin() + i);\
			c->OWNER_POINTER_NAME = nullptr;\
			c->OnRemovedFrom(this);\
		}\
  }\
	return true;\
}