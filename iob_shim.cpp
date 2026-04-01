// Compatibility shim for OpenSSL 1.0.2 pre-built with VS2013 CRT
// linked against VS2015+ where ___iob_func no longer exists.
#include <stdio.h>

extern "C" {
FILE* __cdecl __iob_func(void)
{
    // VS2015+ does not export ___iob_func.
    // This shim is never actually called at runtime by modern code,
    // but satisfies the linker when using OpenSSL 1.0.2 static libs
    // built with an older MSVC toolset.
    static FILE placeholder[3];
    placeholder[0] = *stdin;
    placeholder[1] = *stdout;
    placeholder[2] = *stderr;
    return placeholder;
}
}