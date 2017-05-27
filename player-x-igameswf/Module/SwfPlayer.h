//
//  SwfPlayer.h
//  igameswf
//
//  Created by fallen.ink on 23/05/2017.
//
//

#import <Foundation/Foundation.h>

struct mouse_event {
    int m_x;
    int m_y;
    int m_state;
};

@class SwfPlayer;
@protocol SwfPlayerDelegate <NSObject>

- (void)onMouseDown:(double)x :(double)y;
- (void)onMouseUp:(double)x :(double)y;
- (void)onMouseMove:(double)x :(double)y;

@end

extern SwfPlayer *__SWFPlayer;

@interface SwfPlayer : NSObject <SwfPlayerDelegate>

//- (instancetype)initWithFilepath:(NSString *)path preferredSize:(CGSize)size;

/**
 *  setFilePath:, setPreferredSize: 调用关系不能调换
 */
- (void)setFilePath:(NSString *)path error:(NSError **)error;
- (void)setPreferredSize:(CGSize)size;

- (CGSize)getVideoSize;
- (CGSize)getActualSize;

/**
 *  先尝试用 后接 transform filter 来实现 图像 旋转、放大、移动
 */
- (void)update;

@end
