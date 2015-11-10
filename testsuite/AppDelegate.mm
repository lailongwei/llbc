//
//  AppDelegate.m
//  testsuite
//
//  Created by bo on 13-8-24.
//  Copyright (c) 2013年 llbc. All rights reserved.
//

#include "llbc.h"
using namespace llbc;

#import "AppDelegate.h"
#import "TestSuite.h"

@implementation AppDelegate

- (void)dealloc
{
    [_window release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
    
    // Add lib label.
    UILabel *libLabel = [[UILabel alloc] initWithFrame:CGRectMake(60, 10, 200, 50)];
    [libLabel setText:@"llbc library test suite!"];
    [libLabel setBackgroundColor:[UIColor blueColor]];
    [libLabel setTextAlignment:NSTextAlignmentCenter];
    [self.window addSubview:libLabel];
    [libLabel release];
    
    // Add ver label.
    UILabel *verLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 100, 300, 50)];
    [verLabel setText:[NSString stringWithFormat:@"ver: %s", LLBC_GetVersionInfo().c_str()]];
    [verLabel setBackgroundColor:[UIColor greenColor]];
    [verLabel setTextAlignment:NSTextAlignmentCenter];
    [self.window addSubview:verLabel];
    [verLabel release];
    
    int argc = 3;
    const char *argv[3];
    argv[0] = "hello";
    argv[1] = "192.168.7.193";
    argv[2] = "7778";
    
    TestSuite_Main(argc, (char **)argv);
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
