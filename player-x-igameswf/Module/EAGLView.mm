//
//  EAGLView.m
//  mac
//
//  Created by Vitaly Alexeev on 6/9/09.
//  Copyright Home 2009. All rights reserved.
//



#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#include <functional>
#import "EAGLView.h"
#import "SwfPlayer.h"

#define USE_DEPTH_BUFFER 0

// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end


@implementation EAGLView

@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;

// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder *)coder {
    if ((self = [super initWithCoder:coder])) {
        self.userInteractionEnabled = YES;
        self.backgroundColor = [UIColor clearColor];
        
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        self.layer.backgroundColor = [UIColor clearColor].CGColor;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context]) {
            
            return nil;
        }
		
        animationInterval = 1.0f / 60.0f;
    }
    return self;
}


- (void)drawView {
	[EAGLContext setCurrentContext:context];
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	
	// clear screen
	glViewport(0, 0, backingWidth, backingHeight);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	
    // ====== play start
//    const int w =
    const int w = [UIScreen mainScreen].bounds.size.width;
    const int h = [UIScreen mainScreen].bounds.size.height;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // ====== snippet be wrapped
    [__SWFPlayer update];
    
//    advance_gameswf(w, h, [&](){
//        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//        glDisableClientState(GL_COLOR_ARRAY);
//    },[&](){
//        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//        glEnableClientState(GL_COLOR_ARRAY);
//        
//        glEnable(GL_TEXTURE_2D);
//        
//        glEnableClientState(GL_VERTEX_ARRAY);
////        SetGLColor(1, 1, 1, 1);
//        glColorMask(1, 1, 1, 1);
//        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//    });
    
    
    // ====== play end
	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)layoutSubviews {
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    [self drawView];
}

- (BOOL)createFramebuffer {
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }
    
//		const GLint GL_STENCIL_INDEX8_OES = 0x8D48;
//		glGenRenderbuffersOES(1, &stencilRenderbuffer);
//		glBindRenderbufferOES(GL_RENDERBUFFER_OES, stencilRenderbuffer);
//		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_STENCIL_INDEX8_OES, backingWidth, backingHeight);
//		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_STENCIL_ATTACHMENT_OES, GL_RENDERBUFFER_OES, stencilRenderbuffer);
	
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
		{
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
  
//		glEnable(GL_MULTISAMPLE_ARB);
    glEnable(GL_ONE_MINUS_SRC_ALPHA);
    
    [__SWFPlayer setPreferredSize:CGSizeMake(backingWidth, backingHeight)];
	
    return YES;
}

- (void)destroyFramebuffer {
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}


- (void)startAnimation  {
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}

- (void)stopAnimation {
    self.animationTimer = nil;
}

- (void)setAnimationTimer:(NSTimer *)newTimer  {
    [animationTimer invalidate];
    animationTimer = newTimer;
}

- (void)setAnimationInterval:(NSTimeInterval)interval {
		animationInterval = interval;
    if (animationTimer)
		{
        [self stopAnimation];
        [self startAnimation];
    }
}


- (void)dealloc {
    [self stopAnimation];
    
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *t = [[touches allObjects] objectAtIndex:0];
	CGPoint touchPos = [t locationInView:t.view];
//	CGRect bounds = [self bounds];
	
	// This takes our point and makes it into a "percentage" of the screen
	//   That is 0.85 = 85%
//	CGPoint p = CGPointMake((touchPos.x - bounds.origin.x) / bounds.size.width,
	//												(touchPos.y - bounds.origin.y) / bounds.size.height);

    [__SWFPlayer onMouseDown:touchPos.x :touchPos.y];

}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event 
{
	
	UITouch *t = [[touches allObjects] objectAtIndex:0];
	CGPoint touchPos = [t locationInView:t.view];
//	CGRect bounds = [self bounds];
	
	// This takes our point and makes it into a "percentage" of the screen
	//   That is 0.85 = 85%
//	CGPoint p = CGPointMake((touchPos.x - bounds.origin.x) / bounds.size.width,
//													(touchPos.y - bounds.origin.y) / bounds.size.height);
    
    [__SWFPlayer onMouseMove:touchPos.x :touchPos.y];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *t = [[touches allObjects] objectAtIndex:0];
	CGPoint touchPos = [t locationInView:t.view];
	
    [__SWFPlayer onMouseUp:touchPos.x :touchPos.y];
}

@end
