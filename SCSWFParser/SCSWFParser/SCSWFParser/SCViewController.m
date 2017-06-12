//
//  SCViewController.m
//  SCSWFParser
//
//  Created by scr on 14/05/08.
//  Copyright (c) 2014年 ohkawara shouichi. All rights reserved.
//

#import "SCViewController.h"
#import "SCSWFParser.h"

@interface SCViewController ()

@end

@implementation SCViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    SCSWFParser* swfParser = [[SCSWFParser alloc] initWithData:[NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"4" ofType:@"swf"]]];
    
    NSLog(@"%@: var %u, %lu bytes, {%hd, %hd, %hd, %hd}, %f fps, %u frames", swfParser.type, swfParser.version, swfParser.length, swfParser.rect.left, swfParser.rect.top, swfParser.rect.right, swfParser.rect.bottom, swfParser.frameRate, swfParser.totalOfFrames);
}

@end
