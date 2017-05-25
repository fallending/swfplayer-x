//
//  main.m
//  mac
//
//  Created by Vitaly Alexeev on 6/9/09.
//  Copyright Home 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SwfPlayer.h"
#import "igameswfAppDelegate.h"

#define LOG(FORMAT, ...) {\
NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];\
[dateFormatter setDateStyle:NSDateFormatterMediumStyle];\
[dateFormatter setTimeStyle:NSDateFormatterShortStyle];\
[dateFormatter setDateFormat:@"HH:mm:ss:SSSSSS"]; \
NSString *str = [dateFormatter stringFromDate:[NSDate date]]; \
fprintf(stderr,"[--%s--]*[--%s--]*[--%s:%d--]\n",[str UTF8String], [[NSString stringWithFormat:FORMAT, ##__VA_ARGS__] UTF8String],[[[NSString stringWithUTF8String:__FILE__] lastPathComponent] UTF8String], __LINE__);\
}

int main(int argc, char *argv[]) {
    return UIApplicationMain(argc, argv, nil, NSStringFromClass([igameswfAppDelegate class]));
}
