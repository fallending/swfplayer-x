//
//  SwfPlayer.m
//  igameswf
//
//  Created by fallen.ink on 23/05/2017.
//
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "gameswf/gameswf.h"
#include <stdlib.h>
#include <stdio.h>
#include "base/utility.h"
#include "base/container.h"
#include "base/tu_file.h"
#include "base/tu_types.h"
#include "base/tu_timer.h"
#include "gameswf/gameswf_types.h"
#include "gameswf/gameswf_impl.h"
#include "gameswf/gameswf_root.h"
#include "gameswf/gameswf_freetype.h"
#include "gameswf/gameswf_player.h"
#include "gameswf/gameswf_as_classes/as_array.h"
#import "SwfPlayer.h"

SwfPlayer *__SWFPlayer = nil;

#pragma mark -

static void	message_log(const char* message)
// Process a log message.
{
    if (gameswf::get_verbose_parse())
    {
        fputs(message, stdout);
        fflush(stdout);
    }
}

static void	log_callback(bool error, const char* message)
// Error callback for handling gameswf messages.
{
    if (error)
    {
        // Log, and also print to stderr.
        message_log(message);
        fputs(message, stderr);
        fflush(stderr);
    }
    else
    {
        message_log(message);
    }
}

static tu_file*	file_opener(const char* url)
// Callback function.  This opens files for the gameswf library.
{
    assert(url);
    if (strncasecmp(url, "http://", 7) == 0)
    {
        const char* slash = strchr(url + 7, '/');
        if (slash == NULL)
        {
            return NULL;
        }
        
        char host[256];
        memset(host, 0, 256);
        strncpy(host, url + 7, slash - url - 7);
        
        int len = 0;
        const void* data = NULL;
        tu_string swfile;
        tu_string hisfile;
        
#ifdef WIN32
        swfile = "/test.swf";
        hisfile = "/his.txt";
        {
            tu_file fi(swfile.c_str(), "rb");
            len = fi.size();
            data = malloc(len);
            len = fi.read_bytes(data, len);
        }
#else
        NSString* swfFile = [NSString stringWithFormat: @"%@/Documents/my.swf", NSHomeDirectory()];
        swfile = [swfFile UTF8String];
        NSString* hisFile = [NSString stringWithFormat: @"%@/Documents/his.txt", NSHomeDirectory()];
        hisfile = [hisFile UTF8String];
        NSString* hostName = [NSString stringWithUTF8String: host];
        NSString* pathToFile= [NSString stringWithUTF8String: slash];
        NSURL* fileURL = [[NSURL alloc] initWithScheme:@"http" host:hostName path:pathToFile];
        if (fileURL)
        {
            NSData* fileContents = [NSData dataWithContentsOfURL:fileURL];
            if (fileContents)
            {
                len = [fileContents length];
                data = [fileContents bytes];
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
#endif
        
        // write swf
        {
            tu_file fi(swfile.c_str(), "wb");
            fi.write_bytes(data, len);
        }
        
        // update history
        
        // read history
        array<tu_string> his;
        {
            tu_file fi(hisfile.c_str(), "r");
            if (fi.get_error() == TU_FILE_NO_ERROR)
            {
                char buf[1024];
                while (fi.read_string(buf, 1024) > 1)	// !!!!
                {
                    his.push_back(buf);
                }
            }
        }
        
        // test for same
        bool is_new = true;
        for (int i = 0; i < his.size(); i++)
        {
            if (his[i] == url + 7)
            {
                is_new = false;
                break;
            }
        }
        
        // append new item
        if (is_new)
        {
            his.push_back(url + 7);
        }
        
        // truncate to 20
        while (his.size() > 20)
        {
            his.remove(0);
        }
        
        // save history
        {
            tu_file fi(hisfile.c_str(), "w");
            for (int i = 0; i < his.size(); i++)
            {
                fi.write_string(his[i].c_str());
                fi.write_string("\n");
            }
        }
        //	printf("%s as %s\n", url, swfile.c_str());
        return new tu_file(swfile.c_str(), "rb");
    }
    return new tu_file(url, "rb");
}

static void	fs_callback(gameswf::character* movie, const char* command, const char* args) {
// For handling notification callbacks from ActionScript.
    assert(movie);
    gameswf::gc_ptr<gameswf::player> player = movie->get_player();
    
    if (strcasecmp(command, "loadHistory") == 0) {
        tu_string hisfile;
#ifdef WIN32
        hisfile = "/his.txt";
#else
        NSString* hisFile = [NSString stringWithFormat: @"%@/Documents/his.txt", NSHomeDirectory()];
        hisfile = [hisFile UTF8String];
#endif
        
        // for debugging
        //remove(hisfile.c_str());
        
        array<tu_string> his;
        {
            tu_file fi(hisfile.c_str(), "r");
            if (fi.get_error() == TU_FILE_NO_ERROR)
            {
                char buf[1024];
                while (fi.read_string(buf, 1024) > 1)	// !!!!
                {
                    his.push_back(buf);
                }
            }
        }
        
        gameswf::as_array* as_his = new gameswf::as_array(player);
        for (int i = 0; i < his.size(); i++)
        {
            as_his->push(his[i].c_str());
        }
        player->get_root_movie()->set_member("his", as_his);
    }
}


#pragma mark -

@interface SwfPlayer () {
    CGSize _actualDisplaySize;
    CGSize _preferredSize;
    CGSize _videoSize;
    
    CGFloat _scale;
    
    CGPoint _idealPoint;
    
    Uint32	_startTicks;
    Uint32	_lastTicks;
    
    gameswf::gc_ptr<gameswf::player> player;
    gameswf::gc_ptr<gameswf::root>	m;
    
    array<mouse_event> s_mouse_event;
    
    gameswf::sound_handler*	sound;
}

@end

@implementation SwfPlayer

+ (void)load {
    if (!__SWFPlayer) {
        __SWFPlayer = [SwfPlayer new];
    } else {
        NSAssert(NO, @"no message");
    }
}

#pragma mark -

- (instancetype)initWithFilepath:(NSString *)path preferredSize:(CGSize)size {
    
    if (self = [super init]) {
        _scale = 1;
        _startTicks = 0;
        _lastTicks = 0;
        
        [self setFilePath:path error:nil];
        
        [self setPreferredSize:size];
    }
    
    return self;
    
}

- (void)update {
    Uint32	ticks = tu_timer::get_ticks();
    int	delta_ticks = ticks - _lastTicks;
    float	delta_t = delta_ticks / 1000.f;
    
    _lastTicks = ticks;
    
    m->set_display_viewport(_idealPoint.x, _idealPoint.y, _actualDisplaySize.width, _actualDisplaySize.height);
    
    // 设置swf播放区域的透明度
//    m->set_background_alpha(1.0f); // 透明度, opacity,opaque 不透明度
    // 设置swf播放去的背景色
    m->set_background_color(gameswf::rgba(255.f, 255.f, 255.f, 0.f));
    
    if (s_mouse_event.size() > 0) {
        m->notify_mouse_state(s_mouse_event[0].m_x, s_mouse_event[0].m_y, s_mouse_event[0].m_state);
        
        s_mouse_event.remove(0);
    }
    
    Uint32 t_advance = tu_timer::get_ticks();
    m->advance(delta_t); // 下一帧
    sound->advance(delta_t);
    t_advance = tu_timer::get_ticks() - t_advance;
    
    Uint32 t_display = tu_timer::get_ticks();
    m->display(); // 播放
    t_display = tu_timer::get_ticks() - t_display;
}

#pragma mark -

- (void)setFilePath:(NSString *)path error:(NSError **)error {
    gameswf::register_file_opener_callback(file_opener);
    gameswf::register_fscommand_callback(fs_callback);
    
    if (gameswf::get_verbose_parse()) {
        gameswf::register_log_callback(log_callback);
    }
    
    sound = gameswf::create_sound_handler_openal();
    gameswf::set_sound_handler(sound);
    
    gameswf::render_handler*	render = gameswf::create_render_handler_ogles();
    gameswf::set_render_handler(render);
    
    gameswf::set_glyph_provider(gameswf::create_glyph_provider_tu());
    
    player = new gameswf::player();
    m = player->load_file([path cStringUsingEncoding:NSUTF8StringEncoding]);
    if (m == NULL) {
        if (error) *error = [NSError errorWithDomain:@"swf-player" code:100 userInfo:@{}];
    }
    
    {   // 获取视频源信息
        
        // 视频本身的分辨率
        _videoSize = CGSizeMake(m->get_movie_width(), m->get_movie_height());
    }
    
    if (error) *error = nil;
}

- (void)setPreferredSize:(CGSize)size {
    
    // 期望的分辨率
    _preferredSize = size;
    
    float scale_x = (float) _preferredSize.width / _videoSize.width;
    float scale_y = (float) _preferredSize.height / _videoSize.height;
    
    _scale = fmin(scale_x, scale_y);
    
    // 实际分辨率大小
    _actualDisplaySize = CGSizeMake(m->get_movie_width() * _scale, m->get_movie_height() * _scale);
    
    // 理想位置
    _idealPoint = CGPointMake((int64_t)(_preferredSize.width - _actualDisplaySize.width) >> 1, (int64_t)(_preferredSize.height - _actualDisplaySize.height) >> 1);
    
    {
        _startTicks = tu_timer::get_ticks();
        _lastTicks = _startTicks;
    }
}

- (CGSize)getVideoSize {
    return _videoSize;
}

- (CGSize)getActualSize {
    return _actualDisplaySize;
}

#pragma mark - GameswfWrapperDelegate

- (void)onMouseDown:(double)x :(double)y {
    // mouse move event
    mouse_event me;
    me.m_x = (x - _idealPoint.x) / _scale;
    me.m_y = (y - _idealPoint.y) / _scale;
    me.m_state = 0;
    s_mouse_event.push_back(me);
    
    // mouse down event
    me.m_state = 1;
    s_mouse_event.push_back(me);
}

- (void)onMouseUp:(double)x :(double)y {
    // mouse up event
    mouse_event me;
    me.m_x = (x - _idealPoint.x) / _scale;
    me.m_y = (y - _idealPoint.y) / _scale;
    me.m_state = 0;
    s_mouse_event.push_back(me);
}

- (void)onMouseMove:(double)x :(double)y {
    // mouse move event
    mouse_event me;
    me.m_x = (x - _idealPoint.x) / _scale;
    me.m_y = (y - _idealPoint.y) / _scale;
    me.m_state = 1;
    s_mouse_event.push_back(me);
}


@end
