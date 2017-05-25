//
//  RootViewController.m
//  igameswf
//
//  Created by fallen.ink on 25/05/2017.
//
//

#import "RootViewController.h"
#import "ViewController.h"
#import "GPURenderViewController.h"

@interface RootViewController ()

@end

@implementation RootViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)onSWFPlayTest:(id)sender {
    [self.navigationController pushViewController:[ViewController new] animated:YES];
}

- (IBAction)onSWFRenderTest:(id)sender {
    [self.navigationController pushViewController:[RootViewController new] animated:YES];
}

@end
