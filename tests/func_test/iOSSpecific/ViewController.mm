//
//  ViewController.m
//  testsuite
//
//  Created by kingsoft on 16/10/3.
//  Copyright © 2016年 lailongwei. All rights reserved.
//
#import "ViewController.h"

#import "TestSuite.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    CGRect rect = [UIScreen mainScreen].bounds;
    CGFloat screenWidth = rect.size.width;
    CGFloat screenHeight = rect.size.height;
    
    // library name label.
    UILabel *libNameLabel = [[UILabel alloc] initWithFrame:CGRectMake(screenWidth / 2 - 100, 50, 200, 20)];
    libNameLabel.text = @"llbc library test suite";
    libNameLabel.backgroundColor = [UIColor blueColor];
    libNameLabel.textAlignment = NSTextAlignmentCenter;
    [self.view addSubview:libNameLabel];
    
    // version info label.
    UILabel *verLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 100, screenWidth - 20, 80)];
    verLabel.text = [NSString stringWithFormat:@"version: %s", LLBC_GetVersionInfo().c_str()];
    verLabel.lineBreakMode = NSLineBreakByWordWrapping;
    verLabel.numberOfLines = 0;
    verLabel.backgroundColor = [UIColor greenColor];
    verLabel.textColor = [UIColor blackColor];
    verLabel.textAlignment = NSTextAlignmentCenter;
    [self.view addSubview:verLabel];
    
    // Add prompt label.
    UILabel *promptLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, screenHeight - 100, screenWidth - 20, 80)];
    promptLabel.textAlignment = NSTextAlignmentLeft;
    promptLabel.lineBreakMode = NSLineBreakByWordWrapping;
    promptLabel.numberOfLines = 0;
    promptLabel.text = @"All test output in Xcode output window!";
    [self.view addSubview:promptLabel];
    
    // Begin test
    int argc = 1;
    const char *argv[1];
    argv[0] = "hello";
    TestSuite_Main(argc, (char **)argv);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
