#include "third.bmx.gui.debug.win32.x64.h"
static BBString _s0={
	&bbStringClass,
	54,
	{65,110,32,101,108,101,109,101,110,116,32,119,105,116,104,32,116
	,104,101,32,115,97,109,101,32,107,101,121,32,97,108,114,101
	,97,100,121,32,101,120,105,115,116,115,32,105,110,32,116,104
	,101,32,109,97,112}
};
static BBString _s15={
	&bbStringClass,
	6,
	{70,80,83,32,58,32}
};
static BBString _s9={
	&bbStringClass,
	13,
	{70,114,101,101,65,117,100,105,111,32,83,68,76}
};
static BBString _s17={
	&bbStringClass,
	9,
	{72,101,105,103,104,116,32,58,32}
};
static BBString _s12={
	&bbStringClass,
	5,
	{72,101,108,108,111}
};
static BBString _s13={
	&bbStringClass,
	12,
	{73,109,97,103,101,32,99,104,97,114,103,101}
};
static BBString _s14={
	&bbStringClass,
	12,
	{83,111,117,110,100,32,99,104,97,114,103,101}
};
static BBString _s10={
	&bbStringClass,
	10,
	{83,112,114,105,116,101,46,112,110,103}
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
	9,
	{87,105,100,116,104,32,32,58,32}
};
static BBString _s11={
	&bbStringClass,
	8,
	{100,114,117,109,46,119,97,118}
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
struct BBDebugScope_3{int kind; const char *name; BBDebugDecl decls[4]; };
void __m_third_TT_New(struct _m_third_TT_obj* o) {
	bbObjectCtor((BBOBJECT)o);
	o->clas = &_m_third_TT;
}
struct BBDebugScope_1 _m_third_TT_scope ={
	BBDEBUGSCOPE_USERTYPE,
	"TT",
	{
		{
			BBDEBUGDECL_TYPEMETHOD,
			"New",
			"()i",
			.var_address=(void*)&__m_third_TT_New
		},
		BBDEBUGDECL_END
	}
};
struct BBClass__m_third_TT _m_third_TT={
	&bbObjectClass,
	bbObjectFree,
	(BBDebugScope*)&_m_third_TT_scope,
	sizeof(struct _m_third_TT_obj),
	(void (*)(BBOBJECT))__m_third_TT_New,
	bbObjectDtor,
	bbObjectToString,
	bbObjectCompare,
	bbObjectSendMessage,
	0,
	0,
	0
	,0
};

void __m_third_T_New(struct _m_third_T_obj* o) {
	bbObjectCtor((BBOBJECT)o);
	o->clas = &_m_third_T;
	struct BBDebugScope __scope = {
		BBDEBUGSCOPE_FUNCTION,
		"New",
		{
			BBDEBUGDECL_END 
		}
	};
	bbOnDebugEnterScope(&__scope);
	struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 46, 0};
	bbOnDebugEnterStm(&__stmt_0);
	brl_standardio_Print(&_s12);
	bbOnDebugLeaveScope();
}
BBINT __m_third_T__addeq_TT(struct _m_third_T_obj* o,struct _m_third_T_obj* bbt_other){
	((struct _m_third_T_obj*)bbNullObjectTest(o));
	struct BBDebugScope_2 __scope = {
		BBDEBUGSCOPE_FUNCTION,
		":+",
		{
			{
				BBDEBUGDECL_LOCAL,
				"Self",
				":T",
				.var_address=&o
			},
			{
				BBDEBUGDECL_LOCAL,
				"other",
				":T",
				.var_address=&bbt_other
			},
			BBDEBUGDECL_END 
		}
	};
	bbOnDebugEnterScope(&__scope);
	struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 39, 0};
	bbOnDebugEnterStm(&__stmt_0);
	bbOnDebugLeaveScope();
	return 3;
}
struct BBDebugScope_2 _m_third_T_scope ={
	BBDEBUGSCOPE_USERTYPE,
	"T",
	{
		{
			BBDEBUGDECL_TYPEMETHOD,
			"New",
			"()i",
			.var_address=(void*)&__m_third_T_New
		},
		{
			BBDEBUGDECL_TYPEMETHOD,
			":+",
			"(:T)i",
			.var_address=(void*)&__m_third_T__addeq_TT
		},
		BBDEBUGDECL_END
	}
};
struct BBClass__m_third_T _m_third_T={
	&bbObjectClass,
	bbObjectFree,
	(BBDebugScope*)&_m_third_T_scope,
	sizeof(struct _m_third_T_obj),
	(void (*)(BBOBJECT))__m_third_T_New,
	bbObjectDtor,
	bbObjectToString,
	bbObjectCompare,
	bbObjectSendMessage,
	0,
	0,
	0
	,0
	,__m_third_T__addeq_TT
};

static int _bb_main_inited = 0;
int _bb_main(){
	if (!_bb_main_inited) {
		_bb_main_inited = 1;
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
		bbObjectRegisterType((BBCLASS)&_m_third_TT);
		bbObjectRegisterType((BBCLASS)&_m_third_T);
		bbRegisterSource(0xfb255226cf53a02b, "F:/Personnel/Programmation/Code/BlitzMax/third.bmx");
		struct brl_max2d_image_TImage_obj* bbt_Img=(struct brl_max2d_image_TImage_obj*)(&bbNullObject);
		BBINT bbt_x=0;
		struct brl_audio_TSound_obj* bbt_Snd=(struct brl_audio_TSound_obj*)(&bbNullObject);
		struct BBDebugScope_3 __scope = {
			BBDEBUGSCOPE_FUNCTION,
			"third",
			{
				{
					BBDEBUGDECL_LOCAL,
					"Img",
					":TImage",
					.var_address=&bbt_Img
				},
				{
					BBDEBUGDECL_LOCAL,
					"x",
					"i",
					.var_address=&bbt_x
				},
				{
					BBDEBUGDECL_LOCAL,
					"Snd",
					":TSound",
					.var_address=&bbt_Snd
				},
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 3, 0};
		bbOnDebugEnterStm(&__stmt_0);
		brl_graphics_Graphics(800,600,0,60,0,-1,-1);
		struct BBDebugStm __stmt_1 = {0xfb255226cf53a02b, 10, 0};
		bbOnDebugEnterStm(&__stmt_1);
		brl_audio_SetAudioDriver(&_s9);
		struct BBDebugStm __stmt_2 = {0xfb255226cf53a02b, 12, 0};
		bbOnDebugEnterStm(&__stmt_2);
		bbt_Img=(struct brl_max2d_image_TImage_obj*)brl_max2d_LoadImage((BBOBJECT)&_s10,-1);
		struct BBDebugStm __stmt_3 = {0xfb255226cf53a02b, 14, 0};
		bbOnDebugEnterStm(&__stmt_3);
		bbt_x=brl_polledinput_MouseX();
		struct BBDebugStm __stmt_4 = {0xfb255226cf53a02b, 16, 0};
		bbOnDebugEnterStm(&__stmt_4);
		bbt_Snd=(struct brl_audio_TSound_obj*)brl_audio_LoadSound((BBOBJECT)&_s11,0);
		struct BBDebugStm __stmt_5 = {0xfb255226cf53a02b, 25, 0};
		bbOnDebugEnterStm(&__stmt_5);
		brl_max2d_SetScale(2.0f,2.0f);
		struct BBDebugStm __stmt_6 = {0xfb255226cf53a02b, 57, 0};
		bbOnDebugEnterStm(&__stmt_6);
		if(((BBObject*)bbt_Img)!= &bbNullObject){
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 58, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_standardio_Print(&_s13);
			bbOnDebugLeaveScope();
		}
		struct BBDebugStm __stmt_7 = {0xfb255226cf53a02b, 61, 0};
		bbOnDebugEnterStm(&__stmt_7);
		if(((BBObject*)bbt_Snd)!= &bbNullObject){
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 62, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_standardio_Print(&_s14);
			bbOnDebugLeaveScope();
		}
		struct BBDebugStm __stmt_8 = {0xfb255226cf53a02b, 67, 0};
		bbOnDebugEnterStm(&__stmt_8);
		brl_max2d_SetClsColor(0,0,255);
		struct BBDebugStm __stmt_9 = {0xfb255226cf53a02b, 69, 0};
		bbOnDebugEnterStm(&__stmt_9);
		while(!(brl_polledinput_KeyDown(27)!=0)){
			BBINT bbt_frame=0;
			struct BBDebugScope_1 __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					{
						BBDEBUGDECL_LOCAL,
						"frame",
						"i",
						.var_address=&bbt_frame
					},
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 71, 0};
			bbOnDebugEnterStm(&__stmt_0);
			if(brl_polledinput_KeyDown(32)!=0){
				struct BBDebugScope __scope = {
					BBDEBUGSCOPE_LOCALBLOCK,
					0,
					{
						BBDEBUGDECL_END 
					}
				};
				bbOnDebugEnterScope(&__scope);
				struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 72, 0};
				bbOnDebugEnterStm(&__stmt_0);
				if(((BBObject*)bbt_Snd)!= &bbNullObject){
					struct BBDebugScope __scope = {
						BBDEBUGSCOPE_LOCALBLOCK,
						0,
						{
							BBDEBUGDECL_END 
						}
					};
					bbOnDebugEnterScope(&__scope);
					struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 73, 0};
					bbOnDebugEnterStm(&__stmt_0);
					((struct brl_audio_TSound_obj*)bbNullObjectTest(bbt_Snd))->clas->m_Play_TTChannel((struct brl_audio_TSound_obj*)bbt_Snd,(struct brl_audio_TChannel_obj*)&bbNullObject);
					bbOnDebugLeaveScope();
				}
				struct BBDebugStm __stmt_1 = {0xfb255226cf53a02b, 75, 0};
				bbOnDebugEnterStm(&__stmt_1);
				bbEnd();
				bbOnDebugLeaveScope();
			}
			struct BBDebugStm __stmt_1 = {0xfb255226cf53a02b, 78, 0};
			bbOnDebugEnterStm(&__stmt_1);
			bbt_frame=0;
			struct BBDebugStm __stmt_2 = {0xfb255226cf53a02b, 80, 0};
			bbOnDebugEnterStm(&__stmt_2);
			if(brl_polledinput_MouseDown(1)!=0){
				struct BBDebugScope __scope = {
					BBDEBUGSCOPE_LOCALBLOCK,
					0,
					{
						BBDEBUGDECL_END 
					}
				};
				bbOnDebugEnterScope(&__scope);
				struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 81, 0};
				bbOnDebugEnterStm(&__stmt_0);
				brl_max2d_DrawRect(500.000000f,500.000000f,100.000000f,100.000000f);
				bbOnDebugLeaveScope();
			}
			struct BBDebugStm __stmt_3 = {0xfb255226cf53a02b, 85, 0};
			bbOnDebugEnterStm(&__stmt_3);
			if(brl_polledinput_KeyDown(32)!=0){
				struct BBDebugScope __scope = {
					BBDEBUGSCOPE_LOCALBLOCK,
					0,
					{
						BBDEBUGDECL_END 
					}
				};
				bbOnDebugEnterScope(&__scope);
				struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 86, 0};
				bbOnDebugEnterStm(&__stmt_0);
				brl_max2d_DrawImage((struct brl_max2d_image_TImage_obj*)bbt_Img,500.000000f,500.000000f,bbt_frame);
				bbOnDebugLeaveScope();
			}
			struct BBDebugStm __stmt_4 = {0xfb255226cf53a02b, 89, 0};
			bbOnDebugEnterStm(&__stmt_4);
			brl_max2d_DrawText(bbStringConcat(&_s15,bbStringFromInt(bbMilliSecs())),100.000000f,100.000000f);
			struct BBDebugStm __stmt_5 = {0xfb255226cf53a02b, 91, 0};
			bbOnDebugEnterStm(&__stmt_5);
			brl_max2d_DrawText(bbStringConcat(&_s16,bbStringFromInt(brl_graphics_GraphicsWidth())),100.000000f,150.000000f);
			struct BBDebugStm __stmt_6 = {0xfb255226cf53a02b, 92, 0};
			bbOnDebugEnterStm(&__stmt_6);
			brl_max2d_DrawText(bbStringConcat(&_s17,bbStringFromInt(brl_graphics_GraphicsHeight())),100.000000f,180.000000f);
			struct BBDebugStm __stmt_7 = {0xfb255226cf53a02b, 94, 0};
			bbOnDebugEnterStm(&__stmt_7);
			brl_graphics_Flip(-1);
			struct BBDebugStm __stmt_8 = {0xfb255226cf53a02b, 95, 0};
			bbOnDebugEnterStm(&__stmt_8);
			brl_max2d_Cls();
			bbOnDebugLeaveScope();
		}
		struct BBDebugStm __stmt_10 = {0xfb255226cf53a02b, 100, 0};
		bbOnDebugEnterStm(&__stmt_10);
		while(!(brl_polledinput_KeyDown(27)!=0)){
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0xfb255226cf53a02b, 102, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_graphics_Flip(-1);
			struct BBDebugStm __stmt_1 = {0xfb255226cf53a02b, 103, 0};
			bbOnDebugEnterStm(&__stmt_1);
			brl_max2d_Cls();
			bbOnDebugLeaveScope();
		}
		bbOnDebugLeaveScope();
		return 0;
	}
	return 0;
}