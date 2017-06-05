//
//  TangooBabyAppController.h
//  TangooBaby
//
//  Created by chenee on 13-6-20.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
//  6af0开始的不需要提交，记得去掉。。。。。。。。。

@class RootViewController;

@interface AppController : NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate> {
    UIWindow *window;
    RootViewController    *viewController;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) RootViewController *viewController;

@end

