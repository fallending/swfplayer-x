//
//  igameswfAppDelegate.m
//  igameswf
//
//  Created by Vitaly Alexeev on 6/14/09.
//  Copyright Home 2009. All rights reserved.
//

#import "igameswfAppDelegate.h"
#import "RootViewController.h"

@implementation igameswfAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    self.window.backgroundColor = [UIColor whiteColor];
    self.window.rootViewController = [[UINavigationController alloc] initWithRootViewController:[RootViewController new]];
    [self.window makeKeyAndVisible];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
//	glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
//	glView.animationInterval = 1.0 / 60.0;
}

@end
