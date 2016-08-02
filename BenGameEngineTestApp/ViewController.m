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
@property (nonatomic, assign) std::shared_ptr<BGE::LineRenderComponent> buttonBounds;
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
                        transformComponent->setRotation(0);
                        
                        gameObj->setName("Object4");
                        gameObj->setActive(true);

                        // Spaces are not visible by default
                        space->setVisible(true);
                        
                        gameObj = space->createObject<BGE::GameObject>();
                        transformComponent = space->createComponent<BGE::TransformComponent>();
                        gameObj->addComponent(transformComponent);
                        auto line = space->createComponent<BGE::LineRenderComponent>();
                        BGE::Color color = { 1, 0, 1, 1 };
                        auto material = BGE::Game::getInstance()->getMaterialService()->createMaterial(color);
                        line->setMaterials({material});
                        gameObj->addComponent(line);
                        gameObj->setActive(true);
                        
                        self.buttonBounds = line;
                        
                        typedef void (*func)(id, SEL, std::shared_ptr<BGE::GameObject>, BGE::Event);
                        func impl = (func)[self methodForSelector:@selector(handleInput:event:)];
                        std::function<void(std::shared_ptr<BGE::GameObject>, BGE::Event)> fnc = std::bind(impl, self, @selector(handleInput:event:), std::placeholders::_1, std::placeholders::_2);
                        BGE::Game::getInstance()->getInputService()->registerEventHandler("buy_button", BGE::Event::TouchUpInside, fnc);
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

- (void)handleInput:(std::shared_ptr<BGE::GameObject>)gameObj event:(BGE::Event)event {
    
}

- (void)racer:(std::shared_ptr<BGE::TextureBase>)texture error:(std::shared_ptr<BGE::Error>)error {
    std::shared_ptr<BGE::RenderServiceOpenGLES2> renderer = std::dynamic_pointer_cast<BGE::RenderServiceOpenGLES2>(BGE::Game::getInstance()->getRenderService());
    std::shared_ptr<BGE::TextureOpenGLES2> glTex = std::dynamic_pointer_cast<BGE::TextureOpenGLES2>(texture);
    renderer->setGLKTextureInfo(glTex->getTextureInfo());
    BGE::Game::getInstance()->getRenderService()->render();
    
#if 1
    // Let's create and add a game object
    std::string name = "hello";
    auto space = BGE::Game::getInstance()->getSpaceService()->getSpace(self.spaceHandle);
    auto gameObj0 = space->createObject<BGE::GameObject>();
#if 1
    auto gameObj1 = space->createObject<BGE::GameObject>();
#endif
    auto gameObj2 = space->createObject<BGE::GameObject>();
    
    auto materialHandle = BGE::Game::getInstance()->getMaterialService()->createMaterial(texture);
    auto material = BGE::Game::getInstance()->getMaterialService()->getMaterial(materialHandle);
    auto transformComponent0 = space->createComponent<BGE::TransformComponent>();
    auto transformComponent1 = space->createComponent<BGE::TransformComponent>();
    auto transformComponent2 = space->createComponent<BGE::TransformComponent>();
    auto lineRenderer = space->createComponent<BGE::LineRenderComponent>();
    auto flatRect = space->createComponent<BGE::FlatRectRenderComponent>();
    auto sprite = space->createComponent<BGE::SpriteRenderComponent>();
    
    BGE::Color color = { 1, 0, 0, 1 };
    
    material->setColor(color);
    gameObj0->setName("Object0");
    gameObj0->addComponent(transformComponent0);
    gameObj0->addComponent(lineRenderer);
    gameObj0->setActive(true);

    lineRenderer->setMaterials({ materialHandle });
    lineRenderer->setPoints({ { 500, 200 }, { 700, 100 }, { 500, 800 } }, true);
    
//    NSLog(@"game object name %s", gameObj->getName().c_str());
#if 1
    gameObj1->setName("Object1");
    gameObj1->addComponent(transformComponent1);
    gameObj1->addComponent(flatRect);
    gameObj1->setActive(true);
#endif
    BGE::Vector2 wh = { 500, 700 };
    
    flatRect->setMaterials({ materialHandle });
    flatRect->setWidthHeight(wh);
    
    gameObj2->setName("Object2");
    gameObj2->addComponent(transformComponent2);
    gameObj2->addComponent(sprite);
    gameObj2->setActive(true);
    
    sprite->setMaterials({materialHandle});
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
    auto space = BGE::Game::getInstance()->getSpaceService()->getSpace(self.spaceHandle);
    
    auto gameObj = space->find("buy_button");
    
    if (gameObj) {
        auto button = gameObj->getComponent<BGE::ButtonComponent>();
        
        if (button) {
            auto bbox = button->getBoundingBox();
            if (bbox) {
                auto xform = button->getTransform();
                auto parent = xform->getParent().lock();
                BGE::Matrix4 matrix;
                BGE::Vector2 point;
                std::vector<BGE::Vector2> points;
                
                xform->getMatrix(matrix);
                bbox->computeAABB(matrix);
                
                point.x = bbox->aabbMinX;
                point.y = bbox->aabbMinY;
                points.push_back(point);
                
                point.x = bbox->aabbMaxX;
                point.y = bbox->aabbMinY;
                points.push_back(point);

                point.x = bbox->aabbMaxX;
                point.y = bbox->aabbMaxY;
                points.push_back(point);
#if 1
                point.x = bbox->aabbMinX;
                point.y = bbox->aabbMaxY;
                points.push_back(point);
#endif
                
                self.buttonBounds->setPoints(points);
                
                NSLog(@"GOT YOU BITCHY");
            }
        }
    }
    BGE::Game::getInstance()->getRenderService()->render();
}

void handled()
{
    
}
@end
