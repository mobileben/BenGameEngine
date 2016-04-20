//
//  ViewController.m
//  BenGameEngineTestApp
//
//  Created by Benjamin Lee on 3/3/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#import "ViewController.h"
#include "BenGameEngineTestApp.hpp"
#include "RenderWindow.h"
#include "RenderContextOpenGLES2.h"
#include "RenderServiceOpenGLES2.h"
#include "BGETextureServiceOpenGLES2.h"
#include "BGEFontServiceOpenGLES2.h"
#include "BGETextureOpenGLES2.h"
#include "BGEGLKView.h"
#include "TransformComponent.h"
#include "LineRenderComponent.h"
#include "FlatRectRenderComponent.h"
#include "SpriteRenderComponent.h"

@interface ViewController ()<GLKViewDelegate>

@property (nonatomic, assign) std::shared_ptr<BGE::RenderContextOpenGLES2> renderContext;
@property (nonatomic, assign) std::shared_ptr<BGE::RenderWindow> renderWindow;
@property (nonatomic, weak) GLKView *glView;
@property (nonatomic, assign) BOOL once;

@end

@implementation ViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    //    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    self.renderContext = std::make_shared<BGE::RenderContextOpenGLES2>();
    
    self.glView = (GLKView *) self.view;
    //    self.glView.context = self.renderContext->getContext();
    self.glView.delegate = self;
    
    self.renderWindow = std::make_shared<BGE::RenderWindow>();
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
            typedef void (*func)(id, SEL, std::shared_ptr<BGETextureBase>, std::shared_ptr<BGE::Error>);
            func impl = (func)[self methodForSelector:@selector(racer:error:)];
            std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGE::Error> error)> fnc = std::bind(impl, self, @selector(racer:error:), std::placeholders::_1, std::placeholders::_2);
            BGEGame::getInstance()->getTextureService()->namedTextureFromFile("sample", [path UTF8String], fnc);
        }
        
        path = [[NSBundle mainBundle] pathForResource:@"item_powerup_fish" ofType:@"png"];
        
        if (path) {
            typedef void (*func)(id, SEL, std::shared_ptr<BGETextureBase>, std::shared_ptr<BGE::Error>);
            func impl = (func)[self methodForSelector:@selector(racer:error:)];
            std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGE::Error> error)> fnc = std::bind(impl, self, @selector(racer:error:), std::placeholders::_1, std::placeholders::_2);
            BGEGame::getInstance()->getTextureService()->namedTextureFromFile("fish", [path UTF8String], nullptr);
        }
    }
    
    [self.glView display];
}

- (void)racer:(std::shared_ptr<BGETextureBase>)texture error:(std::shared_ptr<BGE::Error>)error {
    std::shared_ptr<BGE::RenderServiceOpenGLES2> renderer = std::dynamic_pointer_cast<BGE::RenderServiceOpenGLES2>(BGEGame::getInstance()->getRenderService());
    std::shared_ptr<BGETextureOpenGLES2> glTex = std::dynamic_pointer_cast<BGETextureOpenGLES2>(texture);
    renderer->setGLKTextureInfo(glTex->getTextureInfo());
    BGEGame::getInstance()->getRenderService()->render();
    
    // Let's create and add a game object
    std::string name = "hello";
    auto gameObj0 = BGEGame::getInstance()->getGameObjectService()->createObject<BGE::GameObject>();
#if 1
    auto gameObj1 = BGEGame::getInstance()->getGameObjectService()->createObject<BGE::GameObject>();
#endif
    auto gameObj2 = BGEGame::getInstance()->getGameObjectService()->createObject<BGE::GameObject>();
    
    auto material = BGEGame::getInstance()->getMaterialService()->createMaterial("mat", texture);
    auto transformComponent0 = BGEGame::getInstance()->getComponentService()->createComponent<BGE::TransformComponent>();
    auto transformComponent1 = BGEGame::getInstance()->getComponentService()->createComponent<BGE::TransformComponent>();
    auto transformComponent2 = BGEGame::getInstance()->getComponentService()->createComponent<BGE::TransformComponent>();
    auto lineRenderer = BGEGame::getInstance()->getComponentService()->createComponent<BGE::LineRenderComponent>("line");
    auto flatRect = BGEGame::getInstance()->getComponentService()->createComponent<BGE::FlatRectRenderComponent>("rect");
    auto sprite = BGEGame::getInstance()->getComponentService()->createComponent<BGE::SpriteRenderComponent>("sprite");
    
    BGEColor color = { 1, 0, 0, 1 };
    
    material->setColor(color);
    gameObj0->setName("Object0");
    gameObj0->addComponent(transformComponent0);
    gameObj0->addComponent(lineRenderer);

    lineRenderer->setMaterials({ material });
    lineRenderer->setPoints({ { 500, 200 }, { 700, 100 }, { 500, 800 } });
    
//    NSLog(@"game object name %s", gameObj->getName().c_str());
#if 1
    gameObj1->setName("Object1");
    gameObj1->addComponent(transformComponent1);
    gameObj1->addComponent(flatRect);
#endif
    BGEVector2 wh = { 500, 700 };
    
    flatRect->setMaterials({ material });
    flatRect->setWidthHeight(wh);
    
    gameObj2->setName("Object2");
    gameObj2->addComponent(transformComponent2);
    gameObj2->addComponent(sprite);
    
    sprite->setMaterials({material});
    
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
