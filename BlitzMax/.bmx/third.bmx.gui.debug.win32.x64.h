#ifndef BLITZMAX_THIRD_BMX_GUI_DEBUG_WIN32_X64_H
#define BLITZMAX_THIRD_BMX_GUI_DEBUG_WIN32_X64_H

#include <brl.mod/blitz.mod/.bmx/blitz.bmx.debug.win32.x64.h>
#include <brl.mod/appstub.mod/.bmx/appstub.bmx.debug.win32.x64.h>
#include <brl.mod/audio.mod/.bmx/audio.bmx.debug.win32.x64.h>
#include <brl.mod/bank.mod/.bmx/bank.bmx.debug.win32.x64.h>
#include <brl.mod/bankstream.mod/.bmx/bankstream.bmx.debug.win32.x64.h>
#include <brl.mod/base64.mod/.bmx/base64.bmx.debug.win32.x64.h>
#include <brl.mod/basic.mod/.bmx/basic.bmx.debug.win32.x64.h>
#include <brl.mod/bmploader.mod/.bmx/bmploader.bmx.debug.win32.x64.h>
#include <brl.mod/clipboard.mod/.bmx/clipboard.bmx.debug.win32.x64.h>
#include <brl.mod/collections.mod/.bmx/collections.bmx.debug.win32.x64.h>
#include <brl.mod/d3d7max2d.mod/.bmx/d3d7max2d.bmx.debug.win32.x64.h>
#include <brl.mod/d3d9max2d.mod/.bmx/d3d9max2d.bmx.debug.win32.x64.h>
#include <brl.mod/directsoundaudio.mod/.bmx/directsoundaudio.bmx.debug.win32.x64.h>
#include <brl.mod/eventqueue.mod/.bmx/eventqueue.bmx.debug.win32.x64.h>
#include <brl.mod/freeaudioaudio.mod/.bmx/freeaudioaudio.bmx.debug.win32.x64.h>
#include <brl.mod/freetypefont.mod/.bmx/freetypefont.bmx.debug.win32.x64.h>
#include <brl.mod/glgraphics.mod/.bmx/glgraphics.bmx.debug.win32.x64.h>
#include <brl.mod/glmax2d.mod/.bmx/glmax2d.bmx.debug.win32.x64.h>
#include <brl.mod/gnet.mod/.bmx/gnet.bmx.debug.win32.x64.h>
#include <brl.mod/jpgloader.mod/.bmx/jpgloader.bmx.debug.win32.x64.h>
#include <brl.mod/map.mod/.bmx/map.bmx.debug.win32.x64.h>
#include <brl.mod/matrix.mod/.bmx/matrix.bmx.debug.win32.x64.h>
#include <brl.mod/maxlua.mod/.bmx/maxlua.bmx.debug.win32.x64.h>
#include <brl.mod/maxunit.mod/.bmx/maxunit.bmx.debug.win32.x64.h>
#include <brl.mod/maxutil.mod/.bmx/maxutil.bmx.debug.win32.x64.h>
#include <brl.mod/objectlist.mod/.bmx/objectlist.bmx.debug.win32.x64.h>
#include <brl.mod/oggloader.mod/.bmx/oggloader.bmx.debug.win32.x64.h>
#include <brl.mod/openalaudio.mod/.bmx/openalaudio.bmx.debug.win32.x64.h>
#include <brl.mod/platform.mod/.bmx/platform.bmx.debug.win32.x64.h>
#include <brl.mod/pngloader.mod/.bmx/pngloader.bmx.debug.win32.x64.h>
#include <brl.mod/quaternion.mod/.bmx/quaternion.bmx.debug.win32.x64.h>
#include <brl.mod/retro.mod/.bmx/retro.bmx.debug.win32.x64.h>
#include <brl.mod/tgaloader.mod/.bmx/tgaloader.bmx.debug.win32.x64.h>
#include <brl.mod/threadpool.mod/.bmx/threadpool.bmx.debug.win32.x64.h>
#include <brl.mod/timer.mod/.bmx/timer.bmx.debug.win32.x64.h>
#include <brl.mod/timerdefault.mod/.bmx/timerdefault.bmx.debug.win32.x64.h>
#include <brl.mod/uuid.mod/.bmx/uuid.bmx.debug.win32.x64.h>
#include <brl.mod/volumes.mod/.bmx/volumes.bmx.debug.win32.x64.h>
#include <brl.mod/wavloader.mod/.bmx/wavloader.bmx.debug.win32.x64.h>
#include <pub.mod/freejoy.mod/.bmx/freejoy.bmx.debug.win32.x64.h>
#include <pub.mod/freeprocess.mod/.bmx/freeprocess.bmx.debug.win32.x64.h>
#include <pub.mod/glad.mod/.bmx/glad.bmx.debug.win32.x64.h>
#include <pub.mod/nx.mod/.bmx/nx.bmx.debug.win32.x64.h>
#include <pub.mod/opengles.mod/.bmx/opengles.bmx.debug.win32.x64.h>
#include <pub.mod/opengles3.mod/.bmx/opengles3.bmx.debug.win32.x64.h>
#include <pub.mod/vulkan.mod/.bmx/vulkan.bmx.debug.win32.x64.h>
#include <pub.mod/xmmintrin.mod/.bmx/xmmintrin.bmx.debug.win32.x64.h>
int _bb_main();
struct _m_third_TT_obj;
struct _m_third_T_obj;
void __m_third_TT_New(struct _m_third_TT_obj* o);
struct BBClass__m_third_TT {
	BBClass*  super;
	void      (*free)( BBObject *o );
	BBDebugScope* debug_scope;
	unsigned int instance_size;
	void      (*ctor)( BBOBJECT o );
	void      (*dtor)( BBOBJECT o );
	BBSTRING  (*ToString)( BBOBJECT x );
	int       (*Compare)( BBOBJECT x,BBOBJECT y );
	BBOBJECT  (*SendMessage)( BBOBJECT o,BBOBJECT m,BBOBJECT s );
	BBINTERFACETABLE itable;
	void*     extra;
	unsigned int obj_size;
	unsigned int instance_count;
};

struct _m_third_TT_obj {
	struct BBClass__m_third_TT* clas;
};
extern struct BBClass__m_third_TT _m_third_TT;
void __m_third_T_New(struct _m_third_T_obj* o);
typedef BBINT (*_m_third_T__addeq_TT_m)(struct _m_third_T_obj*,struct _m_third_T_obj*);
BBINT __m_third_T__addeq_TT(struct _m_third_T_obj*,struct _m_third_T_obj*);
struct BBClass__m_third_T {
	BBClass*  super;
	void      (*free)( BBObject *o );
	BBDebugScope* debug_scope;
	unsigned int instance_size;
	void      (*ctor)( BBOBJECT o );
	void      (*dtor)( BBOBJECT o );
	BBSTRING  (*ToString)( BBOBJECT x );
	int       (*Compare)( BBOBJECT x,BBOBJECT y );
	BBOBJECT  (*SendMessage)( BBOBJECT o,BBOBJECT m,BBOBJECT s );
	BBINTERFACETABLE itable;
	void*     extra;
	unsigned int obj_size;
	unsigned int instance_count;
	_m_third_T__addeq_TT_m m__addeq_TT;
};

struct _m_third_T_obj {
	struct BBClass__m_third_T* clas;
};
extern struct BBClass__m_third_T _m_third_T;

#endif
