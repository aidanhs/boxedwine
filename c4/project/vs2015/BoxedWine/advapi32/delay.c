#include "svcctl.h"

extern HMODULE rpcrt4dll;

typedef CLIENT_CALL_RETURN (WINAPIV *pfnNdrClientCall2)(PMIDL_STUB_DESC desc, PFORMAT_STRING format, ...);
pfnNdrClientCall2 pNdrClientCall2;

CLIENT_CALL_RETURN WINAPIV NdrClientCall2(PMIDL_STUB_DESC desc, PFORMAT_STRING format, unsigned char* handle) {
    if (!rpcrt4dll) {
        rpcrt4dll = LoadLibraryA("rpcrt.dll");
    }
    if (!pNdrClientCall2)
        pNdrClientCall2 = (pfnNdrClientCall2)GetProcAddress(rpcrt4dll, "NdrClientCall2");
    return pNdrClientCall2(desc, format, handle);
}