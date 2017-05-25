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

@interface GPUSwfRender () {
    CADisplayLink *displayLink;
    
    CAEAGLLayer *_layer;
    EAGLContext *_context;
    
    CGSize previousLayerSizeInPixels;
    CMTime time;
    NSTimeInterval actualTimeOfLastUpdate;
    
    GLuint _renderBuffer;
    GLuint _outputFrameBuffer;
}

@end

@implementation GPUSwfRender

- (id)initWithSwfPath:(NSString *)path {
    if (self = [super init]) {
        NSError *error = nil;
        
        [__SWFPlayer setFilePath:path error:&error];

        if (error) {
            return nil;
        }
        
        [self setupLayer];
        
//        _context = [GPUImageContext sharedImageProcessingContext].context;
        [self setupContext];
        
        [__SWFPlayer setPreferredSize:[UIScreen mainScreen].bounds.size];
        
        [self createFramebuffer];
    }
    
    return self;
}

- (void)startProcessing {
    runSynchronouslyOnVideoProcessingQueue(^{
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateUsingCurrentTime)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        [displayLink setPaused:YES];
        
        [self updateUsingCurrentTime];
    });
}

- (void)endProcessing {
//    keepLooping = NO;
    [displayLink setPaused:YES];
    
    for (id<GPUImageInput> currentTarget in targets) {
        [currentTarget endProcessing];
    }
    
//    if (synchronizedMovieWriter != nil)
//    {
//        [synchronizedMovieWriter setVideoInputReadyCallback:^{return NO;}];
//        [synchronizedMovieWriter setAudioInputReadyCallback:^{return NO;}];
//    }
//    
    if ((displayLink != nil)) {
        [displayLink invalidate]; // remove from all run loops
        displayLink = nil;
    }
    
//    if ([self.delegate respondsToSelector:@selector(didCompletePlayingMovie)]) {
//        [self.delegate didCompletePlayingMovie];
//    }
//    self.delegate = nil;
}

- (void)cancelProcessing
{
//    if (reader) {
//        [reader cancelReading];
//    }
    
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

//- (void)update;
//{
//    [self updateWithTimestamp:kCMTimeIndefinite];
//}

- (void)updateUsingCurrentTime;
{
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

- (void)updateWithTimestamp:(CMTime)frameTime; {
    [GPUImageContext useImageProcessingContext];
    
    CGSize size = [UIScreen mainScreen].bounds.size;
    
    [EAGLContext setCurrentContext:_context];
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, [outputFramebuffer texture]);
    
    // clear screen
    glViewport(0, 0, size.width, size.height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    [__SWFPlayer update];
    
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, [outputFramebuffer texture]);
    
    [_context presentRenderbuffer:GL_RENDERBUFFER_OES];
    
    
//    [GPUImageContext useImageProcessingContext];
    
//    glBindTexture(GL_TEXTURE_2D, [outputFramebuffer texture]);
    // no need to use self.outputTextureOptions here, we always need these texture options
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)layerPixelSize.width, (int)layerPixelSize.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
    
    for (id<GPUImageInput> currentTarget in targets)
    {
        if (currentTarget != self.targetToIgnoreForUpdates)
        {
            NSInteger indexOfObject = [targets indexOfObject:currentTarget];
            NSInteger textureIndexOfTarget = [[targetTextureIndices objectAtIndex:indexOfObject] integerValue];
            
            [currentTarget setInputSize:[__SWFPlayer getActualSize] atIndex:textureIndexOfTarget];
            [currentTarget newFrameReadyAtTime:frameTime atIndex:textureIndexOfTarget];
        }
    }    
}

#pragma mark -

- (BOOL)createFramebuffer {
    glGenRenderbuffers(1, &_renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
    
    // 把渲染缓存绑定到渲染图层上CAEAGLLayer，并为它分配一个共享内存。
    // 并且会设置渲染缓存的格式，和宽度
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_layer];
    
    glGenFramebuffers(1, &_outputFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _outputFrameBuffer);
    // 把颜色渲染缓存 添加到 帧缓存的GL_COLOR_ATTACHMENT0上,就会自动把渲染缓存的内容填充到帧缓存，在由帧缓存渲染到屏幕
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);

//    glGenFramebuffersOES(1, &outputFramebuffer);
////    glGenRenderbuffersOES(1, &viewRenderbuffer);
//    
//    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
//    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
//    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
//    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
//    
//    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
//    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
//    
//    if (USE_DEPTH_BUFFER) {
//        glGenRenderbuffersOES(1, &depthRenderbuffer);
//        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
//        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
//        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
//    }
//    
//    //		const GLint GL_STENCIL_INDEX8_OES = 0x8D48;
//    //		glGenRenderbuffersOES(1, &stencilRenderbuffer);
//    //		glBindRenderbufferOES(GL_RENDERBUFFER_OES, stencilRenderbuffer);
//    //		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_STENCIL_INDEX8_OES, backingWidth, backingHeight);
//    //		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_STENCIL_ATTACHMENT_OES, GL_RENDERBUFFER_OES, stencilRenderbuffer);
//    
//    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
//    {
//        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
//        return NO;
//    }
//    
    
    
    return YES;
}

- (void)destroyFramebuffer {
//    glDeleteFramebuffersOES(1, &viewFramebuffer);
//    viewFramebuffer = 0;
//    
//    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
//    viewRenderbuffer = 0;
//    
//    if(depthRenderbuffer) {
//        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
//        depthRenderbuffer = 0;
//    }
}

@end
