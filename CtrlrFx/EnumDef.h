/// @file EnumDef.h
/// Macros for creating standardized enumerations.


#ifndef __CtrlrFx_EnumDef_h
#define __CtrlrFx_EnumDef_h

#define	ENUM_BEGIN(enum_type) \
typedef enum \
{ \
	enum_type ## _MIN, \
	enum_type ## _INVALID	= -1, 

#define	ENUM_BEGIN_MIN(enum_type, minval) \
typedef enum \
{ \
	enum_type ## _MIN = (minval), \
	enum_type ## _INVALID	= (minval)-1, 

#if defined(__cplusplus)
	#define	ENUM_END(enum_type) \
		, N_ ## enum_type, \
		enum_type ## _MAX = N_ ## enum_type - 1, \
	} enum_type , *P ## enum_type ;
#else
	#define	ENUM_END(enum_type) \
		enum_type ## _INSERT_BEFORE, \
		enum_type ## _MAX = enum_type ## _INSERT_BEFORE - 1, \
		N_ ## enum_type = enum_type ## _INSERT_BEFORE \
	} enum_type, *P ## enum_type;
#endif

#define	ENUM_VALID(enum_type, val) \
	( ((int) enum_type ## _MIN) <= ((int) val) && ((int) val) <= ((int) enum_type ## _MAX ) )

#define	ENUM_COUNT(enum_type) \
	( enum_type ## _MAX - enum_type ## _MIN + 1 )

#define	ENUM_MIN(enum_type)		( enum_type ## _MIN )
#define	ENUM_MAX(enum_type)		( enum_type ## _MAX )
#define	ENUM_INVALID(enumtype)	( enum_type ## _INVALID )

#endif		// __CtrlrFx_EnumDef_h
