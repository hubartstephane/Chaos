#include "sixth.bmx.gui.debug.win32.x64.h"
static BBString _s0={
	&bbStringClass,
	54,
	{65,110,32,101,108,101,109,101,110,116,32,119,105,116,104,32,116
	,104,101,32,115,97,109,101,32,107,101,121,32,97,108,114,101
	,97,100,121,32,101,120,105,115,116,115,32,105,110,32,116,104
	,101,32,109,97,112}
};
static BBString _s9={
	&bbStringClass,
	5,
	{72,101,108,108,111}
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
struct BBDebugScope_1{int kind; const char *name; BBDebugDecl decls[2]; };
struct BBDebugScope_2{int kind; const char *name; BBDebugDecl decls[3]; };
struct BBDebugScope_5{int kind; const char *name; BBDebugDecl decls[6]; };
struct BBDebugScope_7{int kind; const char *name; BBDebugDecl decls[8]; };
void __m_sixth_Player_New_i(struct _m_sixth_Player_obj* o,BBINT bbt_index) {
	bbObjectCtor((BBOBJECT)o);
	o->clas = &_m_sixth_Player;
	((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_position = (struct brl_vector_SVec2F)brl_vector_SVec2F_New_ff_ObjectNew(0.0f,0.0f);
	((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_index = 0;
	struct BBDebugScope_1 __scope = {
		BBDEBUGSCOPE_FUNCTION,
		"New",
		{
			{
				BBDEBUGDECL_LOCAL,
				"index",
				"i",
				.var_address=&bbt_index
			},
			BBDEBUGDECL_END 
		}
	};
	bbOnDebugEnterScope(&__scope);
	struct BBDebugStm __stmt_0 = {0xbf8b04d2cb307065, 14, 0};
	bbOnDebugEnterStm(&__stmt_0);
	((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_index =bbt_index;
	struct BBDebugStm __stmt_1 = {0xbf8b04d2cb307065, 15, 0};
	bbOnDebugEnterStm(&__stmt_1);
	((struct brl_linkedlist_TList_obj*)bbNullObjectTest(_m_sixth_Player_Players))->clas->m_AddLast_TObject((struct brl_linkedlist_TList_obj*)_m_sixth_Player_Players,(BBOBJECT)o);
	bbOnDebugLeaveScope();
}
struct _m_sixth_Player_obj* __m_sixth_Player_New_i_ObjectNew(BBClass * clas,BBINT bbt_index) {
	struct _m_sixth_Player_obj* o = bbObjectAtomicNewNC(clas);
	__m_sixth_Player_New_i(o, bbt_index);
	return o;
}
void __m_sixth_Player_New(struct _m_sixth_Player_obj* o) {
	bbObjectCtor((BBOBJECT)o);
	o->clas = &_m_sixth_Player;
	((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_position = (struct brl_vector_SVec2F)brl_vector_SVec2F_New_ff_ObjectNew(0.0f,0.0f);
	((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_index = 0;
}
BBINT __m_sixth_Player_Update(struct _m_sixth_Player_obj* o){
	((struct _m_sixth_Player_obj*)bbNullObjectTest(o));
	BBFLOAT bbt_jx=0;
	BBFLOAT bbt_jy=0;
	BBFLOAT bbt_DeathZone=0;
	BBFLOAT bbt_speed=0;
	struct BBDebugScope_5 __scope = {
		BBDEBUGSCOPE_FUNCTION,
		"Update",
		{
			{
				BBDEBUGDECL_LOCAL,
				"Self",
				":Player",
				.var_address=&o
			},
			{
				BBDEBUGDECL_LOCAL,
				"jx",
				"f",
				.var_address=&bbt_jx
			},
			{
				BBDEBUGDECL_LOCAL,
				"jy",
				"f",
				.var_address=&bbt_jy
			},
			{
				BBDEBUGDECL_LOCAL,
				"DeathZone",
				"f",
				.var_address=&bbt_DeathZone
			},
			{
				BBDEBUGDECL_LOCAL,
				"speed",
				"f",
				.var_address=&bbt_speed
			},
			BBDEBUGDECL_END 
		}
	};
	bbOnDebugEnterScope(&__scope);
	struct BBDebugStm __stmt_0 = {0xbf8b04d2cb307065, 21, 0};
	bbOnDebugEnterStm(&__stmt_0);
	bbt_jx=pub_joystick_JoyX(((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_index );
	struct BBDebugStm __stmt_1 = {0xbf8b04d2cb307065, 22, 0};
	bbOnDebugEnterStm(&__stmt_1);
	bbt_jy=pub_joystick_JoyY(((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_index );
	struct BBDebugStm __stmt_2 = {0xbf8b04d2cb307065, 24, 0};
	bbOnDebugEnterStm(&__stmt_2);
	bbt_DeathZone=0.3f;
	struct BBDebugStm __stmt_3 = {0xbf8b04d2cb307065, 26, 0};
	bbOnDebugEnterStm(&__stmt_3);
	if((bbt_jx<bbt_DeathZone) && (bbt_jx>-bbt_DeathZone)){
		struct BBDebugScope __scope = {
			BBDEBUGSCOPE_LOCALBLOCK,
			0,
			{
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0xbf8b04d2cb307065, 27, 0};
		bbOnDebugEnterStm(&__stmt_0);
		bbt_jx=0.000000000f;
		bbOnDebugLeaveScope();
	}
	struct BBDebugStm __stmt_4 = {0xbf8b04d2cb307065, 29, 0};
	bbOnDebugEnterStm(&__stmt_4);
	if((bbt_jy<bbt_DeathZone) && (bbt_jy>-bbt_DeathZone)){
		struct BBDebugScope __scope = {
			BBDEBUGSCOPE_LOCALBLOCK,
			0,
			{
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0xbf8b04d2cb307065, 30, 0};
		bbOnDebugEnterStm(&__stmt_0);
		bbt_jy=0.000000000f;
		bbOnDebugLeaveScope();
	}
	struct BBDebugStm __stmt_5 = {0xbf8b04d2cb307065, 33, 0};
	bbOnDebugEnterStm(&__stmt_5);
	bbt_speed=5.0f;
	struct BBDebugStm __stmt_6 = {0xbf8b04d2cb307065, 35, 0};
	bbOnDebugEnterStm(&__stmt_6);
	((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_position =brl_vector_SVec2F_New_ff_ObjectNew((((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_position ._brl_vector_svec2f_x +(bbt_jx*bbt_speed)),(((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_position ._brl_vector_svec2f_y +(bbt_jy*bbt_speed)));
	bbOnDebugLeaveScope();
	return 0;
}
BBINT __m_sixth_Player_Draw(struct _m_sixth_Player_obj* o){
	((struct _m_sixth_Player_obj*)bbNullObjectTest(o));
	BBFLOAT bbt_size=0;
	struct BBDebugScope_2 __scope = {
		BBDEBUGSCOPE_FUNCTION,
		"Draw",
		{
			{
				BBDEBUGDECL_LOCAL,
				"Self",
				":Player",
				.var_address=&o
			},
			{
				BBDEBUGDECL_LOCAL,
				"size",
				"f",
				.var_address=&bbt_size
			},
			BBDEBUGDECL_END 
		}
	};
	bbOnDebugEnterScope(&__scope);
	struct BBDebugStm __stmt_0 = {0xbf8b04d2cb307065, 45, 0};
	bbOnDebugEnterStm(&__stmt_0);
	brl_max2d_SetColor(255,255,255);
	struct BBDebugStm __stmt_1 = {0xbf8b04d2cb307065, 47, 0};
	bbOnDebugEnterStm(&__stmt_1);
	bbt_size=40.0000000f;
	struct BBDebugStm __stmt_2 = {0xbf8b04d2cb307065, 48, 0};
	bbOnDebugEnterStm(&__stmt_2);
	brl_max2d_DrawRect((((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_position ._brl_vector_svec2f_x -(bbt_size/2.00000000f)),(((struct _m_sixth_Player_obj*)bbNullObjectTest(o))->__m_sixth_player_position ._brl_vector_svec2f_y -(bbt_size/2.00000000f)),bbt_size,bbt_size);
	bbOnDebugLeaveScope();
	return 0;
}
struct brl_linkedlist_TList_obj* _m_sixth_Player_Players;
struct BBDebugScope_7 _m_sixth_Player_scope ={
	BBDEBUGSCOPE_USERTYPE,
	"Player",
	{
		{
			BBDEBUGDECL_FIELD,
			"position",
			"@SVec2F",
			.field_offset=offsetof(struct _m_sixth_Player_obj,__m_sixth_player_position)
		},
		{
			BBDEBUGDECL_FIELD,
			"index",
			"i",
			.field_offset=offsetof(struct _m_sixth_Player_obj,__m_sixth_player_index)
		},
		{
			BBDEBUGDECL_GLOBAL,
			"Players",
			":TList",
			.var_address=(void*)&_m_sixth_Player_Players
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			"New",
			"()i",
			.var_address=(void*)&__m_sixth_Player_New
		},
		{
			BBDEBUGDECL_TYPEFUNCTION,
			"New",
			"(i):Player",
			.var_address=(void*)&__m_sixth_Player_New_i
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			"Update",
			"()i",
			.var_address=(void*)&__m_sixth_Player_Update
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			"Draw",
			"()i",
			.var_address=(void*)&__m_sixth_Player_Draw
		},
		BBDEBUGDECL_END
	}
};
struct BBClass__m_sixth_Player _m_sixth_Player={
	&bbObjectClass,
	bbObjectFree,
	(BBDebugScope*)&_m_sixth_Player_scope,
	sizeof(struct _m_sixth_Player_obj),
	(void (*)(BBOBJECT))__m_sixth_Player_New,
	bbObjectDtor,
	bbObjectToString,
	bbObjectCompare,
	bbObjectSendMessage,
	0,
	0,
	offsetof(struct _m_sixth_Player_obj,__m_sixth_player_index) - sizeof(void*) + sizeof(BBINT)
	,0
	,__m_sixth_Player_Update
	,__m_sixth_Player_Draw
};

static int _bb_main_inited = 0;
int _bb_main(){
	if (!_bb_main_inited) {
		_bb_main_inited = 1;
		GC_add_roots(&_m_sixth_Player_Players, &_m_sixth_Player_Players + 1);
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
		bbObjectRegisterType((BBCLASS)&_m_sixth_Player);
		bbRegisterSource(0xbf8b04d2cb307065, "F:/Personnel/Programmation/Code/BlitzMax/sixth.bmx");
		_m_sixth_Player_Players=(&bbNullObject);
		_m_sixth_Player_Players=(struct brl_linkedlist_TList_obj*)(struct brl_linkedlist_TList_obj*)bbObjectNew((BBClass *)&brl_linkedlist_TList);
		struct BBDebugScope __scope = {
			BBDEBUGSCOPE_FUNCTION,
			"sixth",
			{
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0xbf8b04d2cb307065, 1, 0};
		bbOnDebugEnterStm(&__stmt_0);
		brl_graphics_Graphics(800,600,0,60,0,-1,-1);
		struct BBDebugStm __stmt_1 = {0xbf8b04d2cb307065, 3, 0};
		bbOnDebugEnterStm(&__stmt_1);
		brl_max2d_SetClsColor(255,0,0);
		struct BBDebugStm __stmt_2 = {0xbf8b04d2cb307065, 56, 0};
		bbOnDebugEnterStm(&__stmt_2);
		__m_sixth_Player_New_i_ObjectNew((struct _m_sixth_Player_obj*)&_m_sixth_Player,0);
		struct BBDebugStm __stmt_3 = {0xbf8b04d2cb307065, 57, 0};
		bbOnDebugEnterStm(&__stmt_3);
		__m_sixth_Player_New_i_ObjectNew((struct _m_sixth_Player_obj*)&_m_sixth_Player,1);
		struct BBDebugStm __stmt_4 = {0xbf8b04d2cb307065, 64, 0};
		bbOnDebugEnterStm(&__stmt_4);
		while(!(brl_polledinput_KeyDown(27)!=0) && !(brl_polledinput_AppTerminate()!=0)){
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0xbf8b04d2cb307065, 67, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_max2d_Cls();
			struct BBDebugStm __stmt_1 = {0xbf8b04d2cb307065, 68, 0};
			bbOnDebugEnterStm(&__stmt_1);
			struct brl_linkedlist_TListEnum_obj* bbt_=(struct brl_linkedlist_TListEnum_obj*)((struct brl_linkedlist_TList_obj*)bbNullObjectTest(_m_sixth_Player_Players))->clas->m_ObjectEnumerator((struct brl_linkedlist_TList_obj*)_m_sixth_Player_Players);
			while(((struct brl_linkedlist_TListEnum_obj*)bbNullObjectTest(bbt_))->clas->m_HasNext((struct brl_linkedlist_TListEnum_obj*)bbt_)!=0){
				struct _m_sixth_Player_obj* bbt_p=(struct _m_sixth_Player_obj*)(&bbNullObject);
				struct BBDebugScope_1 __scope = {
					BBDEBUGSCOPE_LOCALBLOCK,
					0,
					{
						{
							BBDEBUGDECL_LOCAL,
							"p",
							":Player",
							.var_address=&bbt_p
						},
						BBDEBUGDECL_END 
					}
				};
				bbOnDebugEnterScope(&__scope);
				bbt_p=(struct _m_sixth_Player_obj*)((struct _m_sixth_Player_obj*)bbObjectDowncast((BBOBJECT)((struct brl_linkedlist_TListEnum_obj*)bbNullObjectTest(bbt_))->clas->m_NextObject((struct brl_linkedlist_TListEnum_obj*)bbt_),(BBClass*)&_m_sixth_Player));
				if(bbt_p==&bbNullObject){
					bbOnDebugLeaveScope();
					continue;
				}
				struct BBDebugStm __stmt_0 = {0xbf8b04d2cb307065, 69, 0};
				bbOnDebugEnterStm(&__stmt_0);
				((struct _m_sixth_Player_obj*)bbNullObjectTest(bbt_p))->clas->m_Draw((struct _m_sixth_Player_obj*)bbt_p);
				bbOnDebugLeaveScope();
			}
			struct BBDebugStm __stmt_2 = {0xbf8b04d2cb307065, 72, 0};
			bbOnDebugEnterStm(&__stmt_2);
			brl_graphics_Flip(-1);
			struct BBDebugStm __stmt_3 = {0xbf8b04d2cb307065, 74, 0};
			bbOnDebugEnterStm(&__stmt_3);
			struct brl_linkedlist_TListEnum_obj* bbt_2=(struct brl_linkedlist_TListEnum_obj*)((struct brl_linkedlist_TList_obj*)bbNullObjectTest(_m_sixth_Player_Players))->clas->m_ObjectEnumerator((struct brl_linkedlist_TList_obj*)_m_sixth_Player_Players);
			while(((struct brl_linkedlist_TListEnum_obj*)bbNullObjectTest(bbt_2))->clas->m_HasNext((struct brl_linkedlist_TListEnum_obj*)bbt_2)!=0){
				struct _m_sixth_Player_obj* bbt_p2=(struct _m_sixth_Player_obj*)(&bbNullObject);
				struct BBDebugScope_1 __scope = {
					BBDEBUGSCOPE_LOCALBLOCK,
					0,
					{
						{
							BBDEBUGDECL_LOCAL,
							"p",
							":Player",
							.var_address=&bbt_p2
						},
						BBDEBUGDECL_END 
					}
				};
				bbOnDebugEnterScope(&__scope);
				bbt_p2=(struct _m_sixth_Player_obj*)((struct _m_sixth_Player_obj*)bbObjectDowncast((BBOBJECT)((struct brl_linkedlist_TListEnum_obj*)bbNullObjectTest(bbt_2))->clas->m_NextObject((struct brl_linkedlist_TListEnum_obj*)bbt_2),(BBClass*)&_m_sixth_Player));
				if(bbt_p2==&bbNullObject){
					bbOnDebugLeaveScope();
					continue;
				}
				struct BBDebugStm __stmt_0 = {0xbf8b04d2cb307065, 75, 0};
				bbOnDebugEnterStm(&__stmt_0);
				((struct _m_sixth_Player_obj*)bbNullObjectTest(bbt_p2))->clas->m_Update((struct _m_sixth_Player_obj*)bbt_p2);
				bbOnDebugLeaveScope();
			}
			bbOnDebugLeaveScope();
		}
		struct BBDebugStm __stmt_5 = {0xbf8b04d2cb307065, 83, 0};
		bbOnDebugEnterStm(&__stmt_5);
		brl_standardio_Print(&_s9);
		bbOnDebugLeaveScope();
		return 0;
	}
	return 0;
}