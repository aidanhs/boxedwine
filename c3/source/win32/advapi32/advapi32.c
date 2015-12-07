#include "kmodule.h"

void AdjustTokenPrivileges_cpu(struct CPU* cpu) {
    U32 TokenHandle = pop32(cpu);
    U32 DisableAllPrivileges = pop32(cpu);
    U32 NewState = pop32(cpu);
    U32 BufferLength = pop32(cpu);
    U32 PreviousState = pop32(cpu);
    U32 ReturnLength = pop32(cpu);

    NOT_IMPLEMENTED("AdjustTokenPrivileges");
    EAX = 0;
}

void GetKernelObjectSecurity_cpu(struct CPU* cpu) {
    U32 Handle = pop32(cpu);
    U32 RequestedInformation = pop32(cpu);
    U32 pSecurityDescriptor = pop32(cpu);
    U32 nLength = pop32(cpu);
    U32 lpnLengthNeeded = pop32(cpu);

    NOT_IMPLEMENTED("GetKernelObjectSecurity");
    EAX = 0;
}

void GetSecurityDescriptorControl_cpu(struct CPU* cpu) {
    U32 pSecurityDescriptor = pop32(cpu);
    U32 pControl = pop32(cpu);
    U32 lpdwRevision = pop32(cpu);

    NOT_IMPLEMENTED("GetSecurityDescriptorControl");
    EAX = 0;
}

void GetSecurityDescriptorDacl_cpu(struct CPU* cpu) {
    U32 pSecurityDescriptor = pop32(cpu);
    U32 lpbDaclPresent = pop32(cpu);
    U32 pDacl = pop32(cpu);
    U32 lpbDaclDefaulted = pop32(cpu);

    NOT_IMPLEMENTED("GetSecurityDescriptorDacl");
    EAX = 0;
}

void GetSecurityDescriptorGroup_cpu(struct CPU* cpu) {
    U32 pSecurityDescriptor = pop32(cpu);
    U32 pGroup = pop32(cpu);
    U32 lpbGroupDefaulted = pop32(cpu);

    NOT_IMPLEMENTED("GetSecurityDescriptorGroup");
    EAX = 0;
}

void GetSecurityDescriptorLength_cpu(struct CPU* cpu) {
    U32 pSecurityDescriptor = pop32(cpu);

    NOT_IMPLEMENTED("GetSecurityDescriptorLength");
    EAX = 0;
}

void GetSecurityDescriptorOwner_cpu(struct CPU* cpu) {
    U32 pSecurityDescriptor = pop32(cpu);
    U32 pOwner = pop32(cpu);
    U32 lpbOwnerDefaulted = pop32(cpu);

    NOT_IMPLEMENTED("GetSecurityDescriptorOwner");
    EAX = 0;
}

void GetSecurityDescriptorSacl_cpu(struct CPU* cpu) {
    U32 pSecurityDescriptor = pop32(cpu);
    U32 lpbSaclPresent = pop32(cpu);
    U32 pSacl = pop32(cpu);
    U32 lpbSaclDefaulted = pop32(cpu);

    NOT_IMPLEMENTED("GetSecurityDescriptorSacl");
    EAX = 0;
}

void IsValidAcl_cpu(struct CPU* cpu) {
    U32 pAcl = pop32(cpu);

    NOT_IMPLEMENTED("IsValidAcl");
    EAX = 0;
}

void IsValidSecurityDescriptor_cpu(struct CPU* cpu) {
    U32 pSecurityDescriptor = pop32(cpu);

    NOT_IMPLEMENTED("IsValidSecurityDescriptor");
    EAX = 0;
}

void IsValidSid_cpu(struct CPU* cpu) {
    U32 pSid = pop32(cpu);

    NOT_IMPLEMENTED("IsValidSid");
    EAX = 0;
}

void LookupPrivilegeValueA_cpu(struct CPU* cpu) {
    U32 lpSystemName = pop32(cpu);
    U32 lpName = pop32(cpu);
    U32 lpLuid = pop32(cpu);

    NOT_IMPLEMENTED("LookupPrivilegeValueA");
    EAX = 0;
}

void OpenProcessToken_cpu(struct CPU* cpu) {
    U32 ProcessHandle = pop32(cpu);
    U32 DesiredAccess = pop32(cpu);
    U32 TokenHandle = pop32(cpu);

    NOT_IMPLEMENTED("OpenProcessToken");
    EAX = 0;
}

void SetKernelObjectSecurity_cpu(struct CPU* cpu) {
    U32 Handle = pop32(cpu);
    U32 SecurityInformation = pop32(cpu);
    U32 SecurityDescriptor = pop32(cpu);

    NOT_IMPLEMENTED("SetKernelObjectSecurity");
    EAX = 0;
}

struct KModule* createModule_advapi32() {
    struct KModule* result = createModule(0);
    strcpy(result->name, "advapi32.dll");
    strcpy(result->localDir, "c:\\Windows\\System32");
    
    addBuiltInFunction(result, "AdjustTokenPrivileges", AdjustTokenPrivileges_cpu);
    addBuiltInFunction(result, "GetKernelObjectSecurity", GetKernelObjectSecurity_cpu);    
    addBuiltInFunction(result, "GetSecurityDescriptorControl", GetSecurityDescriptorControl_cpu);
    addBuiltInFunction(result, "GetSecurityDescriptorDacl", GetSecurityDescriptorDacl_cpu);
    addBuiltInFunction(result, "GetSecurityDescriptorGroup", GetSecurityDescriptorGroup_cpu);
    addBuiltInFunction(result, "GetSecurityDescriptorLength", GetSecurityDescriptorLength_cpu);    
    addBuiltInFunction(result, "GetSecurityDescriptorOwner", GetSecurityDescriptorOwner_cpu);        
    addBuiltInFunction(result, "GetSecurityDescriptorSacl", GetSecurityDescriptorSacl_cpu);
    addBuiltInFunction(result, "IsValidAcl", IsValidAcl_cpu);    
    addBuiltInFunction(result, "IsValidSecurityDescriptor", IsValidSecurityDescriptor_cpu);
    addBuiltInFunction(result, "IsValidSid", IsValidSid_cpu);    
    addBuiltInFunction(result, "LookupPrivilegeValueA", LookupPrivilegeValueA_cpu);    
    addBuiltInFunction(result, "OpenProcessToken", OpenProcessToken_cpu);
    addBuiltInFunction(result, "SetKernelObjectSecurity", SetKernelObjectSecurity_cpu);
    
    return result;
}