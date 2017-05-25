//
//  GPUSwfRender.h
//  GPUImageTest-2
//
//  Created by fallen.ink on 23/05/2017.
//  Copyright © 2017 hanlan_iMac. All rights reserved.
//

#import "GPUImageOutput.h"

@interface GPUSwfRender : GPUImageOutput

- (id)initWithSwfPath:(NSString *)path;

- (void)startProcessing;

- (void)endProcessing;

@end
