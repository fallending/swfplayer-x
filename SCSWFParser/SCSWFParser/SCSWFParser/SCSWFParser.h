//
//  SCSWFParser.h
//  SWFParser
//
//  Created by scr on 14/05/06.
//  Copyright (c) 2014年 ohkawara shouichi. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SCSWFParser : NSObject

- (instancetype)initWithData:(NSData*)data;

// header
@property (readonly) NSString* type;
@property (readonly) UInt8 version;
@property (readonly) UInt32 length;

// movie header
@property (readonly) Rect rect;
@property (readonly) Float32 frameRate;
@property (readonly) UInt16 totalOfFrames;

@end
