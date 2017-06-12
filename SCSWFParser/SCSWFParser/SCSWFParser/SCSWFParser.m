//
//  SCSWFParser.m
//  SWFParser
//
//  Created by scr on 14/05/06.
//  Copyright (c) 2014年 ohkawara shouichi. All rights reserved.
//

#import "SCSWFParser.h"

@implementation SCSWFParser
{
    unsigned char* _bytes;
}

- (instancetype)initWithData:(NSData*)data
{
    if ([data length] < 8)
        return nil;
    
    self = [super init];
    if (self)
    {
        // load 8 bytes
        unsigned char bytes[8];
        [data getBytes:bytes range:NSMakeRange(0, 8)];
        
        // load type SWF or CWF
        _type = [NSString stringWithFormat:@"%c%c%c", bytes[0], bytes[1], bytes[2]];
        
        // load version
        _version = bytes[3];
        
        // load file size
        unsigned long long t = 0;
        [[NSScanner scannerWithString:[NSString stringWithFormat:@"%02x%02x%02x%02x", bytes[7], bytes[6], bytes[5], bytes[4]]] scanHexLongLong:&t];
        _length = t;
        
        // unzip if needed
        data = [data subdataWithRange:NSMakeRange(8, [data length] - 8)];
        if ([_type isEqualToString:@"CWS"])
        {
            // copy data without initial 8 bytes, and save as a temporary
            // TO DO
//            data = [ZipUtil unzip:data];
            return self;
        }
        
        // load all bytes without initial 8 bytes
        _bytes = malloc(_length - 8);
        [data getBytes:_bytes range:NSMakeRange(0, _length - 8)];
        
        const int numberOfBitsPerCorner = (_bytes[0] >> 3);
        NSLog(@"1角当たり必要なビット数 %d", numberOfBitsPerCorner);
        
        UInt32 index = 0;
        short* rectCoordinates[] = {&_rect.left, &_rect.right, &_rect.top, &_rect.bottom};
        int offset = 5;
        int bitCount = 0;
        for (UInt32 i = 0, j = 0; j < 4; ++j)
        {
            UInt32 value = 0;
            for (; i < _length - 8; ++i)
            {
                unsigned char byte = _bytes[i];
                
                int bitSize = 8 - offset;
                int maskOffset = 0;
                offset = 0;
                if (numberOfBitsPerCorner - bitCount < bitSize)
                {
                    bitSize = numberOfBitsPerCorner - bitCount;
                    offset = bitSize;
                    maskOffset = 8 - bitSize;
                }
                
                //     1 ???????? & 00000111 = 00000??? => ???----------------...
                // 2~n-1 ???????? & 11111111 = ???????? => ...----????????----...
                //     n ???????? & 11000000 = ??000000 => ...-----------------??
                value |= ((byte & (((1 << bitSize) - 1) << maskOffset)) >> maskOffset) << (numberOfBitsPerCorner - bitCount - bitSize);
                bitCount += bitSize;
                
                if (numberOfBitsPerCorner <= bitCount)
                {
                    bitCount = 0;
                    break;
                }
            }
            
            // 20 twips = 1px
            *rectCoordinates[j] = value / 20;
            
            index = i;
        }
        
        // load frame rate
        [[NSScanner scannerWithString:[NSString stringWithFormat:@"%02x", _bytes[index + 1]]] scanHexFloat:&_frameRate];
        [[NSScanner scannerWithString:[NSString stringWithFormat:@"%02x", _bytes[index + 2]]] scanHexLongLong:&t];
        _frameRate += t;
        
        // load total of frames
        [[NSScanner scannerWithString:[NSString stringWithFormat:@"%02x%02x", _bytes[index + 4], _bytes[index + 3]]] scanHexLongLong:&t];
        _totalOfFrames = t;
        
        // load contents
        index += 5;
        // TO DO
    }
    return self;
}

- (void)dealloc
{
    free(_bytes);
}

@end
