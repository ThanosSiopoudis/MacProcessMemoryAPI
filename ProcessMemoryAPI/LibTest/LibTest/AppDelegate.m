//
//  AppDelegate.m
//  LibTest
//
//  Created by Thanos Siopoudis on 27/03/2014.
//  Copyright (c) 2014 ___THANOSSIOPOUDIS___. All rights reserved.
//

#import "AppDelegate.h"
#import "main.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    unsigned char *buffer;
    
    // int32_t result = ReadInt32(27051, 0x6FFFFFFF);
    vm_map_t task = GetProcessTaskForPID(81490);
    uint64_t newPage = AllocateProcessBytes(task, 4);
    
    
    //uint64_t aslr = ZGGetASLROffset(task);
//    bool isreadable = CanReadAtAddress(44291, 0x6FFFFFFF, 1);
//    bool res = ReadProcessMemory(44291, 0x6FFFFFFF, (void **)&buffer, (unsigned long long)1);
//    buffer = ReadProcessBytes(44291, 0x6FFFFFFF, 1);
    
    NSLog(@"Shit");
}

@end
