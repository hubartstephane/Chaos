#pragma once

// XXX : the component/component owner such as Camera/CameraComponent or Player/PlayerComponent is such a common paradigm that it is worth
//       having some macros prevent code duplication

/** a macro to help inserting code for component owning in class declaration */
#define DEATH_DECLARE_COMPONENT_OWNER(COMPONENT_CLASS, FUNC_COMPONENT_NAME)\
	size_t Get##FUNC_COMPONENT_NAME##Count() const;\
	COMPONENT_CLASS * Get##FUNC_COMPONENT_NAME(size_t index);\
	COMPONENT_CLASS const * Get##FUNC_COMPONENT_NAME(size_t index) const;\
	COMPONENT_CLASS * Find##FUNC_COMPONENT_NAME(chaos::TagType tag);\
	COMPONENT_CLASS const * Find##FUNC_COMPONENT_NAME(chaos::TagType tag) const;\
	COMPONENT_CLASS * Find##FUNC_COMPONENT_NAME(char const * name);\
	COMPONENT_CLASS const * Find##FUNC_COMPONENT_NAME(char const * name) const;\
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
COMPONENT_CLASS * OWNER_CLASS::Find##FUNC_COMPONENT_NAME(chaos::TagType tag)\
{\
	return chaos::NamedObject::FindNamedObject(VECTOR_NAME, tag);\
}\
COMPONENT_CLASS const * OWNER_CLASS::Find##FUNC_COMPONENT_NAME(chaos::TagType tag) const\
{\
	return chaos::NamedObject::FindNamedObject(VECTOR_NAME, tag);\
}\
COMPONENT_CLASS * OWNER_CLASS::Find##FUNC_COMPONENT_NAME(char const * name)\
{\
	return chaos::NamedObject::FindNamedObject(VECTOR_NAME, name);\
}\
COMPONENT_CLASS const * OWNER_CLASS::Find##FUNC_COMPONENT_NAME(char const * name) const\
{\
	return chaos::NamedObject::FindNamedObject(VECTOR_NAME, name);\
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
			component->OWNER_POINTER_NAME = nullptr;\
			component->OnRemovedFrom(this);\
		}\
  }\
	return true;\
}\
bool OWNER_CLASS::Remove##FUNC_COMPONENT_NAME(chaos::TagType tag)\
{\
	return true;\
}\
bool OWNER_CLASS::Remove##FUNC_COMPONENT_NAME(char const * name)\
{\
	return true;\
}