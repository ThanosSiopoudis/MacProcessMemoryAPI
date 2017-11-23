//
//  main.c
//  processmemoryapi
//
//  Created by Thanos Siopoudis on 26/03/2014.
//  Copyright (c) 2014 ___THANOSSIOPOUDIS___. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "main.h"

vm_map_t GetProcessTaskForPID(pid_t process)
{
    vm_map_t task;
	kern_return_t result = task_for_pid(current_task(), process, &task);
	if (result != KERN_SUCCESS)
	{
        return -1;
	}
    
    return task;
}

void FreeBytes(vm_map_t pTask, const void *bytes, mach_vm_size_t size)
{
    mach_vm_deallocate(current_task(), (vm_offset_t)bytes, size);
}

boolean_t ReadProcessMemory(vm_map_t ptask, mach_vm_address_t address, void **bytes, mach_vm_size_t *size)
{
    mach_vm_size_t originalSize = *size;
    vm_offset_t dataPointer = 0;
    mach_msg_type_number_t dataSize = 0;
    boolean_t success = FALSE;
    
    int mach_result = mach_vm_read(ptask, address, originalSize, &dataPointer, &dataSize);
    if (mach_result == KERN_SUCCESS)
    {
        success = TRUE;
        *bytes = (void *)dataPointer;
        *size = dataSize;
    }
	
	return success;
}

boolean_t CanReadAtAddress(pid_t pid, mach_vm_address_t address, int size) {
    mach_vm_size_t length = size;
    unsigned char *bytes = NULL;
    unsigned char *buffer = malloc(size);
    boolean_t success = FALSE;
    
    if (ReadProcessMemory(pid, address, (void**)&bytes, &length)) {
        if (bytes) {
            memcpy(buffer, bytes, length);
            success = TRUE;
        }
        
        FreeBytes(pid, bytes, size);
    }
	
	return success;
}

unsigned char *ReadProcessBytes(vm_map_t pTask, mach_vm_address_t address, int size) {
    mach_vm_size_t length = size;
    unsigned char *bytes = NULL;
    unsigned char *buffer = malloc(size);
    
    if (ReadProcessMemory(pTask, address, (void**)&bytes, &length)) {
        if (bytes) {
            memcpy(buffer, bytes, length);
        }
        
        FreeBytes(pTask, bytes, size);
    }
    
    return buffer;
}

uint64_t AllocateProcessBytes(vm_map_t pTask, mach_vm_size_t size) {
    ZGMemoryAddress address = 0;
    
    int mach_result = mach_vm_allocate(pTask, &address, size, VM_FLAGS_ANYWHERE);
    if (mach_result == KERN_SUCCESS) {
        return (uint64_t)address;
    }
    
    return 0;
}

boolean_t ZGFindASLRBaseAddress(ZGMemoryMap processTask, ZGMemoryAddress *addressPointer) {
    ZGMemoryAddress startAddress = 0x0;
    ZGMemorySize size;
    uint32_t nesting_depth = 0;
    struct vm_region_submap_info_64 vbr;
    mach_msg_type_number_t infoCount = 16;
    boolean_t success = FALSE;
    kern_return_t kr;
    
    if ((kr = mach_vm_region_recurse(processTask, &startAddress, &size, &nesting_depth, (vm_region_recurse_info_t)&vbr, &infoCount)) == KERN_SUCCESS) {
        *addressPointer = startAddress;
        success = TRUE;
    }
    
    return success;
}

uint64_t ZGGetASLROffset(vm_map_t pTask) {
    ZGMemoryAddress aslrAddress = 0;
        
    if (!ZGFindASLRBaseAddress(pTask, &aslrAddress)) {
        printf("Scheize");
    }
    
    if (aslrAddress == 0x0) { // Impossible. Use backup method
        aslrAddress = ZGGetASLROffsetBackup(pTask);
    }
    
    return (uint64_t)aslrAddress;
}

uint64_t ZGGetASLROffsetBackup(ZGMemoryMap processTask) {
    ZGMemoryAddress address = 0x0;
    ZGMemorySize size;
    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t infoCount = VM_REGION_BASIC_INFO_COUNT_64;
    mach_port_t objectName = MACH_PORT_NULL;
    
    if (mach_vm_region(processTask, &address, &size, VM_REGION_BASIC_INFO_64, (vm_region_info_t)&info, &infoCount, &objectName) != KERN_SUCCESS)
    {
        return 0;
    }
    
    return (uint64_t)(address + size);
}

uint32_t ReadInt32(pid_t pid, mach_vm_address_t address) {
    mach_vm_size_t size = sizeof(uint32_t);
    int retVal = 0;
    int *bytes = NULL;
    
    
    if (ReadProcessMemory(pid, address, (void**)&bytes, &size)) {
        if (bytes) {
            retVal = *bytes;
        }
        
        FreeBytes(pid, bytes, size);
    }
    
    return retVal;
}

boolean_t WriteProcessMemory(vm_map_t processTask, mach_vm_address_t address, const void *bytes, int size)
{
	return (mach_vm_write(processTask, address, (vm_offset_t)bytes, (mach_msg_type_number_t)size) == KERN_SUCCESS);
}

