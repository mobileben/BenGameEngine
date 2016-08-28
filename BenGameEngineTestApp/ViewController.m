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
#include "ButtonComponent.h"

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
        
        BGE::Game::getInstance()->outputResourceBreakdown();

        NSString *path;
        
        path = [[NSBundle mainBundle] pathForResource:@"Common-iPh6" ofType:@"json"];
        
        if (path) {
            BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "Common", [path UTF8String], [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                
                if (package) {
                    package->link();
                }
                
                BGE::Game::getInstance()->outputResourceBreakdown();
            });
        }
        
        path = [[NSBundle mainBundle] pathForResource:@"CommonLobby-iPh6" ofType:@"json"];
        
        if (path) {
            BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "CommonLobby", [path UTF8String], [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                
                if (package) {
                    package->link();
                }
                
                BGE::Game::getInstance()->outputResourceBreakdown();
            });
        }
        
        path = [[NSBundle mainBundle] pathForResource:@"CommonHUD-iPh6" ofType:@"json"];
        
        if (path) {
            BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "CommonHUD", [path UTF8String], [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                
                if (package) {
                    package->link();
                }
                
                BGE::Game::getInstance()->outputResourceBreakdown();
            });
        }
        
        path = [[NSBundle mainBundle] pathForResource:@"Lobby-iPh6" ofType:@"json"];
        
        if (path) {
            BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "Lobby", [path UTF8String], [self](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                
                if (package) {
                    package->link();
                }
                
                auto space = BGE::Game::getInstance()->getSpaceService()->getSpace(self.spaceHandle);
                
                // Now create auto display objects
                BGE::SceneObjectCreatedDelegate delegate;
                
                space->createAutoDisplayObjects(BGE::GameObjectHandle(), packageHandle, delegate);
                
                // Spaces are not visible by default
                space->setVisible(true);
                BGE::Game::getInstance()->outputResourceBreakdown();
            });
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
#ifdef NOT_YET
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
#endif
    
    BGE::Game::getInstance()->getRenderService()->render();
}

void handled()
{
    
}
@end
