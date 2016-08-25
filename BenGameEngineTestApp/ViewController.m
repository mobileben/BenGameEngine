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
#include "TransformComponent.h"
#include "LineRenderComponent.h"
#include "FlatRectRenderComponent.h"
#include "SpriteRenderComponent.h"

#include "TextureMaskComponent.h"
#include "InputTouchComponent.h"
#include "BoundingBoxComponent.h"
#include "AnimationChannelComponent.h"
#include <typeindex>

@interface ViewController ()<GLKViewDelegate>

@property (nonatomic, assign) std::shared_ptr<BGE::RenderContextOpenGLES2> renderContext;
@property (nonatomic, assign) std::shared_ptr<BGE::RenderWindow> renderWindow;
@property (nonatomic, assign) BGE::LineRenderComponent *buttonBounds;
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
        
        BGE::Game::getInstance()->provide(std::make_shared<BGE::TextureService>(self.renderContext->getContext()));
        
        NSString *path = [[NSBundle mainBundle] pathForResource:@"sample" ofType:@"png"];
        
#ifdef NOT_YET
        if (path) {
            typedef void (*func)(id, SEL, std::shared_ptr<BGE::Texture>, std::shared_ptr<BGE::Error>);
            func impl = (func)[self methodForSelector:@selector(racer:error:)];
            std::function<void(std::shared_ptr<BGE::Texture>, std::shared_ptr<BGE::Error> error)> fnc = std::bind(impl, self, @selector(racer:error:), std::placeholders::_1, std::placeholders::_2);
            BGE::Game::getInstance()->getTextureService()->createTextureFromFile("sample", [path UTF8String], fnc);
        }
        
        path = [[NSBundle mainBundle] pathForResource:@"item_powerup_fish" ofType:@"png"];
        
        if (path) {
            typedef void (*func)(id, SEL, std::shared_ptr<BGE::Texture>, std::shared_ptr<BGE::Error>);
            func impl = (func)[self methodForSelector:@selector(racer:error:)];
            std::function<void(std::shared_ptr<BGE::Texture>, std::shared_ptr<BGE::Error> error)> fnc = std::bind(impl, self, @selector(racer:error:), std::placeholders::_1, std::placeholders::_2);
            BGE::Game::getInstance()->getTextureService()->createTextureFromFile("fish", [path UTF8String], nullptr);
        }
#endif
        
        path = [[NSBundle mainBundle] pathForResource:@"Common-iPh6" ofType:@"json"];
        
        if (path) {
            BGE::Game::getInstance()->getScenePackageService()->packageFromJSONFile(self.spaceHandle, [path UTF8String], "Common", [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                
                if (package) {
                    package->link();
                }

                NSString *path = [[NSBundle mainBundle] pathForResource:@"CommonLobby-iPh6" ofType:@"json"];
                
                if (path) {
                    BGE::Game::getInstance()->getScenePackageService()->packageFromJSONFile(self.spaceHandle, [path UTF8String], "CommonLobby", [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                        BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                        
                        if (package) {
                            package->link();
                        }
                        
                        NSString *path = [[NSBundle mainBundle] pathForResource:@"CommonHUD-iPh6" ofType:@"json"];
                        
                        if (path) {
                            BGE::Game::getInstance()->getScenePackageService()->packageFromJSONFile(self.spaceHandle, [path UTF8String], "CommonHUD", [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                                BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                                
                                if (package) {
                                    package->link();
                                }
                                
                                NSString *path = [[NSBundle mainBundle] pathForResource:@"Lobby-iPh6" ofType:@"json"];
                                
                                if (path) {
                                    BGE::Game::getInstance()->getScenePackageService()->packageFromJSONFile(self.spaceHandle, [path UTF8String], "Lobby", [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
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
                                        
                                        // Now create auto display objects
                                        BGE::SceneObjectCreatedDelegate delegate;

                                        space->createAutoDisplayObjects(BGE::GameObjectHandle(), packageHandle, delegate);
                                        
                                        gameObj = space->createGameObject();
                                        transformComponent = space->createComponent<BGE::TransformComponent>();
                                        gameObj->addComponent(transformComponent);
                                        auto line = space->createComponent<BGE::LineRenderComponent>();
                                        BGE::Color color = { 1, 0, 1, 1 };
                                        auto material = BGE::Game::getInstance()->getMaterialService()->createMaterial(color);
                                        line->setMaterials({material});
                                        gameObj->addComponent(line);
                                        gameObj->setActive(true);
                                        
                                        self.buttonBounds = line;
                                        
                                        typedef void (*func)(id, SEL, BGE::SpaceHandle, BGE::GameObjectHandle, BGE::Event);
                                        func impl = (func)[self methodForSelector:@selector(handleInput:gameObjectHandle:event:)];
                                        std::function<void(BGE::SpaceHandle, BGE::GameObjectHandle, BGE::Event)> fnc = std::bind(impl, self, @selector(handleInput:gameObjectHandle:event:), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                                        BGE::Game::getInstance()->getInputService()->registerEventHandler("settings_button", BGE::Event::TouchUpInside, fnc);
                                    });
                                }
                            });
                        }
                    });
                }
                
            });
            
#ifdef NOT_YET
            auto tSpaceHandle = BGE::Game::getInstance()->getSpaceService()->createSpace("TestSpace");
            auto tSpace = BGE::Game::getInstance()->getSpaceService()->getSpace(tSpaceHandle);
            
            tSpace->setVisible(false);
            
            // Create test objects
            std::vector<BGE::GameObject *> tObjs;
            auto numGameObjs = 100;
            for (auto i=0;i<numGameObjs;i++) {
                auto gObj = tSpace->createGameObject();
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
            
            uint32_t v1, v2, v3;
            std::type_index t1 = typeid(BGE::TextureMaskComponent);
            std::type_index t2 = typeid(BGE::InputTouchComponent);
            std::type_index t3 = typeid(BGE::BoundingBoxComponent);
            
            loops = 10000;
            
            for (auto i=0;i<loops;i++) {
                v1 = BGE::ComponentBitmask::bitmaskForTypeIndex(t1);
                v2 = BGE::ComponentBitmask::bitmaskForTypeIndex(t2);
                v3 = BGE::ComponentBitmask::bitmaskForTypeIndex(t3);
            }

            endTime = [[NSDate date] timeIntervalSince1970];

            componentTime = endTime - startTime;
            
            NSLog(@"Time interval getComponent %f", endTime - startTime);
            
            NSLog(@"Diff between component and bitmask = %f (%f)", componentTime - bitmaskTime, componentTime/bitmaskTime);
            // Test access by bitmask
            startTime = [[NSDate date] timeIntervalSince1970];
            
            for (auto i=0;i<loops;i++) {
                v1 = BGE::ComponentBitmask::getBitmask<BGE::TextureMaskComponent>();
                v2 = BGE::ComponentBitmask::getBitmask<BGE::InputTouchComponent>();
                v3 = BGE::ComponentBitmask::getBitmask<BGE::BoundingBoxComponent>();
            }
            
            endTime = [[NSDate date] timeIntervalSince1970];
            
            NSLog(@"Time interval bitmaskForTypeIndex %f", endTime - startTime);
            
            startTime = [[NSDate date] timeIntervalSince1970];
            
            for (auto i=0;i<loops;i++) {
            }
            
            endTime = [[NSDate date] timeIntervalSince1970];
            
            componentTime = endTime - startTime;
            
            NSLog(@"Time interval getComponent %f", endTime - startTime);
            
            NSLog(@"Diff between component and bitmask = %f (%f)", componentTime - bitmaskTime, componentTime/bitmaskTime);
#endif
        }
    }
    
    [self.glView display];
}

- (void)handleInput:(BGE::SpaceHandle)spaceHandle gameObjectHandle:(BGE::GameObjectHandle)gameObjectHandle event:(BGE::Event)event {
    
}

- (void)racer:(std::shared_ptr<BGE::Texture>)texture error:(std::shared_ptr<BGE::Error>)error {
    std::shared_ptr<BGE::RenderServiceOpenGLES2> renderer = std::dynamic_pointer_cast<BGE::RenderServiceOpenGLES2>(BGE::Game::getInstance()->getRenderService());
    renderer->setGLKTextureInfo(texture->getTextureInfo());
    BGE::Game::getInstance()->getRenderService()->render();
    
#if 0
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
    
    auto gameObj = space->getGameObject("settings_button");
    
    if (gameObj) {
        auto button = gameObj->getComponent<BGE::ButtonComponent>();
        
        if (button) {
            auto bbox = button->getBoundingBox();
            if (bbox) {
                auto xform = button->getTransform();
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

                point.x = bbox->aabbMinX;
                point.y = bbox->aabbMaxY;
                points.push_back(point);
                
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
