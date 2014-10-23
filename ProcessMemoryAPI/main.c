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

boolean_t GetProcessTaskForPID(pid_t process, vm_map_t *task)
{
	kern_return_t result = task_for_pid(current_task(), process, task);
	if (result != KERN_SUCCESS)
	{
		*task = MACH_PORT_NULL;
	}
	return (result == KERN_SUCCESS);
}

void FreeBytes(pid_t pid, const void *bytes, mach_vm_size_t size)
{
    vm_map_t processTask;
    if (GetProcessTaskForPID(pid, &processTask)) {
        mach_vm_deallocate(current_task(), (vm_offset_t)bytes, size);
    }
}

boolean_t ReadProcessMemory(pid_t pid, mach_vm_address_t address, void **bytes, mach_vm_size_t *size)
{
    vm_map_t processTask;
    mach_vm_size_t originalSize = *size;
    vm_offset_t dataPointer = 0;
    mach_msg_type_number_t dataSize = 0;
    boolean_t success = FALSE;
    
    if (GetProcessTaskForPID(pid, &processTask)) {
        int mach_result = mach_vm_read(processTask, address, originalSize, &dataPointer, &dataSize);
        if (mach_result == KERN_SUCCESS)
        {
            success = TRUE;
            *bytes = (void *)dataPointer;
            *size = dataSize;
        }
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

unsigned char *ReadProcessBytes(pid_t pid, mach_vm_address_t address, int size) {
    mach_vm_size_t length = size;
    unsigned char *bytes = NULL;
    unsigned char *buffer = malloc(size);
    
    if (ReadProcessMemory(pid, address, (void**)&bytes, &length)) {
        if (bytes) {
            memcpy(buffer, bytes, length);
        }
        
        FreeBytes(pid, bytes, size);
    }
    
    return buffer;
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

