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
        {
//            CCSize    m_obWinSizeInPoints = _eglViewWrapper->getDesignResolutionSize();
//            
//            // This method SHOULD be called only after openGLView_ was initialized
//            CCAssert(_eglViewWrapper, "opengl view should not be null");
//            
//            setAlphaBlending(true);
//            // XXX: Fix me, should enable/disable depth test according the depth format as cocos2d-iphone did
//            // [self setDepthTest: view_.depthFormat];
//            setDepthTest(false);
//            setProjection(m_eProjection);
//            
//            // set other opengl default values
//            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        }
        
//        setAlphaBlending(true);
//        // XXX: Fix me, should enable/disable depth test according the depth format as cocos2d-iphone did
//        // [self setDepthTest: view_.depthFormat];
//        setDepthTest(false);
//        setProjection(m_eProjection);
        
        // set other opengl default values
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    return self;
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


#pragma mark - Layer management

- (void)updateUsingCurrentTime {
    if(CMTIME_IS_INVALID(time)) {
        time = CMTimeMakeWithSeconds(0, 60);
        actualTimeOfLastUpdate = [NSDate timeIntervalSinceReferenceDate];
    } else {
        NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
        NSTimeInterval diff = now - actualTimeOfLastUpdate;
        time = CMTimeAdd(time, CMTimeMakeWithSeconds(diff, 60));
        actualTimeOfLastUpdate = now;
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    [self updateWithTimestamp:time];
}

- (void)updateWithTimestamp:(CMTime)frameTime {
    
    [GPUImageContext useImageProcessingContext];
    
    
    outputFramebuffer = [[GPUImageContext sharedFramebufferCache] fetchFramebufferForSize:_swfSize onlyTexture:YES];
    [outputFramebuffer activateFramebuffer];
    
    {
//        CCEGLView::sharedOpenGLView()->swapBuffers();
//        
//        _swfPlayer->draw();
    }
    
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

@end
