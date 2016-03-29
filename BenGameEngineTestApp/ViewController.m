//
//  ViewController.m
//  BenGameEngineTestApp
//
//  Created by Benjamin Lee on 3/3/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#import "ViewController.h"
#include "BenGameEngineTestApp.hpp"
#include "BGERenderWindow.h"
#include "BGERenderContextOpenGLES2.h"
#include "BGERenderServiceOpenGLES2.h"
#include "BGETextureServiceOpenGLES2.h"
#include "BGEFontServiceOpenGLES2.h"
#include "BGETextureOpenGLES2.h"
#include "BGEGLKView.h"

@interface ViewController ()<GLKViewDelegate>

@property (nonatomic, assign) std::shared_ptr<BGERenderContextOpenGLES2> renderContext;
@property (nonatomic, assign) std::shared_ptr<BGERenderWindow> renderWindow;
@property (nonatomic, weak) GLKView *glView;
@property (nonatomic, assign) BOOL once;

@end

@implementation ViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    //    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    self.renderContext = std::make_shared<BGERenderContextOpenGLES2>();
    
    self.glView = (GLKView *) self.view;
    //    self.glView.context = self.renderContext->getContext();
    self.glView.delegate = self;
    
    self.renderWindow = std::make_shared<BGERenderWindow>();
    self.renderWindow->setView((GLKView *) self.view);
    
    BGEGame::getInstance()->getRenderService()->bindRenderWindow(self.renderContext, self.renderWindow);
    
    NSLog(@"DIDLOAD view is %f %f %f %f", self.view.frame.origin.x, self.view.frame.origin.y, self.view.frame.size.width, self.view.frame.size.height);
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    if (!self.once) {
        BGEGame::getInstance()->getRenderService()->createShaders();
        BGEGame::getInstance()->getRenderService()->setIsReady();
        self.once = YES;
        
        BGEGame::getInstance()->provide(std::make_shared<BGETextureServiceOpenGLES2>(self.renderContext->getContext()));
        BGEGame::getInstance()->provide(std::make_shared<BGEFontServiceOpenGLES2>());
        BGEGame::getInstance()->getFontService()->loadFont("default", 32, nullptr);
        
        NSString *path = [[NSBundle mainBundle] pathForResource:@"sample" ofType:@"png"];
        
        if (path) {
            typedef void (*func)(id, SEL, std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError>);
            func impl = (func)[self methodForSelector:@selector(racer:error:)];
            std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError> error)> fnc = std::bind(impl, self, @selector(racer:error:), std::placeholders::_1, std::placeholders::_2);
            BGEGame::getInstance()->getTextureService()->namedTextureFromFile("sample", [path UTF8String], fnc);
        }
        
        path = [[NSBundle mainBundle] pathForResource:@"item_powerup_fish" ofType:@"png"];
        
        if (path) {
            typedef void (*func)(id, SEL, std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError>);
            func impl = (func)[self methodForSelector:@selector(racer:error:)];
            std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError> error)> fnc = std::bind(impl, self, @selector(racer:error:), std::placeholders::_1, std::placeholders::_2);
            BGEGame::getInstance()->getTextureService()->namedTextureFromFile("fish", [path UTF8String], fnc);
        }
    }
    
    [self.glView display];
}

- (void)racer:(std::shared_ptr<BGETextureBase>)texture error:(std::shared_ptr<BGEError>)error {
    std::shared_ptr<BGERenderServiceOpenGLES2> renderer = std::dynamic_pointer_cast<BGERenderServiceOpenGLES2>(BGEGame::getInstance()->getRenderService());
    std::shared_ptr<BGETextureOpenGLES2> glTex = std::dynamic_pointer_cast<BGETextureOpenGLES2>(texture);
    renderer->setGLKTextureInfo(glTex->getTextureInfo());
    BGEGame::getInstance()->getRenderService()->render();
}

- (void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
    
}

- (void)dealloc
{
    self.renderWindow.reset();
}

#pragma mark - GLKViewDelegate

- (void) glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    BGEGame::getInstance()->getRenderService()->render();
}

void handled()
{
    
}
@end
