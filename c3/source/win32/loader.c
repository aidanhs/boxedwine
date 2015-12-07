#include "loader.h"
#include "kalloc.h"
#include "kmodule.h"
#include "kprocess.h"
#include "stringutils.h"
#include "pathutils.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include UNISTD

void getFullNativePath(const char* currentDirectory, const char* localFile, char* nativeFile);

void loadExports(struct KModule* module, IMAGE_EXPORT_DIRECTORY* exportDirectory) {
    if (exportDirectory->NumberOfFunctions) {
        U32 i;

        module->exportsByOrdinal = (int**)kalloc(sizeof(void*)*exportDirectory->NumberOfFunctions);
        for (i=0;i<exportDirectory->NumberOfFunctions;i++) {
            module->exportsByOrdinal[i] = (U32*)(module->baseAddress + exportDirectory->AddressOfFunctions + 4*i);            
        }

        for (i=0;i<exportDirectory->NumberOfFunctions;i++) {
            const char* name = (const char*)(*(U32*)(module->baseAddress + exportDirectory->AddressOfNames + 4*i));
            U16 ordinal = *(U16*)(module->baseAddress + exportDirectory->AddressOfNameOrdinals + 2*i);            
            pblMapAdd(module->exportsByName, (void*)name, strlen(name)+1, &module->exportsByOrdinal[ordinal], sizeof(U32));
        }
    }
}

U32 loadImports(struct KProcess* process, struct KModule* module, IMAGE_IMPORT_DESCRIPTOR* desc) {
    while (desc->u.Characteristics) {
        char* name = (char*)(module->baseAddress+desc->Name);
        IMAGE_THUNK_DATA32* thunkData;
        struct KModule* importModule = getModuleByName(process, name);

        if (!importModule) {
            importModule = loadModule(process, name);
        }
        if (!importModule) {
            return 0;
        }
        thunkData = (IMAGE_THUNK_DATA32*)(desc->FirstThunk+module->baseAddress);
        while (thunkData->u1.AddressOfData) {
            U32 thunk = 0;

            if (thunkData->u1.Ordinal & 0x80000000) {
                thunk = getProcByOrdinal(module, thunkData->u1.Ordinal & 0xFFFF);
                if (!thunk) {
                    printf("Failed to import ordinal %d from %s\n", thunkData->u1.Ordinal & 0xFFFF, importModule->name);
                }
            } else {
                IMAGE_IMPORT_BY_NAME* name = (IMAGE_IMPORT_BY_NAME*)(thunkData->u1.AddressOfData+module->baseAddress);
                thunk = getProcByName(importModule, name->Name);
                if (!thunk) {
                    printf("Failed to import %s from %s\n", name->Name, importModule->name);
                }
            }
            writed((U32)thunkData, thunk);
            thunkData++;
        }
        desc++;
    }
    return 1;
}

U32 doesPathExist(const char* localDir, const char* localName, char* nativePath) {
    struct stat s;

    getFullNativePath(localDir, localName, nativePath);
    if (stat(nativePath, &s)) {
        // on case sensitive filesystems, we should search for this file
        return 0;
    }
    return 1;
}

struct KModule* loadModule(struct KProcess* process, const char* name) {    
    PIMAGE_DOS_HEADER dosHeader;
    IMAGE_NT_HEADERS* nt;
    S32 f;
    U8 buffer[1024];
    struct KModule* module;
    IMAGE_SECTION_HEADER* section;
    int i;
    U32 size;
    U32 relocated = 0;
    char nativePath[MAX_FILEPATH_LEN];
    char localDir[MAX_FILEPATH_LEN];
    char localName[MAX_FILEPATH_LEN];

    splitIntoDirAndName(name, localDir, localName);
    if (localDir[0]) {
        if (!doesPathExist(localDir, localName, nativePath)) {
            return 0;
        }
    } else if (process->mainModule && doesPathExist(process->mainModule->localDir, localName, nativePath)) {
        
    } else if (doesPathExist(process->currentDirectory, localName, nativePath)) {

    } else if (doesPathExist("c:\\windows\\system32", localName, nativePath)) {

    } else if (doesPathExist("c:\\windows\\system", localName, nativePath)) {

    } else if (doesPathExist("c:\\windows", localName, nativePath)) {

    } else {
        // :TODO: iterate path
        return 0;
    }

    f = open(nativePath, O_RDONLY|O_BINARY);
    if (f<0)
        return 0;
    if (read(f, buffer, 1024)!=1024) {
        close(f);
        return 0;
    }
    
    dosHeader = (PIMAGE_DOS_HEADER)buffer;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        close(f);
        return 0;
    }
    nt = (IMAGE_NT_HEADERS*)(buffer+dosHeader->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        close(f);
        return 0;
    }
    if (nt->FileHeader.Machine!=IMAGE_FILE_MACHINE_I386) {
        close(f);
        return 0;
    }
    section = (IMAGE_SECTION_HEADER*)(buffer+dosHeader->e_lfanew+sizeof(IMAGE_FILE_HEADER)+nt->FileHeader.SizeOfOptionalHeader+4);
    size = 0;
    for (i=0;i<nt->FileHeader.NumberOfSections;i++) {
        int count = section->Misc.VirtualSize;
        if (!count)
            count = section->SizeOfRawData;
        if (section->VirtualAddress+count>size)
            size = section->VirtualAddress+count;
        section++;
    }
    section = (IMAGE_SECTION_HEADER*)(buffer+dosHeader->e_lfanew+sizeof(IMAGE_FILE_HEADER)+nt->FileHeader.SizeOfOptionalHeader+4);
    module = createModule(1);
    module->pointer = kalloc(size+0x10000);
    module->baseAddress = (U8*)(((U32)module->pointer + 0xFFFF) & 0xFFFF0000);
    module->size = size;
    convertStringToLower(localName, module->name);
    strcpy(module->localDir, localDir);    

    for (i=0;i<nt->FileHeader.NumberOfSections;i++) {
        if (section->SizeOfRawData) {
            lseek(f, section->PointerToRawData, SEEK_SET);
            read(f, module->baseAddress+section->VirtualAddress, section->SizeOfRawData);      
        }
        section++;
    }    
    if (IMAGE_DIRECTORY_ENTRY_BASERELOC < nt->OptionalHeader.NumberOfRvaAndSizes) {
        if (nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) {
            relocated = 1;
            // :TODO: reloc
        }
    }
    if (!relocated) {
        U32 baseStart;
        U32 baseEnd;
        U32 baseOffset;

        module->originalImageBase = nt->OptionalHeader.ImageBase;

        baseStart = module->originalImageBase;
        baseEnd = module->originalImageBase + module->size;
        baseOffset = (U32)module->baseAddress - module->originalImageBase;

        section = (IMAGE_SECTION_HEADER*)(buffer+dosHeader->e_lfanew+sizeof(IMAGE_FILE_HEADER)+nt->FileHeader.SizeOfOptionalHeader+4);
        for (i=0;i<nt->FileHeader.NumberOfSections;i++) {
            // :TODO: need to research this, why not rdata?
            if (section->SizeOfRawData && !strcmp(section->Name, ".data")) {
                U32 j;
                for (j=0;j<section->SizeOfRawData;j+=4) {
                    U32 r = readd((U32)module->baseAddress+section->VirtualAddress+j);
                    if (r>=baseStart && r<baseEnd) {
                        writed((U32)module->baseAddress+section->VirtualAddress+j, r+baseOffset);
                    }
                }
            }
            section++;
        }
    }
    if (IMAGE_DIRECTORY_ENTRY_EXPORT < nt->OptionalHeader.NumberOfRvaAndSizes) {
        if (nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress) {
            loadExports(module, (IMAGE_EXPORT_DIRECTORY*)(nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress+module->baseAddress));
        }
    }
    pblMapAdd(process->modules, module->name, strlen(module->name)+1, &module, sizeof(struct KModule*));

    if (IMAGE_DIRECTORY_ENTRY_IMPORT < nt->OptionalHeader.NumberOfRvaAndSizes) {
        if (nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) {
            if (!loadImports(process, module, (IMAGE_IMPORT_DESCRIPTOR*)(nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress+module->baseAddress))) {
                return 0;
            }
        }
    }    

    close(f);
    module->startAddress = nt->OptionalHeader.AddressOfEntryPoint + (U32)module->baseAddress;
    module->handle = allocHandle(process, HANDLE_MODULE);
    process->handles[module->handle].module = module;
    return module;
}