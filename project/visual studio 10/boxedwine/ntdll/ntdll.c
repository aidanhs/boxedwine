#include "config.h"
#include <stdlib.h>
#include "wine/port.h"
#include "windef.h"
#include "winternl.h"
#include "winnt.h"
#include "winbase.h"
#include "wine/exception.h"

int _fltused=1;

char* __wine_spec_file_name="NTDLL.DLL";
IMAGE_NT_HEADERS __wine_spec_nt_header;

void setImageNtHeaders(void* p) {
	memcpy(&__wine_spec_nt_header, p, sizeof(IMAGE_NT_HEADERS));
}

void _fini() {
}

/*************************************************************************
 *		call_dll_entry_point
 *
 * Some brain-damaged dlls (ir32_32.dll for instance) modify ebx in
 * their entry point, so we need a small asm wrapper.
 */
typedef DWORD (CALLBACK *DLLENTRYPROC)(HMODULE,DWORD,LPVOID);
/*
#ifdef __i386__
extern BOOL call_dll_entry_point( DLLENTRYPROC proc, void *module, UINT reason, void *reserved );
__ASM_GLOBAL_FUNC(call_dll_entry_point,
                  "pushl %ebp\n\t"
                  __ASM_CFI(".cfi_adjust_cfa_offset 4\n\t")
                  __ASM_CFI(".cfi_rel_offset %ebp,0\n\t")
                  "movl %esp,%ebp\n\t"
                  __ASM_CFI(".cfi_def_cfa_register %ebp\n\t")
                  "pushl %ebx\n\t"
                  __ASM_CFI(".cfi_rel_offset %ebx,-4\n\t")
                  "subl $8,%esp\n\t"
                  "pushl 20(%ebp)\n\t"
                  "pushl 16(%ebp)\n\t"
                  "pushl 12(%ebp)\n\t"
                  "movl 8(%ebp),%eax\n\t"
                  "call *%eax\n\t"
                  "leal -4(%ebp),%esp\n\t"
                  "popl %ebx\n\t"
                  __ASM_CFI(".cfi_same_value %ebx\n\t")
                  "popl %ebp\n\t"
                  __ASM_CFI(".cfi_def_cfa %esp,4\n\t")
                  __ASM_CFI(".cfi_same_value %ebp\n\t")
                  "ret" )
				  */
BOOL call_dll_entry_point( DLLENTRYPROC proc, HMODULE module,
                                         UINT reason, void *reserved )
{
	int i;
	BOOL result;
	__asm mov i, ebp;
    result = proc( module, reason, reserved );
	__asm mov ebp, i;
	return result;
}

/**********************************************************************
 *		DbgBreakPoint   (NTDLL.@)
 */
// __ASM_STDCALL_FUNC( DbgBreakPoint, 0, "int $3; ret")
void WINAPI DbgBreakPoint(void)
{
	__asm int 3;
}

/**********************************************************************
 *		DbgUserBreakPoint   (NTDLL.@)
 */
//__ASM_STDCALL_FUNC( DbgUserBreakPoint, 0, "int $3; ret")
void WINAPI DbgUserBreakPoint(void)
{
	__asm int 3;
}

/*
__ASM_GLOBAL_FUNC( clear_alignment_flag,
                   "pushfl\n\t"
                   __ASM_CFI(".cfi_adjust_cfa_offset 4\n\t")
                   "andl $~0x40000,(%esp)\n\t"
                   "popfl\n\t"
                   __ASM_CFI(".cfi_adjust_cfa_offset -4\n\t")
                   "ret" )
				   */
 void clear_alignment_flag(void)
 {
	 __asm
	 {
		 pushf
		 and esp, ~0x40000
		 popf
	 }
 }

/*
extern void DECLSPEC_NORETURN call_thread_entry_point( LPTHREAD_START_ROUTINE entry, void *arg );
__ASM_GLOBAL_FUNC( call_thread_entry_point,
                   "pushl %ebp\n\t"
                   __ASM_CFI(".cfi_adjust_cfa_offset 4\n\t")
                   __ASM_CFI(".cfi_rel_offset %ebp,0\n\t")
                   "movl %esp,%ebp\n\t"
                   __ASM_CFI(".cfi_def_cfa_register %ebp\n\t")
                   "pushl %ebx\n\t"
                   __ASM_CFI(".cfi_rel_offset %ebx,-4\n\t")
                   "pushl %esi\n\t"
                   __ASM_CFI(".cfi_rel_offset %esi,-8\n\t")
                   "pushl %edi\n\t"
                   __ASM_CFI(".cfi_rel_offset %edi,-12\n\t")
                   "pushl %ebp\n\t"
                   "pushl 12(%ebp)\n\t"
                   "pushl 8(%ebp)\n\t"
                   "call " __ASM_NAME("call_thread_func") );
				   */
extern void DECLSPEC_NORETURN exit_thread( int status ) DECLSPEC_HIDDEN;
typedef LONG (WINAPI *PUNHANDLED_EXCEPTION_FILTER)(PEXCEPTION_POINTERS);
extern PUNHANDLED_EXCEPTION_FILTER unhandled_exception_filter DECLSPEC_HIDDEN;
void call_thread_entry_point( LPTHREAD_START_ROUTINE entry, void *arg )
{
    __TRY
    {
        exit_thread( entry( arg ));
    }
    __EXCEPT(unhandled_exception_filter)
    {
        NtTerminateThread( GetCurrentThread(), GetExceptionCode() );
    }
    __ENDTRY
    abort();  /* should not be reached */
}

/*
__ASM_GLOBAL_FUNC( call_entry_point,
                   "pushl %ebp\n\t"
                   __ASM_CFI(".cfi_adjust_cfa_offset 4\n\t")
                   __ASM_CFI(".cfi_rel_offset %ebp,0\n\t")
                   "movl %esp,%ebp\n\t"
                   __ASM_CFI(".cfi_def_cfa_register %ebp\n\t")
                   "pushl %esi\n\t"
                  __ASM_CFI(".cfi_rel_offset %esi,-4\n\t")
                   "pushl %edi\n\t"
                  __ASM_CFI(".cfi_rel_offset %edi,-8\n\t")
                   "movl 12(%ebp),%edx\n\t"
                   "shll $2,%edx\n\t"
                   "jz 1f\n\t"
                   "subl %edx,%esp\n\t"
                   "andl $~15,%esp\n\t"
                   "movl 12(%ebp),%ecx\n\t"
                   "movl 16(%ebp),%esi\n\t"
                   "movl %esp,%edi\n\t"
                   "cld\n\t"
                   "rep; movsl\n"
                   "testl $2,20(%ebp)\n\t"  // (flags & 2) -> thiscall
                   "jz 1f\n\t"
                   "popl %ecx\n\t"
                   "1:\tcall *8(%ebp)\n\t"
                   "leal -8(%ebp),%esp\n\t"
                   "popl %edi\n\t"
                   __ASM_CFI(".cfi_same_value %edi\n\t")
                   "popl %esi\n\t"
                   __ASM_CFI(".cfi_same_value %esi\n\t")
                   "popl %ebp\n\t"
                   __ASM_CFI(".cfi_def_cfa %esp,4\n\t")
                   __ASM_CFI(".cfi_same_value %ebp\n\t")
                   "ret" )
				   */
// :TODO: need to convert this linux asm to ms asm
LONGLONG CDECL call_entry_point( void *func, int nb_args, const INT_PTR *args, int flags )
{
	return 0;
}

/*
extern void DECLSPEC_NORETURN call_thread_exit_func( int status, void (*func)(int), void *frame );
__ASM_GLOBAL_FUNC( call_thread_exit_func,
                   "movl 4(%esp),%eax\n\t"
                   "movl 8(%esp),%ecx\n\t"
                   "movl 12(%esp),%ebp\n\t"
                   __ASM_CFI(".cfi_def_cfa %ebp,4\n\t")
                   __ASM_CFI(".cfi_rel_offset %ebp,0\n\t")
                   __ASM_CFI(".cfi_rel_offset %ebx,-4\n\t")
                   __ASM_CFI(".cfi_rel_offset %esi,-8\n\t")
                   __ASM_CFI(".cfi_rel_offset %edi,-12\n\t")
                   "leal -20(%ebp),%esp\n\t"
                   "pushl %eax\n\t"
                   "call *%ecx" );
*/

// :TODO:
 void call_thread_exit_func( int status, void (*func)(int), void *frame )
 {
	 func(status);
 }

 /* wrapper for apps that don't declare the thread function correctly */
 /*
extern void DECLSPEC_NORETURN call_thread_func_wrapper( LPTHREAD_START_ROUTINE entry, void *arg );
__ASM_GLOBAL_FUNC(call_thread_func_wrapper,
                  "pushl %ebp\n\t"
                  __ASM_CFI(".cfi_adjust_cfa_offset 4\n\t")
                  __ASM_CFI(".cfi_rel_offset %ebp,0\n\t")
                  "movl %esp,%ebp\n\t"
                  __ASM_CFI(".cfi_def_cfa_register %ebp\n\t")
                  "subl $4,%esp\n\t"
                  "pushl 12(%ebp)\n\t"
                  "call *8(%ebp)\n\t"
                  "leal -4(%ebp),%esp\n\t"
                  "pushl %eax\n\t"
                  "call " __ASM_NAME("exit_thread") "\n\t"
                  "int $3" )
				  */
 // :TODO:
 void call_thread_func_wrapper( LPTHREAD_START_ROUTINE entry, void *arg )
 {

 }
 /*
 __ASM_GLOBAL_FUNC( EXC_CallHandler,
                  "pushl %ebp\n\t"
                  __ASM_CFI(".cfi_adjust_cfa_offset 4\n\t")
                  __ASM_CFI(".cfi_rel_offset %ebp,0\n\t")
                  "movl %esp,%ebp\n\t"
                  __ASM_CFI(".cfi_def_cfa_register %ebp\n\t")
                   "pushl %ebx\n\t"
                   __ASM_CFI(".cfi_rel_offset %ebx,-4\n\t")
                   "movl 28(%ebp), %edx\n\t" // ugly hack to pass the 6th param needed because of Shrinker 
                   "pushl 24(%ebp)\n\t"
                   "pushl 20(%ebp)\n\t"
                   "pushl 16(%ebp)\n\t"
                   "pushl 12(%ebp)\n\t"
                   "pushl 8(%ebp)\n\t"
                   "call " __ASM_NAME("call_exception_handler") "\n\t"
                   "popl %ebx\n\t"
                   __ASM_CFI(".cfi_same_value %ebx\n\t")
                   "leave\n"
                   __ASM_CFI(".cfi_def_cfa %esp,4\n\t")
                   __ASM_CFI(".cfi_same_value %ebp\n\t")
                   "ret" )
				   */
 // :TODO:
 DWORD EXC_CallHandler( EXCEPTION_RECORD *record, EXCEPTION_REGISTRATION_RECORD *frame,
                              CONTEXT *context, EXCEPTION_REGISTRATION_RECORD **dispatcher,
                              PEXCEPTION_HANDLER handler, PEXCEPTION_HANDLER nested_handler )
 {
	 return 0;
 }

void WINAPI relay_call_regs(void)
{

}

void __wine_restore_regs( const CONTEXT *context )
{
}

void WINAPI SNOOP_Entry(void)
{
}

void WINAPI SNOOP_Return(void)
{
}

/*
__ASM_GLOBAL_FUNC(NTDLL_RtlUlongByteSwap,
                  "movl %ecx,%eax\n\t"
                  "bswap %eax\n\t"
                  "ret")
				  */

void NTDLL_RtlUlongByteSwap()
{
	__asm {
		mov eax, ecx;
		bswap eax;
	}
}

/*
__ASM_GLOBAL_FUNC(NTDLL_RtlUshortByteSwap,
                  "movb %ch,%al\n\t"
                  "movb %cl,%ah\n\t"
                  "ret")
				  */
void NTDLL_RtlUshortByteSwap()
{
	__asm {
		mov al, ch;
		mov ah, cl;
	}
}

void WINAPI __regs_RtlRaiseException( EXCEPTION_RECORD *rec, CONTEXT *context );
NTSYSAPI void WINAPI RtlRaiseException(PEXCEPTION_RECORD rec)
{
    CONTEXT context;
    
    RtlCaptureContext( &context );
	__regs_RtlRaiseException(rec, &context);    
}

/*
__ASM_STDCALL_FUNC( RtlCaptureContext, 4,
                    "pushl %eax\n\t"
                    __ASM_CFI(".cfi_adjust_cfa_offset 4\n\t")
                    "movl 8(%esp),%eax\n\t"    // context
                    "movl $0x10007,(%eax)\n\t" // context->ContextFlags
                    "movw %gs,0x8c(%eax)\n\t"  // context->SegGs
                    "movw %fs,0x90(%eax)\n\t"  // context->SegFs
                    "movw %es,0x94(%eax)\n\t"  // context->SegEs
                    "movw %ds,0x98(%eax)\n\t"  // context->SegDs
                    "movl %edi,0x9c(%eax)\n\t" // context->Edi 
                    "movl %esi,0xa0(%eax)\n\t" // context->Esi 
                    "movl %ebx,0xa4(%eax)\n\t" // context->Ebx 
                    "movl %edx,0xa8(%eax)\n\t" // context->Edx 
                    "movl %ecx,0xac(%eax)\n\t" // context->Ecx 
                    "movl %ebp,0xb4(%eax)\n\t" // context->Ebp 
                    "movl 4(%esp),%edx\n\t"
                    "movl %edx,0xb8(%eax)\n\t" // context->Eip 
                    "movw %cs,0xbc(%eax)\n\t"  // context->SegCs 
                    "pushfl\n\t"
                    __ASM_CFI(".cfi_adjust_cfa_offset 4\n\t")
                    "popl 0xc0(%eax)\n\t"      // context->EFlags 
                    __ASM_CFI(".cfi_adjust_cfa_offset -4\n\t")
                    "leal 8(%esp),%edx\n\t"
                    "movl %edx,0xc4(%eax)\n\t" // context->Esp 
                    "movw %ss,0xc8(%eax)\n\t"  // context->SegSs 
                    "popl 0xb0(%eax)\n\t"      // context->Eax 
                    __ASM_CFI(".cfi_adjust_cfa_offset -4\n\t")
                    "ret $4" )
					*/

// copied from exception_utils.cc at http://omaha.googlecode.com
// Copyright 2006-2009 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ========================================================================
__declspec(naked) void WINAPI RtlCaptureContext(CONTEXT* runner) {
  runner;   // unreferenced formal parameter
  __asm {
    // set up a call frame
    push ebp
    mov ebp, esp

    // save ecx for later
    push ecx

    // fetch the context record pointer argument into ecx
    // which we use as pointer to context throughout the rest
    // of this function
    mov ecx, DWORD PTR [ebp + 8]

    // set flags
    mov [ecx]CONTEXT.ContextFlags, CONTEXT_SEGMENTS | CONTEXT_INTEGER | \
                                   CONTEXT_CONTROL | CONTEXT_FLOATING_POINT

    // stash the integer registers away
    mov [ecx]CONTEXT.Edi, edi
    mov [ecx]CONTEXT.Ebx, ebx
    mov [ecx]CONTEXT.Edx, edx
    mov [ecx]CONTEXT.Eax, eax
    mov [ecx]CONTEXT.Esi, esi
    // get the saved ecx
    pop eax
    mov [ecx]CONTEXT.Ecx, eax

    // now control registers
    pushfd
    pop eax
    mov [ecx]CONTEXT.EFlags, eax

    // get the old ebp, our FP points to it
    mov eax, [ebp]
    mov [ecx]CONTEXT.Ebp, eax

    // get return address and record as eip
    mov eax, [ebp + 4]
    mov [ecx]CONTEXT.Eip, eax

    // esp post-return is ...
    lea eax, [ebp + 0xC]
    mov [ecx]CONTEXT.Esp, eax

    // snarf segment registers
    mov word ptr [ecx]CONTEXT.SegSs, ss
    mov word ptr [ecx]CONTEXT.SegCs, cs
    mov word ptr [ecx]CONTEXT.SegGs, gs
    mov word ptr [ecx]CONTEXT.SegFs, fs
    mov word ptr [ecx]CONTEXT.SegEs, es
    mov word ptr [ecx]CONTEXT.SegDs, ds

    // and lastly grab floating point state
    fnsave [ecx]CONTEXT.FloatSave

    // return the CONTEXT pointer
    mov eax, ecx

    // and return
    pop ebp
    ret 4
  }
}

void WINAPI __regs_RtlUnwind( EXCEPTION_REGISTRATION_RECORD* pEndFrame, PVOID targetIp, PEXCEPTION_RECORD pRecord, PVOID retval, CONTEXT *context);
void WINAPI RtlUnwind( EXCEPTION_REGISTRATION_RECORD* pEndFrame, PVOID targetIp, PEXCEPTION_RECORD pRecord, PVOID retval )
{
	CONTEXT context;
    
    RtlCaptureContext( &context );
	__regs_RtlUnwind(pEndFrame, targetIp, pRecord, retval, &context); 
}

/*
__ASM_STDCALL_FUNC( _chkstk, 0,
                   "negl %eax\n\t"
                   "addl %esp,%eax\n\t"
                   "xchgl %esp,%eax\n\t"
                   "movl 0(%eax),%eax\n\t"  // copy return address from old location 
                   "movl %eax,0(%esp)\n\t"
                   "ret" )
				   */
__declspec(naked) void _chkstk()
{
	__asm {
		neg eax
		add eax, esp
		xchg eax, esp
		mov eax, [eax]
		mov [esp], eax
		ret
	}
}
