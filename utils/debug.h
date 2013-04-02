#ifndef PDW_DEBUG_H
#define PDW_DEBUG_H

extern int	nDebugOutput ;
extern int	nDebugNum ;


#if defined _DEBUG || defined DBG


extern int DebugPrintf(char *fmt,...) ;


#define OUTPUTDEBUGMSG(dprintf_exp)			((void) (DebugPrintf dprintf_exp))
#define ASSERTDEBUGMSG(cond, dprintf_exp)	((void)((cond) ? 0:(DebugPrintf dprintf_exp)))
#define DEBUGASSERT(dprintf_exp)			((void) (Debugv dprintf_exp));


#else

#define OUTPUTDEBUGMSG(dprintf_exp)
#define ASSERTDEBUGMSG(cond, dprintf_exp)
#define DEBUGASSERT(dprintf_exp)

#endif // DEBUG



#endif
