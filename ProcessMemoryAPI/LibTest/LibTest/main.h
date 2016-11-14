//
//  main.h
//  processmemoryapi
//
//  Created by Thanos Siopoudis on 27/03/2014.
//  Copyright (c) 2014 ___THANOSSIOPOUDIS___. All rights reserved.
//

#include <stdio.h>
#include <mach/mach_init.h>
#include <mach/mach_vm.h>

#ifndef processmemoryapi_main_h
#define processmemoryapi_main_h

typedef vm_map_t ZGMemoryMap;
typedef vm_region_basic_info_data_64_t ZGMemoryBasicInfo;
typedef mach_vm_address_t ZGMemoryAddress;
typedef uint32_t ZG32BitMemoryAddress;
typedef mach_vm_size_t ZGMemorySize;

vm_map_t GetProcessTaskForPID(pid_t process);
boolean_t ReadProcessMemory(vm_map_t ptask, mach_vm_address_t address, void **bytes, mach_vm_size_t *size);
boolean_t WriteProcessMemory(vm_map_t processTask, mach_vm_address_t address, const void *bytes, int size);
unsigned char *ReadProcessBytes(vm_map_t ptask, mach_vm_address_t address, int size);
uint32_t ReadInt32(pid_t pid, mach_vm_address_t address);
boolean_t CanReadAtAddress(pid_t pid, mach_vm_address_t address, int size);
boolean_t ZGFindASLRBaseAddress(ZGMemoryMap processTask, ZGMemoryAddress *addressPointer);
uint64_t ZGGetASLROffset(vm_map_t pTask);
uint64_t ZGGetASLROffsetBackup(ZGMemoryMap processTask);

#endif
