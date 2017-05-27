//
//  ViewController.m
//  igameswf
//
//  Created by fallen.ink on 16/05/2017.
//
//

#import "ViewController.h"
#import "EAGLView.h"
#import "SWFPlayer.h"

@interface ViewController () {
    BOOL _isPlaying;
}

@property (weak, nonatomic) IBOutlet EAGLView *eaglView;

@property (weak, nonatomic) IBOutlet UIButton *playButton;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    {
        self.title = @"SWF播放测试";
    }
    
    {
        NSError *error = nil;
        NSString *path = [[NSBundle mainBundle] pathForResource:@"app_mashaladi" ofType:@"swf"];
        
        [__SWFPlayer setFilePath:path error:&error];
        if (!error) {
            self.eaglView.animationInterval = 1.0 / 60.0;
            
            [self.eaglView startAnimation];
            
            _isPlaying = YES;
            [self.playButton addTarget:self action:@selector(onPlay) forControlEvents:UIControlEventTouchUpInside];
        }
    }
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark -

- (void)onPlay {
//    if (_isPlaying) {
//        gameswf_stop();
//        
//        _isPlaying = NO;
//        [self.playButton setImage:[UIImage imageNamed:@"stop"] forState:UIControlStateNormal];
//    } else {
//        
//        gameswf_play();
//        
//        _isPlaying = YES;
//        [self.playButton setImage:[UIImage imageNamed:@"play"] forState:UIControlStateNormal];
//    }
}

@end
