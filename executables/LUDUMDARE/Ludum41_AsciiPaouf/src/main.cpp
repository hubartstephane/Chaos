#include "Ludum41PCH.h"
#include "Ludum41Game.h"

using namespace chaos;

class A;
class B;

#define CHAOS_SUPERVISION_A_B (B,A,B,A,objects, owner)



#define CHAOS_GENERATE_SUPERVISER_DECLARATION_FUNCTIONS(\
	SUPERVISER_CLASSNAME,\
	SUPERVISED_CLASSNAME,\
	SUPERVISER_SHORTNAME,\
	SUPERVISED_SHORTNAME,\
	SUPERVISER_VECTOR_MEMBER_NAME,\
	SUPERVISED_PARENT_MEMBER_NAME\
)\
friend class SUPERVISED_CLASSNAME;\
public:\
void Remove##SUPERVISED_SHORTNAME(SUPERVISED_CLASSNAME * obj);\
protected:\
void Remove##SUPERVISED_SHORTNAME##At(size_t index);\
protected:\
std::vector<supervised_ptr<SUPERVISED_CLASSNAME>> SUPERVISER_VECTOR_MEMBER_NAME;


#define CHAOS_GENERATE_SUPERVISER_IMPLEMENTATION_FUNCTIONS(\
	SUPERVISER_CLASSNAME,\
	SUPERVISED_CLASSNAME,\
	SUPERVISER_SHORTNAME,\
	SUPERVISED_SHORTNAME,\
	SUPERVISER_VECTOR_MEMBER_NAME,\
	SUPERVISED_PARENT_MEMBER_NAME,\
	REMOVE_SWAP\
)\
void SUPERVISER_CLASSNAME::Remove##SUPERVISED_SHORTNAME(SUPERVISED_CLASSNAME * obj)\
{\
	assert(obj != nullptr);\
	assert(obj->SUPERVISED_PARENT_MEMBER_NAME == this);\
	for (size_t i = SUPERVISER_VECTOR_MEMBER_NAME.size(); i > 0; --i)\
	{\
		size_t index = i - 1;\
		if (SUPERVISER_VECTOR_MEMBER_NAME[index] == obj)\
			return Remove##SUPERVISED_SHORTNAME##At(index);\
	}\
}\
void SUPERVISER_CLASSNAME::Remove##SUPERVISED_SHORTNAME##At(size_t index)\
{\
	shared_ptr<A> prevent_destruction = objects[index];\
	if constexpr (REMOVE_SWAP)\
	{\
		if (index != SUPERVISER_VECTOR_MEMBER_NAME.size() - 1)\
			std::swap(SUPERVISER_VECTOR_MEMBER_NAME[index], SUPERVISER_VECTOR_MEMBER_NAME.back());\
		SUPERVISER_VECTOR_MEMBER_NAME.pop_back();\
	}\
	else\
	{\
		SUPERVISER_VECTOR_MEMBER_NAME.erase(SUPERVISER_VECTOR_MEMBER_NAME.begin() + index);\
	}\
	prevent_destruction->SUPERVISED_PARENT_MEMBER_NAME = nullptr;\
\
	constexpr auto has_OnRemovedFromFunction = []<typename T>()\
	{\
		return requires(T t)\
		{\
			{ t.OnRemovedFrom##SUPERVISER_SHORTNAME() };\
		};\
	};\
	if constexpr (has_OnRemovedFromFunction.operator()<SUPERVISED_CLASSNAME>())\
		prevent_destruction->OnRemovedFrom##SUPERVISER_SHORTNAME();\
}





#define CHAOS_GENERATE_SUPERVISED_DECLARATION_FUNCTIONS(\
	SUPERVISER_CLASSNAME,\
	SUPERVISED_CLASSNAME,\
	SUPERVISER_SHORTNAME,\
	SUPERVISED_SHORTNAME,\
	SUPERVISER_VECTOR_MEMBER_NAME,\
	SUPERVISED_PARENT_MEMBER_NAME\
)\
friend class SUPERVISER_CLASSNAME;\
public:\
void RemoveFrom##SUPERVISER_SHORTNAME();\
protected:\
virtual void RemoveFromSuperviser() override;\
protected:\
SUPERVISER_CLASSNAME * SUPERVISED_PARENT_MEMBER_NAME = nullptr;


//virtual void OnRemovedFrom##SUPERVISER_SHORTNAME(); \




class A : public SupervisableInterface
{
public:

	CHAOS_GENERATE_SUPERVISED_DECLARATION_FUNCTIONS(B, A, B, A, objects, owner)


	//CHAOS_GENERATE_SUPERVISED_DECLARATION_FUNCTIONS(CHAOS_SUPERVISION_A_B)

};

class B
{
public:

	CHAOS_GENERATE_SUPERVISER_DECLARATION_FUNCTIONS(B, A, B, A, objects, owner)
};

#define CHAOS_GENERATE_SUPERVISED_IMPLEMENTATION_FUNCTIONS(\
	SUPERVISER_CLASSNAME,\
	SUPERVISED_CLASSNAME,\
	SUPERVISER_SHORTNAME,\
	SUPERVISED_SHORTNAME,\
	SUPERVISER_VECTOR_MEMBER_NAME,\
	SUPERVISED_PARENT_MEMBER_NAME,\
	REMOVE_SWAP\
)\
void SUPERVISED_CLASSNAME::RemoveFromSuperviser()\
{\
	RemoveFrom##SUPERVISER_SHORTNAME();\
}\
void SUPERVISED_CLASSNAME::RemoveFrom##SUPERVISER_SHORTNAME()\
{\
	if (SUPERVISED_PARENT_MEMBER_NAME != nullptr)\
	{\
		SUPERVISED_PARENT_MEMBER_NAME->Remove##SUPERVISED_SHORTNAME(this);\
		SUPERVISED_PARENT_MEMBER_NAME = nullptr;\
	}\
}

#if 0
void SUPERVISED_CLASSNAME::OnRemovedFrom##SUPERVISER_SHORTNAME()\
{\
}\

#endif
//-----------------------------------------------------------

CHAOS_GENERATE_SUPERVISER_IMPLEMENTATION_FUNCTIONS(B, A, B, A, objects, owner, true)
CHAOS_GENERATE_SUPERVISED_IMPLEMENTATION_FUNCTIONS(B, A, B, A, objects, owner, true)








int main(int argc, char ** argv, char ** env)
{
	B b;

	A * a = new A;



	chaos::GameApplicationData game_application_data;
	game_application_data.game_class = LudumGame::GetStaticClass();
	return chaos::RunApplication<chaos::GameApplication>(argc, argv, env, &game_application_data);
}