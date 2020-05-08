#include "first.bmx.gui.debug.win32.x64.h"
static BBString _s10={
	&bbStringClass,
	1,
	{32}
};
static BBString _s12={
	&bbStringClass,
	9,
	{32,87,105,100,116,104,32,61,32}
};
static BBString _s11={
	&bbStringClass,
	15,
	{45,32,80,108,101,97,115,101,32,87,97,105,116,32,45}
};
static BBString _s0={
	&bbStringClass,
	54,
	{65,110,32,101,108,101,109,101,110,116,32,119,105,116,104,32,116
	,104,101,32,115,97,109,101,32,107,101,121,32,97,108,114,101
	,97,100,121,32,101,120,105,115,116,115,32,105,110,32,116,104
	,101,32,109,97,112}
};
static BBString _s13={
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
static BBString _s9={
	&bbStringClass,
	3,
	{116,111,116}
};
struct BBDebugScope_1{int kind; const char *name; BBDebugDecl decls[2]; };
struct BBDebugScope_6{int kind; const char *name; BBDebugDecl decls[7]; };
BBSTRING _m_first_GetName(BBSTRING bbt_toto){
	struct BBDebugScope_1 __scope = {
		BBDEBUGSCOPE_FUNCTION,
		"GetName",
		{
			{
				BBDEBUGDECL_LOCAL,
				"toto",
				"$",
				.var_address=&bbt_toto
			},
			BBDEBUGDECL_END 
		}
	};
	bbOnDebugEnterScope(&__scope);
	struct BBDebugStm __stmt_0 = {0x1972601e56d492d1, 66, 0};
	bbOnDebugEnterStm(&__stmt_0);
	bbOnDebugLeaveScope();
	return bbStringConcat(bbStringConcat(bbt_toto,&_s10),bbt_toto);
}
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
		bbRegisterSource(0x1972601e56d492d1, "F:/Personnel/Programmation/BlitzMax/first.bmx");
		BBFLOAT bbt_speed=0;
		BBSTRING bbt_name=(&bbEmptyString);
		BBINT bbt_frame=0;
		BBINT bbt_kk=0;
		BBARRAY bbt_Arr=(&bbEmptyArray);
		BBINT bbt_W=0;
		struct BBDebugScope_6 __scope = {
			BBDEBUGSCOPE_FUNCTION,
			"first",
			{
				{
					BBDEBUGDECL_LOCAL,
					"speed",
					"f",
					.var_address=&bbt_speed
				},
				{
					BBDEBUGDECL_LOCAL,
					"name",
					"$",
					.var_address=&bbt_name
				},
				{
					BBDEBUGDECL_LOCAL,
					"frame",
					"i",
					.var_address=&bbt_frame
				},
				{
					BBDEBUGDECL_LOCAL,
					"kk",
					"i",
					.var_address=&bbt_kk
				},
				{
					BBDEBUGDECL_LOCAL,
					"Arr",
					"[]i",
					.var_address=&bbt_Arr
				},
				{
					BBDEBUGDECL_LOCAL,
					"W",
					"i",
					.var_address=&bbt_W
				},
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0x1972601e56d492d1, 6, 0};
		bbOnDebugEnterStm(&__stmt_0);
		brl_graphics_Graphics(800,600,0,60,0,-1,-1);
		struct BBDebugStm __stmt_1 = {0x1972601e56d492d1, 9, 0};
		bbOnDebugEnterStm(&__stmt_1);
		bbt_speed=0.0f;
		struct BBDebugStm __stmt_2 = {0x1972601e56d492d1, 11, 0};
		bbOnDebugEnterStm(&__stmt_2);
		bbt_name=&_s9;
		struct BBDebugStm __stmt_3 = {0x1972601e56d492d1, 13, 0};
		bbOnDebugEnterStm(&__stmt_3);
		bbt_frame=0;
		struct BBDebugStm __stmt_4 = {0x1972601e56d492d1, 15, 0};
		bbOnDebugEnterStm(&__stmt_4);
		bbt_kk=666;
		struct BBDebugStm __stmt_5 = {0x1972601e56d492d1, 54, 0};
		bbOnDebugEnterStm(&__stmt_5);
		BBINT bbt_[]={1,2,5};
		BBARRAY bbt_2 = bbArrayFromData("i",3,bbt_);
		bbt_Arr=bbt_2;
		struct BBDebugStm __stmt_6 = {0x1972601e56d492d1, 56, 0};
		bbOnDebugEnterStm(&__stmt_6);
		BBARRAY bbt_3=bbt_Arr;
		BBUINT bbt_4=0U;
		while(bbt_4<((BBUINT)(bbt_3->scales[0]))){
			BBINT bbt_n=0;
			struct BBDebugScope_1 __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					{
						BBDEBUGDECL_LOCAL,
						"n",
						"i",
						.var_address=&bbt_n
					},
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			bbt_n=((BBINT*)BBARRAYDATAINDEX((bbt_3),(bbt_3)->dims,bbt_4))[bbt_4];
			bbt_4=(bbt_4+1U);
			struct BBDebugStm __stmt_0 = {0x1972601e56d492d1, 57, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_standardio_Print(bbStringFromInt(bbt_n));
			bbOnDebugLeaveScope();
		}
		struct BBDebugStm __stmt_7 = {0x1972601e56d492d1, 62, 0};
		bbOnDebugEnterStm(&__stmt_7);
		bbt_W=0;
		struct BBDebugStm __stmt_8 = {0x1972601e56d492d1, 83, 0};
		bbOnDebugEnterStm(&__stmt_8);
		do{
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0x1972601e56d492d1, 71, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_max2d_DrawRect(0.000000000f,40.0000000f,((BBFLOAT)bbt_W),12.0000000f);
			struct BBDebugStm __stmt_1 = {0x1972601e56d492d1, 72, 0};
			bbOnDebugEnterStm(&__stmt_1);
			brl_max2d_DrawText(&_s11,0.000000000f,0.000000000f);
			struct BBDebugStm __stmt_2 = {0x1972601e56d492d1, 73, 0};
			bbOnDebugEnterStm(&__stmt_2);
			brl_max2d_DrawText(_m_first_GetName(bbStringConcat(&_s12,bbStringFromInt(bbt_W))),0.000000000f,20.0000000f);
			struct BBDebugStm __stmt_3 = {0x1972601e56d492d1, 74, 0};
			bbOnDebugEnterStm(&__stmt_3);
			bbt_W+=3;
			struct BBDebugStm __stmt_4 = {0x1972601e56d492d1, 76, 0};
			bbOnDebugEnterStm(&__stmt_4);
			{
				BBINT bbt_k=1;
				for(;(bbt_k<=100);bbt_k=(bbt_k+1)){
					struct BBDebugScope_1 __scope = {
						BBDEBUGSCOPE_LOCALBLOCK,
						0,
						{
							{
								BBDEBUGDECL_LOCAL,
								"k",
								"i",
								.var_address=&bbt_k
							},
							BBDEBUGDECL_END 
						}
					};
					bbOnDebugEnterScope(&__scope);
					struct BBDebugStm __stmt_0 = {0x1972601e56d492d1, 78, 0};
					bbOnDebugEnterStm(&__stmt_0);
					brl_max2d_Plot(((BBFLOAT)brl_random_Rand(800,1)),((BBFLOAT)brl_random_Rand(800,1)));
					bbOnDebugLeaveScope();
				}
			}
			struct BBDebugStm __stmt_5 = {0x1972601e56d492d1, 82, 0};
			bbOnDebugEnterStm(&__stmt_5);
			brl_graphics_Flip(-1);
			struct BBDebugStm __stmt_6 = {0x1972601e56d492d1, 82, 0};
			bbOnDebugEnterStm(&__stmt_6);
			brl_max2d_Cls();
			bbOnDebugLeaveScope();
		}while(!(bbt_W>=800));
		struct BBDebugStm __stmt_9 = {0x1972601e56d492d1, 87, 0};
		bbOnDebugEnterStm(&__stmt_9);
		brl_max2d_SetOrigin(320.000000f,240.000000f);
		struct BBDebugStm __stmt_10 = {0x1972601e56d492d1, 88, 0};
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
			struct BBDebugStm __stmt_0 = {0x1972601e56d492d1, 90, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_max2d_Cls();
			struct BBDebugStm __stmt_1 = {0x1972601e56d492d1, 92, 0};
			bbOnDebugEnterStm(&__stmt_1);
			brl_max2d_SetRotation((BBFLOAT)bbt_frame);
			struct BBDebugStm __stmt_2 = {0x1972601e56d492d1, 93, 0};
			bbOnDebugEnterStm(&__stmt_2);
			brl_max2d_DrawText(bbStringConcat(&_s13,bbStringFromFloat(bbt_speed)),0.000000000f,0.000000000f);
			struct BBDebugStm __stmt_3 = {0x1972601e56d492d1, 95, 0};
			bbOnDebugEnterStm(&__stmt_3);
			brl_graphics_Flip(-1);
			struct BBDebugStm __stmt_4 = {0x1972601e56d492d1, 97, 0};
			bbOnDebugEnterStm(&__stmt_4);
			bbt_frame+=1;
			bbOnDebugLeaveScope();
		}
		bbOnDebugLeaveScope();
		return 0;
	}
	return 0;
}