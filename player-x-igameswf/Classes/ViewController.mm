//
//  ViewController.m
//  igameswf
//
//  Created by fallen.ink on 16/05/2017.
//
//

#import "ViewController.h"
#import "EAGLView.h"
#import "mygameswf.h"

@interface ViewController () {
    BOOL _isPlaying;
}

@property (weak, nonatomic) IBOutlet EAGLView *eaglView;

@property (weak, nonatomic) IBOutlet UIButton *playButton;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    self.eaglView.animationInterval = 1.0 / 60.0;
    
    [self.eaglView startAnimation];
    
    _isPlaying = YES;
    [self.playButton addTarget:self action:@selector(onPlay) forControlEvents:UIControlEventTouchUpInside];
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
    if (_isPlaying) {
        
        
        _isPlaying = NO;
        [self.playButton setImage:[UIImage imageNamed:@"stop"] forState:UIControlStateNormal];
    } else {
        
        _isPlaying = NO;
        [self.playButton setImage:[UIImage imageNamed:@"play"] forState:UIControlStateNormal];
    }
}

@end
