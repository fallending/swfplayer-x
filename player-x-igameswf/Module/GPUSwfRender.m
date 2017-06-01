//
//  GPUSwfRender.m
//  GPUImageTest-2
//
//  Created by fallen.ink on 23/05/2017.
//  Copyright © 2017 hanlan_iMac. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "GPUSwfRender.h"
#import "SwfPlayer.h"

#define USE_LAYER_CONTEXT 0

/**
 *  1. 只关注刷新时间，不关注swf本身的pts
 */

@interface GPUSwfRender () {
    CADisplayLink *displayLink;
    
    CAEAGLLayer *_layer;
    EAGLContext *_context;
    
    GLuint _renderBuffer;
    GLuint _frameBuffer;
    
    GLuint _outputFrameBuffer;
    
    CGSize previousLayerSizeInPixels;
    CMTime time;
    NSTimeInterval actualTimeOfLastUpdate;
    
    
    
    CGSize _swfSize;
}

@end

@implementation GPUSwfRender

- (id)initWithSwfPath:(NSString *)path {
    if (self = [super init]) {
        NSError *error = nil;
        
        [__SWFPlayer setFilePath:path error:&error];

        if (error) {
            NSLog(@"Swf render init failed!!");
            
            return nil;
        }
        
        _swfSize = [__SWFPlayer getVideoSize];
        
        [__SWFPlayer setPreferredSize:[UIScreen mainScreen].bounds.size];
        
        _swfSize = [__SWFPlayer getActualSize];
        
#if USE_LAYER_CONTEXT
        [self setupLayer];
        
        [self setupContext];
#endif
        
        [self createFramebuffer];
    }
    
    return self;
}

- (void)dealloc {
    [self destroyFramebuffer];
}

- (void)startProcessing {
    runSynchronouslyOnVideoProcessingQueue(^{
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateUsingCurrentTime)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        [displayLink setPaused:NO];
        
        [self updateUsingCurrentTime];
    });
}

- (void)endProcessing {
    [displayLink setPaused:YES];
    
    for (id<GPUImageInput> currentTarget in targets) {
        [currentTarget endProcessing];
    }

    if ((displayLink != nil)) {
        [displayLink invalidate]; // remove from all run loops
        displayLink = nil;
    }

}

- (void)cancelProcessing {
    // 关闭 swf player
    [self endProcessing];
}

#pragma mark -

- (void)setupLayer {
    _layer = [CAEAGLLayer layer];
    _layer.opaque = YES;
    _layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                 [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
}

- (void)setupContext {
    // 指定OpenGL 渲染 API 的版本，目前都使用 OpenGL ES 2.0
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    
    // 创建EAGLContext上下文
    _context = [[EAGLContext alloc] initWithAPI:api];
    
    // 设置为当前上下文，所有的渲染默认渲染到当前上下文
    [EAGLContext setCurrentContext:_context];
}

#pragma mark - Layer management

- (void)updateUsingCurrentTime {
    if(CMTIME_IS_INVALID(time)) {
        time = CMTimeMakeWithSeconds(0, 600);
        actualTimeOfLastUpdate = [NSDate timeIntervalSinceReferenceDate];
    } else {
        NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
        NSTimeInterval diff = now - actualTimeOfLastUpdate;
        time = CMTimeAdd(time, CMTimeMakeWithSeconds(diff, 600));
        actualTimeOfLastUpdate = now;
    }
    
    [self updateWithTimestamp:time];
}

- (void)updateWithTimestamp:(CMTime)frameTime {
    
    [GPUImageContext useImageProcessingContext];
    
    
    outputFramebuffer = [[GPUImageContext sharedFramebufferCache] fetchFramebufferForSize:_swfSize onlyTexture:YES];
    [outputFramebuffer activateFramebuffer];
    
    
//    glActiveTexture(GL_TEXTURE4);
//    glBindTexture(GL_TEXTURE_2D, [outputFramebuffer texture]);
    
#if USE_LAYER_CONTEXT
    [EAGLContext setCurrentContext:_context];
#endif
    
//    glBindFramebufferOES(GL_FRAMEBUFFER_OES, [outputFramebuffer texture]);
    
    [__SWFPlayer update];
    
//    [_context presentRenderbuffer:GL_RENDERBUFFER_OES];
    
    
    
    // First, update all the framebuffers in the targets
    for (id<GPUImageInput> currentTarget in targets) {
        if (currentTarget != self.targetToIgnoreForUpdates) {
            NSInteger indexOfObject = [targets indexOfObject:currentTarget];
            NSInteger textureIndexOfTarget = [[targetTextureIndices objectAtIndex:indexOfObject] integerValue];
            
            if (currentTarget != self.targetToIgnoreForUpdates) {
                [currentTarget setInputSize:_swfSize atIndex:textureIndexOfTarget];
                [currentTarget setInputFramebuffer:outputFramebuffer atIndex:textureIndexOfTarget];
            }
        }
    }
    
    // Then release our hold on the local framebuffer to send it back to the cache as soon as it's no longer needed
    [outputFramebuffer unlock];
    outputFramebuffer = nil;
    
    // Finally, trigger rendering as needed
    for (id<GPUImageInput> currentTarget in targets) {
        if (currentTarget != self.targetToIgnoreForUpdates) {
            NSInteger indexOfObject = [targets indexOfObject:currentTarget];
            NSInteger textureIndexOfTarget = [[targetTextureIndices objectAtIndex:indexOfObject] integerValue];
            
            if (currentTarget != self.targetToIgnoreForUpdates) {
                [currentTarget newFrameReadyAtTime:frameTime atIndex:textureIndexOfTarget];
            }
        }
    }
}

#pragma mark -

- (BOOL)createFramebuffer {
//    outputFramebuffer = [[GPUImageContext sharedFramebufferCache] fetchFramebufferForSize:_swfSize onlyTexture:YES];
//    [outputFramebuffer activateFramebuffer];
    
#if USE_LAYER_CONTEXT
    glGenRenderbuffers(1, &_renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
    
    // 把渲染缓存绑定到渲染图层上CAEAGLLayer，并为它分配一个共享内存。
    // 并且会设置渲染缓存的格式，和宽度
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_layer];
    
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    // 把颜色渲染缓存 添加到 帧缓存的GL_COLOR_ATTACHMENT0上,就会自动把渲染缓存的内容填充到帧缓存，在由帧缓存渲染到屏幕
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);
#endif
    
    return YES;
}

- (void)destroyFramebuffer {
#if USE_LAYER_CONTEXT
    glDeleteFramebuffersOES(1, &_frameBuffer);
    _frameBuffer = 0;
    
    glDeleteRenderbuffersOES(1, &_renderBuffer);
    _renderBuffer = 0;
#endif
}

@end
