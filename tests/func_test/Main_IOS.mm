/**
 * @file    Main_IOS.mm
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/24
 * @version 1.0
 *
 * @brief
 */

#import <UIKit/UIKit.h>
#import "iOSSpecific/AppDelegate.h"

int main(int argc, char *argv[])
{
    NSLog(@"Hello, For test!");
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
