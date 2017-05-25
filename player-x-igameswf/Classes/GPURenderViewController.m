//
//  GPURenderViewController.m
//  igameswf
//
//  Created by fallen.ink on 25/05/2017.
//
//

#import "GPURenderViewController.h"
#import "GPUImageView.h"
#import "GPUSwfRender.h"

@interface GPURenderViewController ()

@property (nonatomic, strong) GPUImageView *filterView;
@property (nonatomic, strong) GPUSwfRender *outputFilter;

@end

@implementation GPURenderViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    {
        self.title = @"SWF渲染测试";
    }
    
    {
        self.filterView = [[GPUImageView alloc] initWithFrame:self.view.frame];
        self.filterView.center = self.view.center;
        [self.view addSubview:self.filterView];
        
        NSString *path = [[NSBundle mainBundle] pathForResource:@"app_mashaladi" ofType:@"swf"];
        
        self.outputFilter = [[GPUSwfRender alloc] initWithSwfPath:path];
        
        [self.outputFilter addTarget:self.filterView];
        [self.outputFilter startProcessing];
        
//        self.videoCamera.outputImageOrientation = UIInterfaceOrientationPortrait;
//        self.videoCamera.horizontallyMirrorFrontFacingCamera = YES;
    }
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
