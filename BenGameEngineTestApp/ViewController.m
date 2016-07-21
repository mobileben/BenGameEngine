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
#include "TextureServiceOpenGLES2.h"
#include "TextureOpenGLES2.h"
#include "TransformComponent.h"
#include "LineRenderComponent.h"
#include "FlatRectRenderComponent.h"
#include "SpriteRenderComponent.h"

@interface ViewController ()<GLKViewDelegate>

@property (nonatomic, assign) std::shared_ptr<BGE::RenderContextOpenGLES2> renderContext;
@property (nonatomic, assign) std::shared_ptr<BGE::RenderWindow> renderWindow;
@property (nonatomic, weak) BGEView *glView;
@property (nonatomic, assign) BOOL once;
@property (nonatomic, assign) BGE::SpaceHandle spaceHandle;
@end

@implementation ViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    //    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    self.renderContext = std::make_shared<BGE::RenderContextOpenGLES2>();
    
    self.glView = (BGEView *) self.view;
    //    self.glView.context = self.renderContext->getContext();
    self.glView.delegate = self;
    
    self.renderWindow = std::make_shared<BGE::RenderWindow>();
    self.renderWindow->setView((BGEView *) self.view);
    
    BGE::Game::getInstance()->getRenderService()->bindRenderWindow(self.renderContext, self.renderWindow);
    self.spaceHandle = BGE::Game::getInstance()->getSpaceService()->createSpace("default");
    
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
        BGE::Game::getInstance()->getRenderService()->createShaders();
        BGE::Game::getInstance()->getRenderService()->setIsReady();
        self.once = YES;
        
        BGE::Game::getInstance()->provide(std::make_shared<BGE::TextureServiceOpenGLES2>(self.renderContext->getContext()));
        BGE::Game::getInstance()->getFontService()->loadFont("default", 32, nullptr);
        
        NSString *path = [[NSBundle mainBundle] pathForResource:@"sample" ofType:@"png"];
        
        if (path) {
            typedef void (*func)(id, SEL, std::shared_ptr<BGE::TextureBase>, std::shared_ptr<BGE::Error>);
            func impl = (func)[self methodForSelector:@selector(racer:error:)];
            std::function<void(std::shared_ptr<BGE::TextureBase>, std::shared_ptr<BGE::Error> error)> fnc = std::bind(impl, self, @selector(racer:error:), std::placeholders::_1, std::placeholders::_2);
            BGE::Game::getInstance()->getTextureService()->namedTextureFromFile("sample", [path UTF8String], fnc);
        }
        
        path = [[NSBundle mainBundle] pathForResource:@"item_powerup_fish" ofType:@"png"];
        
        if (path) {
            typedef void (*func)(id, SEL, std::shared_ptr<BGE::TextureBase>, std::shared_ptr<BGE::Error>);
            func impl = (func)[self methodForSelector:@selector(racer:error:)];
            std::function<void(std::shared_ptr<BGE::TextureBase>, std::shared_ptr<BGE::Error> error)> fnc = std::bind(impl, self, @selector(racer:error:), std::placeholders::_1, std::placeholders::_2);
            BGE::Game::getInstance()->getTextureService()->namedTextureFromFile("fish", [path UTF8String], nullptr);
        }
        
        path = [[NSBundle mainBundle] pathForResource:@"Common-iPh6" ofType:@"json"];
        
        if (path) {
            BGE::Game::getInstance()->getScenePackageService()->packageFromJSONFile([path UTF8String], "common", [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                
                if (package) {
                    package->link();
                }
                
                NSString *path = [[NSBundle mainBundle] pathForResource:@"Lobby-iPh6" ofType:@"json"];
                
                if (path) {
                    BGE::Game::getInstance()->getScenePackageService()->packageFromJSONFile([path UTF8String], "lobby", [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                        BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                        
                        if (package) {
                            package->link();
                        }
                        auto space = BGE::Game::getInstance()->getSpaceService()->getSpace(self.spaceHandle);
                        
                        auto gameObj = space->createSprite("SaleBkg");
                        auto transformComponent = gameObj->getComponent<BGE::TransformComponent>();
                        
                        BGE::Vector2 pos = { 700, 700 };
                        BGE::Vector2 scale = { 2, 1 };
                        transformComponent->setPosition(pos);
                        transformComponent->setScale(scale);
                        transformComponent->setRotation(M_PI/16);
                        
                        gameObj->setName("Object1");
                        
                        gameObj->setActive(true);
                        
                        gameObj = space->createText("CashText");
                        transformComponent = gameObj->getComponent<BGE::TransformComponent>();

                        transformComponent->setX(200);
                        transformComponent->setY(1200);
                        gameObj->setName("Object2");
                        gameObj->setActive(true);
                        
                        gameObj = space->createAnimSequence("SaleGlowAnim");
                        transformComponent = gameObj->getComponent<BGE::TransformComponent>();
                        auto animator = gameObj->getComponent<BGE::AnimatorComponent>();

                        transformComponent->setX(200);
                        transformComponent->setY(1000);
                        
                        gameObj->setName("Object3");
                        animator->reset();
                        
                        animator->play(BGE::AnimatorComponent::AnimPlayForever, true, 1 );
                        gameObj->setActive(true);
                        
                        // Spaces are not visible by default
                        space->setVisible(true);

                        gameObj = space->createAnimSequence("BuyButtonObject");
                        transformComponent = gameObj->getComponent<BGE::TransformComponent>();
                        animator = gameObj->getComponent<BGE::AnimatorComponent>();
                        animator->reset();
                        
                        transformComponent->setX(600);
                        transformComponent->setY(1000);
                        
                        gameObj->setName("Object4");
                        gameObj->setActive(true);

                        // Spaces are not visible by default
                        space->setVisible(true);

                    });
                }
            });
            
            auto tSpaceHandle = BGE::Game::getInstance()->getSpaceService()->createSpace("TestSpace");
            auto tSpace = BGE::Game::getInstance()->getSpaceService()->getSpace(tSpaceHandle);
            
            tSpace->setVisible(false);
            
            // Create test objects
            std::vector<std::shared_ptr<BGE::GameObject>> tObjs;
            auto numGameObjs = 1000;
            for (auto i=0;i<numGameObjs;i++) {
                auto gObj = tSpace->createObject<BGE::GameObject>();
                auto xform = tSpace->createComponent<BGE::TransformComponent>();
                auto sprite = tSpace->createComponent<BGE::SpriteRenderComponent>();
                
                gObj->addComponent<BGE::TransformComponent>(xform);
                gObj->addComponent<BGE::SpriteRenderComponent>(sprite);
                gObj->setActive(true);
                tObjs.push_back(gObj);
            }
            
            auto loops = 500;
            
            // Test access by getComponent
            NSTimeInterval componentTime;
            NSTimeInterval startTime, endTime;
            
            
            // Test access by bitmask
            startTime = [[NSDate date] timeIntervalSince1970];
            
            for (auto i=0;i<loops;i++) {
                for (auto gObj : tObjs) {
                    auto count = 0;
                    
#if 1
                    if (gObj->getComponentBitmask() & BGE::Component::getBitmask<BGE::TransformComponent>()) {
                        count++;
                    }
                    if (gObj->getComponentBitmask() & BGE::Component::getBitmask<BGE::SpriteRenderComponent>()) {
                        count++;
                    }
                    assert(count == 2);
#endif
                }
            }
            
            endTime = [[NSDate date] timeIntervalSince1970];
            
            NSTimeInterval bitmaskTime = endTime - startTime;
            
            NSLog(@"Time interval bitmask %f", endTime - startTime);

            startTime = [[NSDate date] timeIntervalSince1970];
            
            for (auto i=0;i<loops;i++) {
                for (auto gObj : tObjs) {
                    auto count = 0;
                    
#if 1
                    if (gObj->getComponent<BGE::TransformComponent>()) {
                        count++;
                    }
                    if (gObj->getComponent<BGE::SpriteRenderComponent>()) {
                        count++;
                    }
                    assert(count == 2);
#endif
                }
            }

            endTime = [[NSDate date] timeIntervalSince1970];

            componentTime = endTime - startTime;
            
            NSLog(@"Time interval getComponent %f", endTime - startTime);
            
            NSLog(@"Diff between component and bitmask = %f (%f)", componentTime - bitmaskTime, componentTime/bitmaskTime);
        }
    }
    
    [self.glView display];
}

- (void)racer:(std::shared_ptr<BGE::TextureBase>)texture error:(std::shared_ptr<BGE::Error>)error {
    std::shared_ptr<BGE::RenderServiceOpenGLES2> renderer = std::dynamic_pointer_cast<BGE::RenderServiceOpenGLES2>(BGE::Game::getInstance()->getRenderService());
    std::shared_ptr<BGE::TextureOpenGLES2> glTex = std::dynamic_pointer_cast<BGE::TextureOpenGLES2>(texture);
    renderer->setGLKTextureInfo(glTex->getTextureInfo());
    BGE::Game::getInstance()->getRenderService()->render();
    
#if 0
    // Let's create and add a game object
    std::string name = "hello";
    auto gameObj0 = BGE::Game::getInstance()->getGameObjectService()->createObject<BGE::GameObject>();
#if 1
    auto gameObj1 = BGE::Game::getInstance()->getGameObjectService()->createObject<BGE::GameObject>();
#endif
    auto gameObj2 = BGE::Game::getInstance()->getGameObjectService()->createObject<BGE::GameObject>();
    
    auto material = BGE::Game::getInstance()->getMaterialService()->createMaterial(texture);
    auto transformComponent0 = BGE::Game::getInstance()->getComponentService()->createComponent<BGE::TransformComponent>();
    auto transformComponent1 = BGE::Game::getInstance()->getComponentService()->createComponent<BGE::TransformComponent>();
    auto transformComponent2 = BGE::Game::getInstance()->getComponentService()->createComponent<BGE::TransformComponent>();
    auto lineRenderer = BGE::Game::getInstance()->getComponentService()->createComponent<BGE::LineRenderComponent>();
    auto flatRect = BGE::Game::getInstance()->getComponentService()->createComponent<BGE::FlatRectRenderComponent>();
    auto sprite = BGE::Game::getInstance()->getComponentService()->createComponent<BGE::SpriteRenderComponent>();
    
    BGE::Color color = { 1, 0, 0, 1 };
    
    material->setColor(color);
    gameObj0->setName("Object0");
    gameObj0->addComponent(transformComponent0);
    gameObj0->addComponent(lineRenderer);
    gameObj0->setActive(true);

    lineRenderer->setMaterials({ material });
    lineRenderer->setPoints({ { 500, 200 }, { 700, 100 }, { 500, 800 } });
    
//    NSLog(@"game object name %s", gameObj->getName().c_str());
#if 1
    gameObj1->setName("Object1");
    gameObj1->addComponent(transformComponent1);
    gameObj1->addComponent(flatRect);
    gameObj1->setActive(true);
#endif
    BGE::Vector2 wh = { 500, 700 };
    
    flatRect->setMaterials({ material });
    flatRect->setWidthHeight(wh);
    
    gameObj2->setName("Object2");
    gameObj2->addComponent(transformComponent2);
    gameObj2->addComponent(sprite);
    gameObj2->setActive(true);
    
    sprite->setMaterials({material});
#endif
    
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
    BGE::Game::getInstance()->getRenderService()->render();
}

void handled()
{
    
}
@end
