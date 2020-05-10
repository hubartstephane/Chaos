#include "fifth.bmx.gui.debug.win32.x64.h"
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
static BBString _s10={
	&bbStringClass,
	7,
	{72,101,108,108,111,32,50}
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
static BBString _s11={
	&bbStringClass,
	4,
	{116,111,116,111}
};
static BBString _s12={
	&bbStringClass,
	8,
	{116,114,117,99,58,112,110,103}
};
struct BBDebugScope_2{int kind; const char *name; BBDebugDecl decls[3]; };
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
		bbRegisterSource(0x43d402b220378183, "F:/Personnel/Programmation/Code/BlitzMax/fifth.bmx");
		struct BBDebugScope __scope = {
			BBDEBUGSCOPE_FUNCTION,
			"fifth",
			{
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0x43d402b220378183, 2, 0};
		bbOnDebugEnterStm(&__stmt_0);
		brl_graphics_Graphics(800,600,0,60,0,-1,-1);
		struct BBDebugStm __stmt_1 = {0x43d402b220378183, 4, 0};
		bbOnDebugEnterStm(&__stmt_1);
		brl_system_HideMouse();
		struct BBDebugStm __stmt_2 = {0x43d402b220378183, 7, 0};
		bbOnDebugEnterStm(&__stmt_2);
		brl_system_Confirm(&_s9,0);
		struct BBDebugStm __stmt_3 = {0x43d402b220378183, 8, 0};
		bbOnDebugEnterStm(&__stmt_3);
		brl_system_Proceed(&_s10,0);
		struct BBDebugStm __stmt_4 = {0x43d402b220378183, 10, 0};
		bbOnDebugEnterStm(&__stmt_4);
		brl_system_RequestFile(&_s11,&_s12,0,&bbEmptyString);
		struct BBDebugStm __stmt_5 = {0x43d402b220378183, 26, 0};
		bbOnDebugEnterStm(&__stmt_5);
		do{
			BBINT bbt_x=0;
			BBINT bbt_y=0;
			struct BBDebugScope_2 __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					{
						BBDEBUGDECL_LOCAL,
						"x",
						"i",
						.var_address=&bbt_x
					},
					{
						BBDEBUGDECL_LOCAL,
						"y",
						"i",
						.var_address=&bbt_y
					},
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0x43d402b220378183, 16, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_max2d_Cls();
			struct BBDebugStm __stmt_1 = {0x43d402b220378183, 18, 0};
			bbOnDebugEnterStm(&__stmt_1);
			bbt_x=brl_polledinput_MouseX();
			struct BBDebugStm __stmt_2 = {0x43d402b220378183, 19, 0};
			bbOnDebugEnterStm(&__stmt_2);
			bbt_y=brl_polledinput_MouseY();
			struct BBDebugStm __stmt_3 = {0x43d402b220378183, 21, 0};
			bbOnDebugEnterStm(&__stmt_3);
			brl_max2d_DrawRect(((BBFLOAT)bbt_x),((BBFLOAT)bbt_y),50.0000000f,50.0000000f);
			struct BBDebugStm __stmt_4 = {0x43d402b220378183, 24, 0};
			bbOnDebugEnterStm(&__stmt_4);
			brl_graphics_Flip(-1);
			bbOnDebugLeaveScope();
		}while(!(brl_polledinput_KeyDown(27)!=0));
		bbOnDebugLeaveScope();
		return 0;
	}
	return 0;
}