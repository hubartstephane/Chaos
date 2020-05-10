#include "Gwen1.bmx.gui.debug.win32.x64.h"
static BBString _s0={
	&bbStringClass,
	54,
	{65,110,32,101,108,101,109,101,110,116,32,119,105,116,104,32,116
	,104,101,32,115,97,109,101,32,107,101,121,32,97,108,114,101
	,97,100,121,32,101,120,105,115,116,115,32,105,110,32,116,104
	,101,32,109,97,112}
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
	11,
	{118,111,105,116,117,114,101,46,112,110,103}
};
struct BBDebugScope_3{int kind; const char *name; BBDebugDecl decls[4]; };
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
		bbRegisterSource(0x7febf9c48ae25513, "F:/Personnel/Programmation/Code/BlitzMax/Gwen1.bmx");
		struct brl_max2d_image_TImage_obj* bbt_Image=(struct brl_max2d_image_TImage_obj*)(&bbNullObject);
		BBINT bbt_diametre=0;
		BBINT bbt_X=0;
		struct BBDebugScope_3 __scope = {
			BBDEBUGSCOPE_FUNCTION,
			"Gwen1",
			{
				{
					BBDEBUGDECL_LOCAL,
					"Image",
					":TImage",
					.var_address=&bbt_Image
				},
				{
					BBDEBUGDECL_LOCAL,
					"diametre",
					"i",
					.var_address=&bbt_diametre
				},
				{
					BBDEBUGDECL_LOCAL,
					"X",
					"i",
					.var_address=&bbt_X
				},
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0x7febf9c48ae25513, 1, 0};
		bbOnDebugEnterStm(&__stmt_0);
		brl_graphics_Graphics(800,600,0,60,0,-1,-1);
		struct BBDebugStm __stmt_1 = {0x7febf9c48ae25513, 3, 0};
		bbOnDebugEnterStm(&__stmt_1);
		brl_max2d_SetClsColor(255,159,78);
		struct BBDebugStm __stmt_2 = {0x7febf9c48ae25513, 5, 0};
		bbOnDebugEnterStm(&__stmt_2);
		bbt_Image=(struct brl_max2d_image_TImage_obj*)brl_max2d_LoadImage((BBOBJECT)&_s9,-1);
		struct BBDebugStm __stmt_3 = {0x7febf9c48ae25513, 8, 0};
		bbOnDebugEnterStm(&__stmt_3);
		bbt_diametre=30;
		struct BBDebugStm __stmt_4 = {0x7febf9c48ae25513, 10, 0};
		bbOnDebugEnterStm(&__stmt_4);
		bbt_X=0;
		struct BBDebugStm __stmt_5 = {0x7febf9c48ae25513, 11, 0};
		bbOnDebugEnterStm(&__stmt_5);
		while(!(brl_polledinput_AppTerminate()!=0)){
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0x7febf9c48ae25513, 13, 0};
			bbOnDebugEnterStm(&__stmt_0);
			brl_max2d_Cls();
			struct BBDebugStm __stmt_1 = {0x7febf9c48ae25513, 15, 0};
			bbOnDebugEnterStm(&__stmt_1);
			brl_max2d_DrawImage((struct brl_max2d_image_TImage_obj*)bbt_Image,((BBFLOAT)bbt_X),100.000000f,0);
			struct BBDebugStm __stmt_2 = {0x7febf9c48ae25513, 17, 0};
			bbOnDebugEnterStm(&__stmt_2);
			bbt_X=(bbt_X+1);
			struct BBDebugStm __stmt_3 = {0x7febf9c48ae25513, 18, 0};
			bbOnDebugEnterStm(&__stmt_3);
			if(bbt_X>800){
				struct BBDebugScope __scope = {
					BBDEBUGSCOPE_LOCALBLOCK,
					0,
					{
						BBDEBUGDECL_END 
					}
				};
				bbOnDebugEnterScope(&__scope);
				struct BBDebugStm __stmt_0 = {0x7febf9c48ae25513, 19, 0};
				bbOnDebugEnterStm(&__stmt_0);
				bbt_X=0;
				bbOnDebugLeaveScope();
			}
			struct BBDebugStm __stmt_4 = {0x7febf9c48ae25513, 22, 0};
			bbOnDebugEnterStm(&__stmt_4);
			brl_graphics_Flip(-1);
			bbOnDebugLeaveScope();
		}
		bbOnDebugLeaveScope();
		return 0;
	}
	return 0;
}