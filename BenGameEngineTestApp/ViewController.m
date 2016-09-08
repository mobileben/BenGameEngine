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
@property (nonatomic, weak) BGEView *glView;
@property (nonatomic, assign) BOOL once;
@property (nonatomic, assign) BGE::SpaceHandle spaceHandle;
@property (nonatomic, strong) dispatch_queue_t queue;
@property (nonatomic, strong) dispatch_source_t timer;
@property (nonatomic, assign) uint32_t stage;

@end

std::vector<BGE::ScenePackageHandle> packageHandles;

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

    self.queue = dispatch_queue_create("com.test", NULL);;
    self.timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, self.queue);
    
    dispatch_source_set_timer(self.timer,  DISPATCH_TIME_NOW, 1 * NSEC_PER_SEC, 1 * NSEC_PER_SEC);
    
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

        auto space = BGE::Game::getInstance()->getSpaceService()->createSpace("default");
        
        if (space) {
            self.spaceHandle = space->getHandle();
        } else {
            self.spaceHandle = BGE::SpaceHandle();
        }

        [self defaultPackageTest];
//        [self packageTest3];
    }
    
    [self.glView display];
}

- (void)defaultPackageTest
{
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

- (void)packageTest1
{
    self.stage = 0;
    
    __weak typeof(self) weakSelf = self;
    
    dispatch_source_set_event_handler(self.timer, ^{
        __strong typeof(self) strongSelf = weakSelf;
        
        if (strongSelf.stage == 0) {
            strongSelf.stage = 1;
            
            NSString *path;
            
            path = [[NSBundle mainBundle] pathForResource:@"Common-iPh6" ofType:@"json"];
            
            if (path) {
                BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "Common", [path UTF8String], [weakSelf](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                    __strong typeof(self) strongSelf = weakSelf;
                    
                    packageHandles.push_back(packageHandle);

                    BGE::Game::getInstance()->outputResourceBreakdown();
                    strongSelf.stage = 2;
                });
            }
        } else if (strongSelf.stage == 2) {
            BGE::Game::getInstance()->getScenePackageService()->removePackage(strongSelf.spaceHandle, packageHandles[0]);
            
            BGE::Game::getInstance()->outputResourceBreakdown();
            
            packageHandles.clear();
            
            strongSelf.stage = 0;
        }
    });
    
    dispatch_resume(self.timer);
}

- (void)packageTest2
{
    self.stage = 0;

    __weak typeof(self) weakSelf = self;
    
    dispatch_source_set_event_handler(self.timer, ^{
        __strong typeof(self) strongSelf = weakSelf;
        
        if (strongSelf.stage == 0 ) {
            strongSelf.stage = 1;
            
            NSString *path;
            
            path = [[NSBundle mainBundle] pathForResource:@"Common-iPh6" ofType:@"json"];
            
            if (path) {
                BGE::Game::getInstance()->getScenePackageService()->createPackage(strongSelf.spaceHandle, "Common", [path UTF8String], [weakSelf](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                    packageHandles.push_back(packageHandle);
                    
                    BGE::Game::getInstance()->outputResourceBreakdown();
                });
            }
            
            path = [[NSBundle mainBundle] pathForResource:@"CommonLobby-iPh6" ofType:@"json"];
            
            if (path) {
                BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "CommonLobby", [path UTF8String], [weakSelf](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                    __strong typeof(self) strongSelf = weakSelf;
                    packageHandles.push_back(packageHandle);
                    
                    BGE::Game::getInstance()->outputResourceBreakdown();
                    
                    strongSelf.stage = 2;
                });
            }
        } else if (strongSelf.stage == 2) {
            BGE::Game::getInstance()->getScenePackageService()->removePackage(strongSelf.spaceHandle, packageHandles[0]);
            BGE::Game::getInstance()->outputResourceBreakdown();
            strongSelf.stage++;
        } else if (strongSelf.stage == 3) {
            BGE::Game::getInstance()->getScenePackageService()->removePackage(strongSelf.spaceHandle, packageHandles[1]);
            BGE::Game::getInstance()->outputResourceBreakdown();
            packageHandles.clear();
            strongSelf.stage = 0;
        }
    });
    
    dispatch_resume(self.timer);
}

- (void)packageTest3
{
    self.stage = 0;
    
    __weak typeof(self) weakSelf = self;
    
    dispatch_source_set_event_handler(self.timer, ^{
        __strong typeof(self) strongSelf = weakSelf;
        
        if (strongSelf.stage == 0 ) {
            // Space handle already exists, so remove it
            BGE::Game::getInstance()->getSpaceService()->removeSpace(strongSelf.spaceHandle);

            auto space = BGE::Game::getInstance()->getSpaceService()->createSpace("default");
            
            if (space) {
                self.spaceHandle = space->getHandle();
            } else {
                self.spaceHandle = BGE::SpaceHandle();
            }

            strongSelf.stage = 1;
            
            NSString *path;
            
            path = [[NSBundle mainBundle] pathForResource:@"Common-iPh6" ofType:@"json"];
            
            if (path) {
                BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "Common", [path UTF8String], [weakSelf](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                    BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);

                    packageHandles.push_back(packageHandle);
                    
                    if (package) {
                        package->link();
                    }
                    
                    BGE::Game::getInstance()->outputResourceBreakdown();
                });
            }
            
            path = [[NSBundle mainBundle] pathForResource:@"CommonLobby-iPh6" ofType:@"json"];
            
            if (path) {
                BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "CommonLobby", [path UTF8String], [weakSelf](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                    BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);

                    packageHandles.push_back(packageHandle);

                    if (package) {
                        package->link();
                    }
                    
                    BGE::Game::getInstance()->outputResourceBreakdown();
                });
            }
            
            path = [[NSBundle mainBundle] pathForResource:@"CommonHUD-iPh6" ofType:@"json"];
            
            if (path) {
                BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "CommonHUD", [path UTF8String], [weakSelf](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                    BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                    
                    packageHandles.push_back(packageHandle);

                    if (package) {
                        package->link();
                    }
                    
                    BGE::Game::getInstance()->outputResourceBreakdown();
                });
            }
            
            path = [[NSBundle mainBundle] pathForResource:@"Lobby-iPh6" ofType:@"json"];
            
            if (path) {
                BGE::Game::getInstance()->getScenePackageService()->createPackage(self.spaceHandle, "Lobby", [path UTF8String], [weakSelf](BGE::ScenePackageHandle packageHandle, std::shared_ptr<BGE::Error> error) -> void {
                    __strong typeof(self) strongSelf = weakSelf;
                    BGE::ScenePackage *package = BGE::Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
                    
                    packageHandles.push_back(packageHandle);

                    if (package) {
                        package->link();
                    }
                    
                    auto space = BGE::Game::getInstance()->getSpaceService()->getSpace(strongSelf.spaceHandle);
                    
                    // Now create auto display objects
                    BGE::SceneObjectCreatedDelegate delegate;
                    
                    space->createAutoDisplayObjects(BGE::GameObjectHandle(), packageHandle, delegate);
                    
                    // Spaces are not visible by default
                    space->setVisible(true);
                    BGE::Game::getInstance()->outputResourceBreakdown();
                    BGE::Game::getInstance()->outputMemoryBreakdown();
                    strongSelf.stage = 2;
                });
            }
        } else if (strongSelf.stage == 2) {
            strongSelf.stage = 3;
        } else if (strongSelf.stage == 3) {
            auto space = BGE::Game::getInstance()->getSpaceService()->getSpace(strongSelf.spaceHandle);
            
            space->setVisible(false);
            
            BGE::Game::getInstance()->getSpaceService()->removeSpace(strongSelf.spaceHandle);
            
            BGE::Game::getInstance()->outputResourceBreakdown();
            packageHandles.clear();

            strongSelf.stage = 0;
        }
    });
    
    dispatch_resume(self.timer);
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

@end
