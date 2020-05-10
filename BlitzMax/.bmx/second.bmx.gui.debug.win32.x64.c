#include "second.bmx.gui.debug.win32.x64.h"
static BBString _s13={
	&bbStringClass,
	7,
	{65,65,65,32,61,32,51}
};
static BBString _s0={
	&bbStringClass,
	54,
	{65,110,32,101,108,101,109,101,110,116,32,119,105,116,104,32,116
	,104,101,32,115,97,109,101,32,107,101,121,32,97,108,114,101
	,97,100,121,32,101,120,105,115,116,115,32,105,110,32,116,104
	,101,32,109,97,112}
};
static BBString _s11={
	&bbStringClass,
	4,
	{71,119,101,110}
};
static BBString _s17={
	&bbStringClass,
	6,
	{76,101,110,32,61,32}
};
static BBString _s1={
	&bbStringClass,
	18,
	{84,104,101,32,113,117,101,117,101,32,105,115,32,101,109,112,116
	,121}
};
static BBString _s2={
	&bbStringClass,
	18,
	{84,104,101,32,115,116,97,99,107,32,105,115,32,101,109,112,116
	,121}
};
static BBString _s16={
	&bbStringClass,
	8,
	{99,111,117,110,116,32,61,32}
};
static BBString _s12={
	&bbStringClass,
	11,
	{104,101,108,108,111,32,119,111,114,108,100}
};
static BBString _s3={
	&bbStringClass,
	13,
	{108,105,115,116,32,105,115,32,101,109,112,116,121}
};
static BBString _s8={
	&bbStringClass,
	37,
	{108,111,119,101,114,86,97,108,117,101,32,105,115,32,103,114,101
	,97,116,101,114,32,116,104,97,110,32,117,112,112,101,114,86
	,97,108,117,101}
};
static BBString _s4={
	&bbStringClass,
	4,
	{110,111,100,101}
};
static BBString _s6={
	&bbStringClass,
	25,
	{110,111,100,101,32,105,115,32,102,114,111,109,32,97,110,111,116
	,104,101,114,32,108,105,115,116}
};
static BBString _s5={
	&bbStringClass,
	29,
	{110,111,100,101,32,112,97,114,101,110,116,32,108,105,115,116,32
	,105,115,32,100,105,102,102,101,114,101,110,116}
};
static BBString _s7={
	&bbStringClass,
	5,
	{111,116,104,101,114}
};
static BBString _s15={
	&bbStringClass,
	2,
	{112,117}
};
static BBString _s9={
	&bbStringClass,
	4,
	{116,111,116,111}
};
static BBString _s10={
	&bbStringClass,
	7,
	{116,117,116,117,32,58,32}
};
static BBString _s14={
	&bbStringClass,
	5,
	{120,120,32,58,32}
};
struct BBDebugScope_1{int kind; const char *name; BBDebugDecl decls[2]; };
struct BBDebugScope_2{int kind; const char *name; BBDebugDecl decls[3]; };
struct BBDebugScope_3{int kind; const char *name; BBDebugDecl decls[4]; };
struct BBDebugScope_5{int kind; const char *name; BBDebugDecl decls[6]; };
struct BBDebugScope_9{int kind; const char *name; BBDebugDecl decls[10]; };
void __m_second_Spaceship_New(struct _m_second_Spaceship_obj* o) {
	bbObjectCtor((BBOBJECT)o);
	o->clas = &_m_second_Spaceship;
	((struct _m_second_Spaceship_obj*)bbNullObjectTest(o))->__m_second_spaceship_name = &_s9;
}
BBINT __m_second_Spaceship_ChangeName(struct _m_second_Spaceship_obj* o){
	((struct _m_second_Spaceship_obj*)bbNullObjectTest(o));
	struct BBDebugScope_1 __scope = {
		BBDEBUGSCOPE_FUNCTION,
		"ChangeName",
		{
			{
				BBDEBUGDECL_LOCAL,
				"Self",
				":Spaceship",
				.var_address=&o
			},
			BBDEBUGDECL_END 
		}
	};
	bbOnDebugEnterScope(&__scope);
	struct BBDebugStm __stmt_0 = {0xff8062eca9443a54, 26, 0};
	bbOnDebugEnterStm(&__stmt_0);
	((struct _m_second_Spaceship_obj*)bbNullObjectTest(o))->__m_second_spaceship_name =bbStringConcat(((struct _m_second_Spaceship_obj*)bbNullObjectTest(o))->__m_second_spaceship_name ,((struct _m_second_Spaceship_obj*)bbNullObjectTest(o))->__m_second_spaceship_name );
	bbOnDebugLeaveScope();
	return 0;
}
struct _m_second_Spaceship_obj* _m_second_Spaceship_CreateSpaceship_TSpaceship(){
	struct _m_second_Spaceship_obj* bbt_result=(struct _m_second_Spaceship_obj*)(&bbNullObject);
	struct BBDebugScope_1 __scope = {
		BBDEBUGSCOPE_FUNCTION,
		"CreateSpaceship",
		{
			{
				BBDEBUGDECL_LOCAL,
				"result",
				":Spaceship",
				.var_address=&bbt_result
			},
			BBDEBUGDECL_END 
		}
	};
	bbOnDebugEnterScope(&__scope);
	struct BBDebugStm __stmt_0 = {0xff8062eca9443a54, 32, 0};
	bbOnDebugEnterStm(&__stmt_0);
	bbt_result=(struct _m_second_Spaceship_obj*)(struct _m_second_Spaceship_obj*)bbObjectNew((BBClass *)&_m_second_Spaceship);
	struct BBDebugStm __stmt_1 = {0xff8062eca9443a54, 35, 0};
	bbOnDebugEnterStm(&__stmt_1);
	if(!(((BBObject*)_m_second_Spaceship_ListOfSpaceship)!= &bbNullObject)){
		struct BBDebugScope __scope = {
			BBDEBUGSCOPE_LOCALBLOCK,
			0,
			{
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0xff8062eca9443a54, 36, 0};
		bbOnDebugEnterStm(&__stmt_0);
		_m_second_Spaceship_ListOfSpaceship=(struct brl_linkedlist_TList_obj*)brl_linkedlist_CreateList();
		bbOnDebugLeaveScope();
	}
	struct BBDebugStm __stmt_2 = {0xff8062eca9443a54, 39, 0};
	bbOnDebugEnterStm(&__stmt_2);
	brl_linkedlist_ListAddLast((struct brl_linkedlist_TList_obj*)_m_second_Spaceship_ListOfSpaceship,(BBOBJECT)bbt_result);
	struct BBDebugStm __stmt_3 = {0xff8062eca9443a54, 41, 0};
	bbOnDebugEnterStm(&__stmt_3);
	bbOnDebugLeaveScope();
	return (struct _m_second_Spaceship_obj*)bbt_result;
}
struct brl_linkedlist_TList_obj* _m_second_Spaceship_ListOfSpaceship;
struct BBDebugScope_5 _m_second_Spaceship_scope ={
	BBDEBUGSCOPE_USERTYPE,
	"Spaceship",
	{
		{
			BBDEBUGDECL_FIELD,
			"Name",
			"$",
			.field_offset=offsetof(struct _m_second_Spaceship_obj,__m_second_spaceship_name)
		},
		{
			BBDEBUGDECL_GLOBAL,
			"ListOfSpaceship",
			":TList",
			.var_address=(void*)&_m_second_Spaceship_ListOfSpaceship
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			"New",
			"()i",
			.var_address=(void*)&__m_second_Spaceship_New
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			"ChangeName",
			"()i",
			.var_address=(void*)&__m_second_Spaceship_ChangeName
		},
		{
			BBDEBUGDECL_TYPEFUNCTION,
			"CreateSpaceship",
			"():Spaceship",
			.var_address=(void*)&_m_second_Spaceship_CreateSpaceship_TSpaceship
		},
		BBDEBUGDECL_END
	}
};
struct BBClass__m_second_Spaceship _m_second_Spaceship={
	&bbObjectClass,
	bbObjectFree,
	(BBDebugScope*)&_m_second_Spaceship_scope,
	sizeof(struct _m_second_Spaceship_obj),
	(void (*)(BBOBJECT))__m_second_Spaceship_New,
	bbObjectDtor,
	bbObjectToString,
	bbObjectCompare,
	bbObjectSendMessage,
	0,
	0,
	offsetof(struct _m_second_Spaceship_obj,__m_second_spaceship_name) - sizeof(void*) + sizeof(BBSTRING)
	,0
	,__m_second_Spaceship_ChangeName
	,_m_second_Spaceship_CreateSpaceship_TSpaceship
};

void __m_second_MyType_New(struct _m_second_MyType_obj* o) {
	bbObjectCtor((BBOBJECT)o);
	o->clas = &_m_second_MyType;
	((struct _m_second_MyType_obj*)bbNullObjectTest(o))->__m_second_mytype_value = 3;
}
BBINT __m_second_MyType_toto(struct _m_second_MyType_obj* o){
	((struct _m_second_MyType_obj*)bbNullObjectTest(o));
	struct BBDebugScope_1 __scope = {
		BBDEBUGSCOPE_FUNCTION,
		"toto",
		{
			{
				BBDEBUGDECL_LOCAL,
				"Self",
				":MyType",
				.var_address=&o
			},
			BBDEBUGDECL_END 
		}
	};
	bbOnDebugEnterScope(&__scope);
	struct BBDebugStm __stmt_0 = {0xff8062eca9443a54, 97, 0};
	bbOnDebugEnterStm(&__stmt_0);
	((struct _m_second_MyType_obj*)bbNullObjectTest(o))->__m_second_mytype_value =666;
	struct BBDebugStm __stmt_1 = {0xff8062eca9443a54, 99, 0};
	bbOnDebugEnterStm(&__stmt_1);
	brl_standardio_Print(&_s12);
	bbOnDebugLeaveScope();
	return 0;
}
struct BBDebugScope_3 _m_second_MyType_scope ={
	BBDEBUGSCOPE_USERTYPE,
	"MyType",
	{
		{
			BBDEBUGDECL_FIELD,
			"value",
			"i",
			.field_offset=offsetof(struct _m_second_MyType_obj,__m_second_mytype_value)
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			"New",
			"()i",
			.var_address=(void*)&__m_second_MyType_New
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			"toto",
			"()i",
			.var_address=(void*)&__m_second_MyType_toto
		},
		BBDEBUGDECL_END
	}
};
struct BBClass__m_second_MyType _m_second_MyType={
	&bbObjectClass,
	bbObjectFree,
	(BBDebugScope*)&_m_second_MyType_scope,
	sizeof(struct _m_second_MyType_obj),
	(void (*)(BBOBJECT))__m_second_MyType_New,
	bbObjectDtor,
	bbObjectToString,
	bbObjectCompare,
	bbObjectSendMessage,
	0,
	0,
	offsetof(struct _m_second_MyType_obj,__m_second_mytype_value) - sizeof(void*) + sizeof(BBINT)
	,0
	,__m_second_MyType_toto
};

void __m_second_Spaceship2_New(struct _m_second_Spaceship2_obj* o) {
	bbObjectCtor((BBOBJECT)o);
	o->clas = &_m_second_Spaceship2;
	((struct _m_second_Spaceship2_obj*)bbNullObjectTest(o))->__m_second_spaceship2_name = &_s9;
}
struct BBDebugScope_2 _m_second_Spaceship2_scope ={
	BBDEBUGSCOPE_USERTYPE,
	"Spaceship2",
	{
		{
			BBDEBUGDECL_FIELD,
			"Name",
			"$",
			.field_offset=offsetof(struct _m_second_Spaceship2_obj,__m_second_spaceship2_name)
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			"New",
			"()i",
			.var_address=(void*)&__m_second_Spaceship2_New
		},
		BBDEBUGDECL_END
	}
};
struct BBClass__m_second_Spaceship2 _m_second_Spaceship2={
	&bbObjectClass,
	bbObjectFree,
	(BBDebugScope*)&_m_second_Spaceship2_scope,
	sizeof(struct _m_second_Spaceship2_obj),
	(void (*)(BBOBJECT))__m_second_Spaceship2_New,
	bbObjectDtor,
	bbObjectToString,
	bbObjectCompare,
	bbObjectSendMessage,
	0,
	0,
	offsetof(struct _m_second_Spaceship2_obj,__m_second_spaceship2_name) - sizeof(void*) + sizeof(BBSTRING)
	,0
};

void __m_second_PPP_New(struct _m_second_PPP_obj* o) {
	bbObjectCtor((BBOBJECT)o);
	o->clas = &_m_second_PPP;
}
struct BBDebugScope_1 _m_second_PPP_scope ={
	BBDEBUGSCOPE_USERTYPE,
	"PPP",
	{
		{
			BBDEBUGDECL_TYPEMETHOD,
			"New",
			"()i",
			.var_address=(void*)&__m_second_PPP_New
		},
		BBDEBUGDECL_END
	}
};
struct BBClass__m_second_PPP _m_second_PPP={
	&bbObjectClass,
	bbObjectFree,
	(BBDebugScope*)&_m_second_PPP_scope,
	sizeof(struct _m_second_PPP_obj),
	(void (*)(BBOBJECT))__m_second_PPP_New,
	bbObjectDtor,
	bbObjectToString,
	bbObjectCompare,
	bbObjectSendMessage,
	0,
	0,
	0
	,0
};

void __m_second_MMM_New(struct _m_second_MMM_obj* o) {
	bbObjectCtor((BBOBJECT)o);
	o->clas = &_m_second_MMM;
	((struct _m_second_MMM_obj*)bbNullObjectTest(o))->__m_second_mmm_value = 3;
}
struct BBDebugScope_2 _m_second_MMM_scope ={
	BBDEBUGSCOPE_USERTYPE,
	"MMM",
	{
		{
			BBDEBUGDECL_FIELD,
			"value",
			"i",
			.field_offset=offsetof(struct _m_second_MMM_obj,__m_second_mmm_value)
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			"New",
			"()i",
			.var_address=(void*)&__m_second_MMM_New
		},
		BBDEBUGDECL_END
	}
};
struct BBClass__m_second_MMM _m_second_MMM={
	&bbObjectClass,
	bbObjectFree,
	(BBDebugScope*)&_m_second_MMM_scope,
	sizeof(struct _m_second_MMM_obj),
	(void (*)(BBOBJECT))__m_second_MMM_New,
	bbObjectDtor,
	bbObjectToString,
	bbObjectCompare,
	bbObjectSendMessage,
	0,
	0,
	offsetof(struct _m_second_MMM_obj,__m_second_mmm_value) - sizeof(void*) + sizeof(BBINT)
	,0
};

static int _bb_main_inited = 0;
int _bb_main(){
	if (!_bb_main_inited) {
		_bb_main_inited = 1;
		GC_add_roots(&_m_second_Spaceship_ListOfSpaceship, &_m_second_Spaceship_ListOfSpaceship + 1);
		__bb_brl_blitz_blitz();
		__bb_brl_appstub_appstub();
		__bb_brl_audio_audio();
		__bb_brl_bank_bank();
		__bb_brl_bankstream_bankstream();
		__bb_brl_base64_base64();
		__bb_brl_basic_basic();
		__bb_brl_bmploader_bmploader();
		__bb_brl_clipboard_clipboard();
		__bb_brl_collections_collections();
		__bb_brl_d3d7max2d_d3d7max2d();
		__bb_brl_d3d9max2d_d3d9max2d();
		__bb_brl_directsoundaudio_directsoundaudio();
		__bb_brl_eventqueue_eventqueue();
		__bb_brl_freeaudioaudio_freeaudioaudio();
		__bb_brl_freetypefont_freetypefont();
		__bb_brl_glgraphics_glgraphics();
		__bb_brl_glmax2d_glmax2d();
		__bb_brl_gnet_gnet();
		__bb_brl_jpgloader_jpgloader();
		__bb_brl_map_map();
		__bb_brl_matrix_matrix();
		__bb_brl_maxlua_maxlua();
		__bb_brl_maxunit_maxunit();
		__bb_brl_maxutil_maxutil();
		__bb_brl_objectlist_objectlist();
		__bb_brl_oggloader_oggloader();
		__bb_brl_openalaudio_openalaudio();
		__bb_brl_platform_platform();
		__bb_brl_pngloader_pngloader();
		__bb_brl_quaternion_quaternion();
		__bb_brl_retro_retro();
		__bb_brl_tgaloader_tgaloader();
		__bb_brl_threadpool_threadpool();
		__bb_brl_timer_timer();
		__bb_brl_timerdefault_timerdefault();
		__bb_brl_uuid_uuid();
		__bb_brl_volumes_volumes();
		__bb_brl_wavloader_wavloader();
		__bb_pub_freejoy_freejoy();
		__bb_pub_freeprocess_freeprocess();
		__bb_pub_glad_glad();
		__bb_pub_nx_nx();
		__bb_pub_opengles_opengles();
		__bb_pub_opengles3_opengles3();
		__bb_pub_vulkan_vulkan();
		__bb_pub_xmmintrin_xmmintrin();
		bbObjectRegisterType((BBCLASS)&_m_second_Spaceship);
		bbObjectRegisterType((BBCLASS)&_m_second_MyType);
		bbObjectRegisterType((BBCLASS)&_m_second_Spaceship2);
		bbObjectRegisterType((BBCLASS)&_m_second_PPP);
		bbObjectRegisterType((BBCLASS)&_m_second_MMM);
		bbRegisterSource(0xff8062eca9443a54, "F:/Personnel/Programmation/Code/BlitzMax/second.bmx");
		_m_second_Spaceship_ListOfSpaceship=(&bbNullObject);
		_m_second_Spaceship_ListOfSpaceship=(struct brl_linkedlist_TList_obj*)(&bbNullObject);
		BBARRAY bbt_Grid=(&bbEmptyArray);
		BBARRAY bbt_OtherArray=(&bbEmptyArray);
		BBARRAY bbt_arr0=(&bbEmptyArray);
		struct _m_second_Spaceship_obj* bbt_player1=(struct _m_second_Spaceship_obj*)(&bbNullObject);
		struct _m_second_Spaceship_obj* bbt_player2=(struct _m_second_Spaceship_obj*)(&bbNullObject);
		struct _m_second_Spaceship_obj* bbt_p1=(struct _m_second_Spaceship_obj*)(&bbNullObject);
		BBINT bbt_AAA=0;
		struct brl_linkedlist_TList_obj* bbt_L=(struct brl_linkedlist_TList_obj*)(&bbNullObject);
		BBINT bbt_c=0;
		struct BBDebugScope_9 __scope = {
			BBDEBUGSCOPE_FUNCTION,
			"second",
			{
				{
					BBDEBUGDECL_LOCAL,
					"Grid",
					"[,]$",
					.var_address=&bbt_Grid
				},
				{
					BBDEBUGDECL_LOCAL,
					"OtherArray",
					"[][]i",
					.var_address=&bbt_OtherArray
				},
				{
					BBDEBUGDECL_LOCAL,
					"arr0",
					"[]i",
					.var_address=&bbt_arr0
				},
				{
					BBDEBUGDECL_LOCAL,
					"player1",
					":Spaceship",
					.var_address=&bbt_player1
				},
				{
					BBDEBUGDECL_LOCAL,
					"player2",
					":Spaceship",
					.var_address=&bbt_player2
				},
				{
					BBDEBUGDECL_LOCAL,
					"p1",
					":Spaceship",
					.var_address=&bbt_p1
				},
				{
					BBDEBUGDECL_LOCAL,
					"AAA",
					"i",
					.var_address=&bbt_AAA
				},
				{
					BBDEBUGDECL_LOCAL,
					"L",
					":TList",
					.var_address=&bbt_L
				},
				{
					BBDEBUGDECL_LOCAL,
					"c",
					"i",
					.var_address=&bbt_c
				},
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0xff8062eca9443a54, 2, 0};
		bbOnDebugEnterStm(&__stmt_0);
		brl_graphics_Graphics(800,600,0,60,0,-1,-1);
		struct BBDebugStm __stmt_1 = {0xff8062eca9443a54, 4, 0};
		bbOnDebugEnterStm(&__stmt_1);
		bbt_Grid=bbArrayNew("$", 2, 3, 3);
		struct BBDebugStm __stmt_2 = {0xff8062eca9443a54, 6, 0};
		bbOnDebugEnterStm(&__stmt_2);
		BBUINT* bbt_=((BBARRAY)bbt_Grid)->scales + 1;
		((BBSTRING*)BBARRAYDATAINDEX((bbt_Grid),(bbt_Grid)->dims,(*(bbt_)) * 0U + 0U))[(*(bbt_)) * 0U + 0U]=&_s9;
		struct BBDebugStm __stmt_3 = {0xff8062eca9443a54, 8, 0};
		bbOnDebugEnterStm(&__stmt_3);
		BBINT bbt_4[]={1,1,1};
		BBARRAY bbt_5 = bbArrayFromData("i",3,bbt_4);
		BBINT bbt_6[]={2,2};
		BBARRAY bbt_7 = bbArrayFromData("i",2,bbt_6);
		BBARRAY bbt_2[]={bbt_5,bbt_7};
		BBARRAY bbt_3 = bbArrayFromData("[]i",2,bbt_2);
		bbt_OtherArray=bbt_3;
		struct BBDebugStm __stmt_4 = {0xff8062eca9443a54, 11, 0};
		bbOnDebugEnterStm(&__stmt_4);
		BBINT bbt_8[]={0,1,2,3,4,5};
		BBARRAY bbt_9 = bbArrayFromData("i",6,bbt_8);
		bbt_arr0=bbt_9;
		struct BBDebugStm __stmt_5 = {0xff8062eca9443a54, 13, 0};
		bbOnDebugEnterStm(&__stmt_5);
		bbt_arr0=bbArraySlice("i",bbt_arr0,4,(bbt_arr0)->scales[0]);
		struct BBDebugStm __stmt_6 = {0xff8062eca9443a54, 14, 0};
		bbOnDebugEnterStm(&__stmt_6);
		bbArraySort((struct bbArrayClass_obj*)bbt_arr0,1);
		struct BBDebugStm __stmt_7 = {0xff8062eca9443a54, 57, 0};
		bbOnDebugEnterStm(&__stmt_7);
		bbt_player1=(struct _m_second_Spaceship_obj*)_m_second_Spaceship_CreateSpaceship_TSpaceship();
		struct BBDebugStm __stmt_8 = {0xff8062eca9443a54, 58, 0};
		bbOnDebugEnterStm(&__stmt_8);
		bbt_player2=(struct _m_second_Spaceship_obj*)bbt_player1;
		struct BBDebugStm __stmt_9 = {0xff8062eca9443a54, 61, 0};
		bbOnDebugEnterStm(&__stmt_9);
		brl_standardio_Print(bbStringConcat(&_s10,bbStringFromInt(((struct brl_linkedlist_TList_obj*)bbNullObjectTest(_m_second_Spaceship_ListOfSpaceship))->clas->m_Count((struct brl_linkedlist_TList_obj*)_m_second_Spaceship_ListOfSpaceship))));
		struct BBDebugStm __stmt_10 = {0xff8062eca9443a54, 63, 0};
		bbOnDebugEnterStm(&__stmt_10);
		bbt_p1=(struct _m_second_Spaceship_obj*)((struct _m_second_Spaceship_obj*)bbObjectDowncast((BBOBJECT)((struct brl_linkedlist_TList_obj*)bbNullObjectTest(_m_second_Spaceship_ListOfSpaceship))->clas->m_First((struct brl_linkedlist_TList_obj*)_m_second_Spaceship_ListOfSpaceship),(BBClass*)&_m_second_Spaceship));
		struct BBDebugStm __stmt_11 = {0xff8062eca9443a54, 64, 0};
		bbOnDebugEnterStm(&__stmt_11);
		((struct _m_second_Spaceship_obj*)bbNullObjectTest(bbt_p1))->__m_second_spaceship_name =&_s11;
		struct BBDebugStm __stmt_12 = {0xff8062eca9443a54, 71, 0};
		bbOnDebugEnterStm(&__stmt_12);
		((struct _m_second_Spaceship_obj*)bbNullObjectTest(bbt_player1))->clas->m_ChangeName((struct _m_second_Spaceship_obj*)bbt_player1);
		struct BBDebugStm __stmt_13 = {0xff8062eca9443a54, 72, 0};
		bbOnDebugEnterStm(&__stmt_13);
		((struct _m_second_Spaceship_obj*)bbNullObjectTest(bbt_player1))->clas->m_ChangeName((struct _m_second_Spaceship_obj*)bbt_player1);
		struct BBDebugStm __stmt_14 = {0xff8062eca9443a54, 73, 0};
		bbOnDebugEnterStm(&__stmt_14);
		((struct _m_second_Spaceship_obj*)bbNullObjectTest(bbt_player1))->clas->m_ChangeName((struct _m_second_Spaceship_obj*)bbt_player1);
		struct BBDebugStm __stmt_15 = {0xff8062eca9443a54, 74, 0};
		bbOnDebugEnterStm(&__stmt_15);
		brl_standardio_Print(((struct _m_second_Spaceship_obj*)bbNullObjectTest(bbt_player2))->__m_second_spaceship_name );
		struct BBDebugStm __stmt_16 = {0xff8062eca9443a54, 82, 0};
		bbOnDebugEnterStm(&__stmt_16);
		do{
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			bbOnDebugLeaveScope();
		}while(!(1));
		struct BBDebugStm __stmt_17 = {0xff8062eca9443a54, 107, 0};
		bbOnDebugEnterStm(&__stmt_17);
		_m_second_MyType.m_toto((struct _m_second_MyType_obj*)((struct _m_second_MyType_obj*)bbObjectAtomicNew((BBClass *)&_m_second_MyType)));
		struct BBDebugStm __stmt_18 = {0xff8062eca9443a54, 112, 0};
		bbOnDebugEnterStm(&__stmt_18);
		bbt_AAA=3;
		struct BBDebugStm __stmt_19 = {0xff8062eca9443a54, 114, 0};
		bbOnDebugEnterStm(&__stmt_19);
		BBINT bbt_10=bbt_AAA;
		struct BBDebugStm __stmt_20 = {0xff8062eca9443a54, 116, 0};
		bbOnDebugEnterStm(&__stmt_20);
		if(bbt_10==3){
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0xff8062eca9443a54, 117, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_standardio_Print(&_s13);
			bbOnDebugLeaveScope();
		}
		struct BBDebugStm __stmt_21 = {0xff8062eca9443a54, 124, 0};
		bbOnDebugEnterStm(&__stmt_21);
		bbt_L=(struct brl_linkedlist_TList_obj*)brl_linkedlist_CreateList();
		struct BBDebugStm __stmt_22 = {0xff8062eca9443a54, 125, 0};
		bbOnDebugEnterStm(&__stmt_22);
		brl_linkedlist_ListAddLast((struct brl_linkedlist_TList_obj*)bbt_L,(BBOBJECT)(struct _m_second_Spaceship2_obj*)bbObjectNew((BBClass *)&_m_second_Spaceship2));
		struct BBDebugStm __stmt_23 = {0xff8062eca9443a54, 126, 0};
		bbOnDebugEnterStm(&__stmt_23);
		brl_linkedlist_ListAddLast((struct brl_linkedlist_TList_obj*)bbt_L,(BBOBJECT)bbt_player1);
		struct BBDebugStm __stmt_24 = {0xff8062eca9443a54, 127, 0};
		bbOnDebugEnterStm(&__stmt_24);
		brl_linkedlist_ListAddLast((struct brl_linkedlist_TList_obj*)bbt_L,(BBOBJECT)(struct _m_second_Spaceship2_obj*)bbObjectNew((BBClass *)&_m_second_Spaceship2));
		struct BBDebugStm __stmt_25 = {0xff8062eca9443a54, 128, 0};
		bbOnDebugEnterStm(&__stmt_25);
		brl_linkedlist_ListAddLast((struct brl_linkedlist_TList_obj*)bbt_L,(BBOBJECT)bbt_player1);
		struct BBDebugStm __stmt_26 = {0xff8062eca9443a54, 131, 0};
		bbOnDebugEnterStm(&__stmt_26);
		struct brl_linkedlist_TListEnum_obj* bbt_11=(struct brl_linkedlist_TListEnum_obj*)((struct brl_linkedlist_TList_obj*)bbNullObjectTest(bbt_L))->clas->m_ObjectEnumerator((struct brl_linkedlist_TList_obj*)bbt_L);
		while(((struct brl_linkedlist_TListEnum_obj*)bbNullObjectTest(bbt_11))->clas->m_HasNext((struct brl_linkedlist_TListEnum_obj*)bbt_11)!=0){
			struct _m_second_Spaceship_obj* bbt_o=(struct _m_second_Spaceship_obj*)(&bbNullObject);
			struct BBDebugScope_1 __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					{
						BBDEBUGDECL_LOCAL,
						"o",
						":Spaceship",
						.var_address=&bbt_o
					},
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			bbt_o=(struct _m_second_Spaceship_obj*)((struct _m_second_Spaceship_obj*)bbObjectDowncast((BBOBJECT)((struct brl_linkedlist_TListEnum_obj*)bbNullObjectTest(bbt_11))->clas->m_NextObject((struct brl_linkedlist_TListEnum_obj*)bbt_11),(BBClass*)&_m_second_Spaceship));
			if(bbt_o==&bbNullObject){
				bbOnDebugLeaveScope();
				continue;
			}
			struct BBDebugStm __stmt_0 = {0xff8062eca9443a54, 133, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_standardio_Print(bbStringConcat(&_s14,((struct _m_second_Spaceship_obj*)bbNullObjectTest(bbt_o))->__m_second_spaceship_name ));
			struct BBDebugStm __stmt_1 = {0xff8062eca9443a54, 134, 0};
			bbOnDebugEnterStm(&__stmt_1);
			brl_standardio_Print(&_s15);
			bbOnDebugLeaveScope();
		}
		struct BBDebugStm __stmt_27 = {0xff8062eca9443a54, 139, 0};
		bbOnDebugEnterStm(&__stmt_27);
		bbt_c=((struct brl_linkedlist_TList_obj*)bbNullObjectTest(bbt_L))->clas->m_Count((struct brl_linkedlist_TList_obj*)bbt_L);
		struct BBDebugStm __stmt_28 = {0xff8062eca9443a54, 140, 0};
		bbOnDebugEnterStm(&__stmt_28);
		brl_standardio_Print(bbStringConcat(&_s16,bbStringFromInt(bbt_c)));
		struct BBDebugStm __stmt_29 = {0xff8062eca9443a54, 143, 0};
		bbOnDebugEnterStm(&__stmt_29);
		BBARRAY bbt_12=bbt_arr0;
		BBUINT bbt_13=0U;
		while(bbt_13<((BBUINT)(bbt_12->scales[0]))){
			BBINT bbt_i=0;
			struct BBDebugScope_1 __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					{
						BBDEBUGDECL_LOCAL,
						"i",
						"i",
						.var_address=&bbt_i
					},
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			bbt_i=((BBINT*)BBARRAYDATAINDEX((bbt_12),(bbt_12)->dims,bbt_13))[bbt_13];
			bbt_13=(bbt_13+1U);
			struct BBDebugStm __stmt_0 = {0xff8062eca9443a54, 144, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_standardio_Print(bbStringFromInt(bbt_i));
			bbOnDebugLeaveScope();
		}
		struct BBDebugStm __stmt_30 = {0xff8062eca9443a54, 147, 0};
		bbOnDebugEnterStm(&__stmt_30);
		brl_standardio_Print(bbStringConcat(&_s17,bbStringFromInt(bbt_arr0->scales[0])));
		struct BBDebugStm __stmt_31 = {0xff8062eca9443a54, 150, 0};
		bbOnDebugEnterStm(&__stmt_31);
		while(!(brl_polledinput_KeyDown(27)!=0)){
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0xff8062eca9443a54, 153, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_graphics_Flip(-1);
			struct BBDebugStm __stmt_1 = {0xff8062eca9443a54, 153, 0};
			bbOnDebugEnterStm(&__stmt_1);
			brl_max2d_Cls();
			bbOnDebugLeaveScope();
		}
		bbOnDebugLeaveScope();
		return 0;
	}
	return 0;
}