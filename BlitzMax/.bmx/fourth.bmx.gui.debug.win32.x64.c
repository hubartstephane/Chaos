#include "fourth.bmx.gui.debug.win32.x64.h"
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
static BBString _s9={
	&bbStringClass,
	23,
	{86,105,114,116,117,97,108,32,82,101,115,111,108,117,116,105,111
	,110,32,84,101,115,116}
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
		__bb_maxgui_drivers_drivers();
		bbRegisterSource(0xc0c226a68bc14a22, "F:/Personnel/Programmation/Code/BlitzMax/fourth.bmx");
		struct maxgui_maxgui_gadget_TGadget_obj* bbt_win=(struct maxgui_maxgui_gadget_TGadget_obj*)(&bbNullObject);
		struct maxgui_maxgui_gadget_TGadget_obj* bbt_canvas=(struct maxgui_maxgui_gadget_TGadget_obj*)(&bbNullObject);
		struct BBDebugScope_2 __scope = {
			BBDEBUGSCOPE_FUNCTION,
			"fourth",
			{
				{
					BBDEBUGDECL_LOCAL,
					"win",
					":TGadget",
					.var_address=&bbt_win
				},
				{
					BBDEBUGDECL_LOCAL,
					"canvas",
					":TGadget",
					.var_address=&bbt_canvas
				},
				BBDEBUGDECL_END 
			}
		};
		bbOnDebugEnterScope(&__scope);
		struct BBDebugStm __stmt_0 = {0xc0c226a68bc14a22, 4, 0};
		bbOnDebugEnterStm(&__stmt_0);
		bbt_win=(struct maxgui_maxgui_gadget_TGadget_obj*)maxgui_maxgui_CreateWindow(&_s9,0,0,640,480,(struct maxgui_maxgui_gadget_TGadget_obj*)&bbNullObject,547);
		struct BBDebugStm __stmt_1 = {0xc0c226a68bc14a22, 6, 0};
		bbOnDebugEnterStm(&__stmt_1);
		bbt_canvas=(struct maxgui_maxgui_gadget_TGadget_obj*)maxgui_maxgui_CreateCanvas(0,0,640,480,(struct maxgui_maxgui_gadget_TGadget_obj*)bbt_win,0);
		struct BBDebugStm __stmt_2 = {0xc0c226a68bc14a22, 7, 0};
		bbOnDebugEnterStm(&__stmt_2);
		maxgui_maxgui_SetGadgetLayout((struct maxgui_maxgui_gadget_TGadget_obj*)bbt_canvas,1,1,1,1);
		struct BBDebugStm __stmt_3 = {0xc0c226a68bc14a22, 28, 0};
		bbOnDebugEnterStm(&__stmt_3);
		do{
			struct BBDebugScope __scope = {
				BBDEBUGSCOPE_LOCALBLOCK,
				0,
				{
					BBDEBUGDECL_END 
				}
			};
			bbOnDebugEnterScope(&__scope);
			struct BBDebugStm __stmt_0 = {0xc0c226a68bc14a22, 10, 0};
			bbOnDebugEnterStm(&__stmt_0);
			BBINT bbt_=brl_eventqueue_WaitEvent();
			struct BBDebugStm __stmt_1 = {0xc0c226a68bc14a22, 11, 0};
			bbOnDebugEnterStm(&__stmt_1);
			if(bbt_==259){
				struct BBDebugScope __scope = {
					BBDEBUGSCOPE_LOCALBLOCK,
					0,
					{
						BBDEBUGDECL_END 
					}
				};
				bbOnDebugEnterScope(&__scope);
				struct BBDebugStm __stmt_0 = {0xc0c226a68bc14a22, 12, 0};
				bbOnDebugEnterStm(&__stmt_0);
				bbEnd();
				bbOnDebugLeaveScope();
			}else{
				struct BBDebugScope __scope = {
					BBDEBUGSCOPE_LOCALBLOCK,
					0,
					{
						BBDEBUGDECL_END 
					}
				};
				bbOnDebugEnterScope(&__scope);
				struct BBDebugStm __stmt_0 = {0xc0c226a68bc14a22, 13, 0};
				bbOnDebugEnterStm(&__stmt_0);
				if(bbt_==16387){
					struct BBDebugScope __scope = {
						BBDEBUGSCOPE_LOCALBLOCK,
						0,
						{
							BBDEBUGDECL_END 
						}
					};
					bbOnDebugEnterScope(&__scope);
					struct BBDebugStm __stmt_0 = {0xc0c226a68bc14a22, 14, 0};
					bbOnDebugEnterStm(&__stmt_0);
					brl_eventqueue_PostEvent((struct brl_event_TEvent_obj*)brl_event_CreateEvent(259,(BBOBJECT)&bbNullObject,0,0,0,0,(BBOBJECT)&bbNullObject),0);
					bbOnDebugLeaveScope();
				}else{
					struct BBDebugScope __scope = {
						BBDEBUGSCOPE_LOCALBLOCK,
						0,
						{
							BBDEBUGDECL_END 
						}
					};
					bbOnDebugEnterScope(&__scope);
					struct BBDebugStm __stmt_0 = {0xc0c226a68bc14a22, 18, 0};
					bbOnDebugEnterStm(&__stmt_0);
					if(bbt_==8194){
						struct BBDebugScope __scope = {
							BBDEBUGSCOPE_LOCALBLOCK,
							0,
							{
								BBDEBUGDECL_END 
							}
						};
						bbOnDebugEnterScope(&__scope);
						struct BBDebugStm __stmt_0 = {0xc0c226a68bc14a22, 19, 0};
						bbOnDebugEnterStm(&__stmt_0);
						brl_graphics_SetGraphics((struct brl_graphics_TGraphics_obj*)maxgui_maxgui_CanvasGraphics((struct maxgui_maxgui_gadget_TGadget_obj*)bbt_canvas));
						struct BBDebugStm __stmt_1 = {0xc0c226a68bc14a22, 20, 0};
						bbOnDebugEnterStm(&__stmt_1);
						brl_max2d_Cls();
						struct BBDebugStm __stmt_2 = {0xc0c226a68bc14a22, 21, 0};
						bbOnDebugEnterStm(&__stmt_2);
						brl_max2d_SetColor(brl_random_Rand(0,255),brl_random_Rand(0,255),brl_random_Rand(0,255));
						struct BBDebugStm __stmt_3 = {0xc0c226a68bc14a22, 22, 0};
						bbOnDebugEnterStm(&__stmt_3);
						brl_max2d_DrawRect(16.0000000f,16.0000000f,608.000000f,448.000000f);
						struct BBDebugStm __stmt_4 = {0xc0c226a68bc14a22, 23, 0};
						bbOnDebugEnterStm(&__stmt_4);
						brl_graphics_Flip(-1);
						bbOnDebugLeaveScope();
					}
					bbOnDebugLeaveScope();
				}
				bbOnDebugLeaveScope();
			}
			bbOnDebugLeaveScope();
		}while(!(0));
	}
	return 0;
}