//
//  BGETransformComponentTests.m
//  BGETransformComponentTests
//
//  Created by Benjamin Lee on 4/1/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#import <XCTest/XCTest.h>
#include "BGEComponentService.h"
#include "BGETransformComponent.h"

static BGEComponentService componentService;

@interface BGETransformComponentTests : XCTestCase

@end

@implementation BGETransformComponentTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testConstructor {
    auto transform = componentService.createComponent<BGETransformComponent>("test");
    
    XCTAssertEqual(transform!=nullptr, true, @"transform not created");
    XCTAssertEqual(transform->getName(), "test", @"getName is incorrect");
    XCTAssertEqual(transform->isVisible(), true, @"isVisible is incorrect");
    XCTAssertEqual(transform->isInteractable(), true, @"isInteractable is incorrect");
    XCTAssertEqual(transform->isInteractableWhenHidden(), false, @"isInteractableWhenHidden is incorrect");
    
    BGERect rect;
    
    transform->getBounds(rect);
    
    XCTAssertEqual(rect.x, 0, @"rect.x is incorrect");
    XCTAssertEqual(rect.y, 0, @"rect.y is incorrect");
    XCTAssertEqual(rect.w, 0, @"rect.w is incorrect");
    XCTAssertEqual(rect.h, 0, @"rect.h is incorrect");
    
    XCTAssertEqual(transform->getPosition().x, 0, @"getPosition().x is incorrect");
    XCTAssertEqual(transform->getPosition().y, 0, @"getPosition().y is incorrect");
    XCTAssertEqual(transform->getZ(), 0, @"getZ() is incorrect");

    XCTAssertEqual(transform->getScale().x, 1, @"getScale().x is incorrect");
    XCTAssertEqual(transform->getScale().y, 1, @"getScale().y is incorrect");
    
    XCTAssertEqual(transform->getSkew().x, 0, @"getSkew().x is incorrect");
    XCTAssertEqual(transform->getSkew().y, 0, @"getSkew().y is incorrect");
    
    XCTAssertEqual(transform->getRotation(), 0, @"getRotation() is incorrect");
    
    BGEMatrix4 matrix;
    
    transform->getMatrix(matrix);
    
    XCTAssertEqual(BGEMatrix4IsIdentity(matrix), true, @"matrix is not identity");
}

- (void)testName {
    auto name1 = "test1";
    auto name2 = "test2";
    auto newname1 = "newtest1";
    auto newname2 = "newtest2";
    auto transform1 = componentService.createComponent<BGETransformComponent>(name1);
    auto transform2 = componentService.createComponent<BGETransformComponent>(name2);
    XCTAssertEqual(transform1!=nullptr, true, @"transform not created");
    XCTAssertEqual(transform1->getName(), name1, @"getName is incorrect");
    XCTAssertEqual(transform2!=nullptr, true, @"transform not created");
    XCTAssertEqual(transform2->getName(), name2, @"getName is incorrect");

    // Change name for transform1
    transform1->setName(newname1);
    XCTAssertEqual(transform1->getName(), newname1, @"getName is incorrect");
    XCTAssertEqual(transform2->getName(), name2, @"getName is incorrect");
    transform2->setName(newname2);
    XCTAssertEqual(transform1->getName(), newname1, @"getName is incorrect");
    XCTAssertEqual(transform2->getName(), newname2, @"getName is incorrect");
}

- (void)testVisibilityAndInteractability {
    auto name = "test";
    auto transform = componentService.createComponent<BGETransformComponent>(name);

    XCTAssertEqual(transform->isVisible(), true, @"isVisible is incorrect");
    XCTAssertEqual(transform->isInteractable(), true, @"isInteractable is incorrect");
    XCTAssertEqual(transform->isInteractableWhenHidden(), false, @"isInteractableWhenHidden is incorrect");
    
    transform->setVisibility(false);
    XCTAssertEqual(transform->isVisible(), false, @"isVisible is incorrect");
    transform->setVisibility(true);
    XCTAssertEqual(transform->isVisible(), true, @"isVisible is incorrect");

    transform->setInteractability(false);
    XCTAssertEqual(transform->isInteractable(), false, @"isInteractable is incorrect");
    transform->setInteractability(true);
    XCTAssertEqual(transform->isInteractable(), true, @"isInteractable is incorrect");
    
    transform->setInteractabilityWhenHidden(false);
    XCTAssertEqual(transform->isInteractableWhenHidden(), false, @"isInteractableWhenHidden is incorrect");
    transform->setInteractabilityWhenHidden(true);
    XCTAssertEqual(transform->isInteractableWhenHidden(), true, @"isInteractableWhenHidden is incorrect");
}

- (void)testPosition {
    auto name = "test";
    auto transform = componentService.createComponent<BGETransformComponent>(name);
    
    BGERect rect;
    
    transform->getBounds(rect);
    
    XCTAssertEqual(rect.x, 0, @"rect.x is incorrect");
    XCTAssertEqual(rect.y, 0, @"rect.y is incorrect");
    XCTAssertEqual(rect.w, 0, @"rect.w is incorrect");
    XCTAssertEqual(rect.h, 0, @"rect.h is incorrect");
    
    XCTAssertEqual(transform->getPosition().x, 0, @"getPosition().x is incorrect");
    XCTAssertEqual(transform->getPosition().y, 0, @"getPosition().y is incorrect");
    XCTAssertEqual(transform->getZ(), 0, @"getZ() is incorrect");
    
    float x = 50;
    float y = 90;
    float z = 100;
    
    BGEVector2 pos;
    
    transform->setX(x);
    transform->setY(y);
    transform->setZ(z);
    pos = transform->getPosition();
    
    XCTAssertEqual(transform->getX(), x, @"getX().x is incorrect");
    XCTAssertEqual(transform->getY(), y, @"getY() is incorrect");
    XCTAssertEqual(pos.x, x, @"getPosition().x is incorrect");
    XCTAssertEqual(pos.y, y, @"getPosition().y is incorrect");
    XCTAssertEqual(transform->getZ(), z, @"getZ() is incorrect");

    BGEVector2 newPosition = { 10, 20 };
    
    transform->setPosition(newPosition);
    pos = transform->getPosition();
    XCTAssertEqual(transform->getX(), newPosition.x, @"getX() is incorrect");
    XCTAssertEqual(transform->getY(), newPosition.y, @"getY() is incorrect");
    XCTAssertEqual(pos.x, newPosition.x, @"getPosition().x is incorrect");
    XCTAssertEqual(pos.y, newPosition.y, @"getPosition().y is incorrect");
    XCTAssertEqual(transform->getZ(), z, @"getZ() is incorrect");
    
    // TODO: bounds
    // TODO: matrix
}

- (void)testScale {
    auto name = "test";
    auto transform = componentService.createComponent<BGETransformComponent>(name);
    
    BGERect rect;
    
    transform->getBounds(rect);
    
    XCTAssertEqual(rect.x, 0, @"rect.x is incorrect");
    XCTAssertEqual(rect.y, 0, @"rect.y is incorrect");
    XCTAssertEqual(rect.w, 0, @"rect.w is incorrect");
    XCTAssertEqual(rect.h, 0, @"rect.h is incorrect");

    XCTAssertEqual(transform->getScale().x, 1, @"getScale().x is incorrect");
    XCTAssertEqual(transform->getScale().y, 1, @"getScale().y is incorrect");

    BGEVector2 scale;
    
    float x = 3.1;
    float y = 4.8;
    
    transform->setScaleX(x);
    transform->setScaleY(y);
    scale = transform->getScale();
    
    XCTAssertEqual(transform->getScaleX(), x, @"getScaleX() is incorrect");
    XCTAssertEqual(transform->getScaleY(), y, @"getScaleY() is incorrect");
    XCTAssertEqual(scale.x, x, @"getScale().x is incorrect");
    XCTAssertEqual(scale.y, y, @"getScale().y is incorrect");

    BGEVector2 newScale = { 1.7, 2.9 };
    
    transform->setScale(newScale);
    scale = transform->getScale();
    
    XCTAssertEqual(transform->getScaleX(), newScale.x, @"getScaleX() is incorrect");
    XCTAssertEqual(transform->getScaleY(), newScale.y, @"getScaleY() is incorrect");
    XCTAssertEqual(scale.x, newScale.x, @"getScale().x is incorrect");
    XCTAssertEqual(scale.y, newScale.y, @"getScale().y is incorrect");
    
    // TODO: bounds
    // TODO: matrix
}

- (void)testSkew {
    auto name = "test";
    auto transform = componentService.createComponent<BGETransformComponent>(name);
    
    BGERect rect;
    
    transform->getBounds(rect);
    
    XCTAssertEqual(rect.x, 0, @"rect.x is incorrect");
    XCTAssertEqual(rect.y, 0, @"rect.y is incorrect");
    XCTAssertEqual(rect.w, 0, @"rect.w is incorrect");
    XCTAssertEqual(rect.h, 0, @"rect.h is incorrect");
    
    XCTAssertEqual(transform->getSkew().x, 0, @"getSkew().x is incorrect");
    XCTAssertEqual(transform->getSkew().y, 0, @"getSkew().y is incorrect");
    
    BGEVector2 skew;
    
    float x = 3.1;
    float y = 4.8;
    
    transform->setSkewX(x);
    transform->setSkewY(y);
    skew = transform->getSkew();
    
    XCTAssertEqual(transform->getSkewX(), x, @"getSkewX() is incorrect");
    XCTAssertEqual(transform->getSkewY(), y, @"getSkewY() is incorrect");
    XCTAssertEqual(skew.x, x, @"getSkew().x is incorrect");
    XCTAssertEqual(skew.y, y, @"getSkew().y is incorrect");
    
    BGEVector2 newSkew = { 1.7, 2.9 };
    
    transform->setSkew(newSkew);
    skew = transform->getSkew();
    
    XCTAssertEqual(transform->getSkewX(), newSkew.x, @"getSkewX() is incorrect");
    XCTAssertEqual(transform->getSkewY(), newSkew.y, @"getSkewY() is incorrect");
    XCTAssertEqual(skew.x, newSkew.x, @"getSkew().x is incorrect");
    XCTAssertEqual(skew.y, newSkew.y, @"getSkew().y is incorrect");
    
    // TODO: bounds
    // TODO: matrix
}

- (void)testRotate {
    auto name = "test";
    auto transform = componentService.createComponent<BGETransformComponent>(name);
    
    BGERect rect;
    
    transform->getBounds(rect);
    
    XCTAssertEqual(rect.x, 0, @"rect.x is incorrect");
    XCTAssertEqual(rect.y, 0, @"rect.y is incorrect");
    XCTAssertEqual(rect.w, 0, @"rect.w is incorrect");
    XCTAssertEqual(rect.h, 0, @"rect.h is incorrect");
    
    XCTAssertEqual(transform->getRotation(), 0, @"getRotation() is incorrect");
    
    float rotation = 3.1;
    
    transform->setRotation(rotation);
    
    XCTAssertEqual(transform->getRotation(), rotation, @"getRotation() is incorrect");
    
    // TODO: bounds
    // TODO: matrix
}

- (void)testMatrix {
    
}

- (void)testtransformHierarchy {
    std::string name0 = "name0";
    std::string name00 = "name00";
    std::string name01 = "name01";
    std::string name02 = "name02";
    std::string name000 = "name000";
    std::string name001 = "name001";
    std::string name0000 = "name0000";
    std::string name0001 = "name0001";
    std::string name0010 = "name0010";
    std::string name0011 = "name0011";
    std::string name010 = "name010";
    std::string name011 = "name011";
    std::string name0100 = "name0100";
    std::string name0110 = "name0110";
    std::string name020 = "name020";
    std::string name021 = "name021";
    
    auto transform0 = componentService.createComponent<BGETransformComponent>(name0);
    auto transform00 = componentService.createComponent<BGETransformComponent>(name00);
    auto transform01 = componentService.createComponent<BGETransformComponent>(name01);
    auto transform02 = componentService.createComponent<BGETransformComponent>(name02);
    auto transform000 = componentService.createComponent<BGETransformComponent>(name000);
    auto transform001 = componentService.createComponent<BGETransformComponent>(name001);
    auto transform0000 = componentService.createComponent<BGETransformComponent>(name0000);
    auto transform0001 = componentService.createComponent<BGETransformComponent>(name0001);
    auto transform0010 = componentService.createComponent<BGETransformComponent>(name0010);
    auto transform0011 = componentService.createComponent<BGETransformComponent>(name0011);
    auto transform010 = componentService.createComponent<BGETransformComponent>(name010);
    auto transform011 = componentService.createComponent<BGETransformComponent>(name011);
    auto transform0100 = componentService.createComponent<BGETransformComponent>(name0100);
    auto transform0110 = componentService.createComponent<BGETransformComponent>(name0110);
    auto transform020 = componentService.createComponent<BGETransformComponent>(name020);
    auto transform021 = componentService.createComponent<BGETransformComponent>(name021);
    
    std::shared_ptr<BGETransformComponent> parent;
    
    // transform0 (root)
    XCTAssertEqual(transform0->getNumChildren(), 0, "wrong num children");
    XCTAssertEqual(transform0->childWithName(name00), nullptr, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name01), nullptr, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name02), nullptr, "wrong child with name");
    XCTAssertEqual(transform0->hasChild(transform00), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform01), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform02), false, "wrong hasChild");
    XCTAssertEqual(transform00->inParentHierarchy(transform0), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform01->inParentHierarchy(transform0), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform02->inParentHierarchy(transform0), false, "wrong inParentHierarchy");

    transform0->addChild(transform00);
    XCTAssertEqual(transform0->getNumChildren(), 1, "wrong num children");
    parent = transform00->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), transform00, "wrong child at index");
    
    transform0->addChild(transform01);
    XCTAssertEqual(transform0->getNumChildren(), 2, "wrong num children");
    parent = transform01->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), transform00, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), transform01, "wrong child at index");
    
    transform0->addChild(transform02);
    XCTAssertEqual(transform0->getNumChildren(), 3, "wrong num children");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), transform00, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), transform01, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(2), transform02, "wrong child at index");

    XCTAssertEqual(transform0->childWithName(name00), transform00, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name00, true), transform00, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name01), transform01, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name01, true), transform01, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name02), transform02, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name02, true), transform02, "wrong child with name");
    
    XCTAssertEqual(transform0->hasChild(transform00), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform00, true), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform01), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform01, true), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform02), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform02, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform00->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform01->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform02->inParentHierarchy(transform0), true, "wrong inParentHierarchy");

    // transform00
    XCTAssertEqual(transform00->getNumChildren(), 0, "wrong num children");
    XCTAssertEqual(transform00->childWithName(name000), nullptr, "wrong child with name");
    XCTAssertEqual(transform00->childWithName(name001), nullptr, "wrong child with name");
    XCTAssertEqual(transform00->hasChild(transform000), false, "wrong hasChild");
    XCTAssertEqual(transform00->hasChild(transform001), false, "wrong hasChild");
    XCTAssertEqual(transform000->inParentHierarchy(transform00), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform001->inParentHierarchy(transform00), false, "wrong inParentHierarchy");
    
    transform00->addChild(transform000);
    XCTAssertEqual(transform00->getNumChildren(), 1, "wrong num children");
    parent = transform000->getParent().lock();
    XCTAssertEqual(parent, transform00, "wrong parent");
    XCTAssertEqual(transform00->childAtIndex(0), transform000, "wrong child at index");

    transform00->addChild(transform001);
    XCTAssertEqual(transform00->getNumChildren(), 2, "wrong num children");
    parent = transform001->getParent().lock();
    XCTAssertEqual(parent, transform00, "wrong parent");
    XCTAssertEqual(transform00->childAtIndex(0), transform000, "wrong child at index");
    XCTAssertEqual(transform00->childAtIndex(1), transform001, "wrong child at index");
    
    XCTAssertEqual(transform00->childWithName(name000), transform000, "wrong child with name");
    XCTAssertEqual(transform00->childWithName(name000, true), transform000, "wrong child with name");
    XCTAssertEqual(transform00->childWithName(name001), transform001, "wrong child with name");
    XCTAssertEqual(transform00->childWithName(name001, true), transform001, "wrong child with name");

    XCTAssertEqual(transform0->childWithName(name000), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name000, true), transform000, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name001), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name001, true), transform001, "wrong child with name");

    XCTAssertEqual(transform00->hasChild(transform000), true, "wrong hasChild");
    XCTAssertEqual(transform00->hasChild(transform000, true), true, "wrong hasChild");
    XCTAssertEqual(transform00->hasChild(transform001), true, "wrong hasChild");
    XCTAssertEqual(transform00->hasChild(transform001, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0->hasChild(transform000), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform000, true), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform001), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform001, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform000->inParentHierarchy(transform00), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform001->inParentHierarchy(transform00), true, "wrong inParentHierarchy");

    XCTAssertEqual(transform000->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform001->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform000->inParentHierarchy(transform01), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform001->inParentHierarchy(transform01), false, "wrong inParentHierarchy");

    // transform000
    XCTAssertEqual(transform000->getNumChildren(), 0, "wrong num children");
    XCTAssertEqual(transform000->childWithName(name0000), nullptr, "wrong child with name");
    XCTAssertEqual(transform000->childWithName(name0001), nullptr, "wrong child with name");
    XCTAssertEqual(transform000->hasChild(transform0000), false, "wrong hasChild");
    XCTAssertEqual(transform000->hasChild(transform0001), false, "wrong hasChild");
    XCTAssertEqual(transform0000->inParentHierarchy(transform000), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform0001->inParentHierarchy(transform000), false, "wrong inParentHierarchy");

    transform000->addChild(transform0000);
    XCTAssertEqual(transform000->getNumChildren(), 1, "wrong num children");
    parent = transform0000->getParent().lock();
    XCTAssertEqual(parent, transform000, "wrong parent");
    XCTAssertEqual(transform000->childAtIndex(0), transform0000, "wrong child at index");
    
    transform000->addChild(transform0001);
    XCTAssertEqual(transform000->getNumChildren(), 2, "wrong num children");
    parent = transform0001->getParent().lock();
    XCTAssertEqual(parent, transform000, "wrong parent");
    XCTAssertEqual(transform000->childAtIndex(0), transform0000, "wrong child at index");
    XCTAssertEqual(transform000->childAtIndex(1), transform0001, "wrong child at index");
    
    XCTAssertEqual(transform000->childWithName(name0000), transform0000, "wrong child with name");
    XCTAssertEqual(transform000->childWithName(name0000, true), transform0000, "wrong child with name");
    XCTAssertEqual(transform000->childWithName(name0001), transform0001, "wrong child with name");
    XCTAssertEqual(transform000->childWithName(name0001, true), transform0001, "wrong child with name");
    
    XCTAssertEqual(transform0->childWithName(name0000), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name0000, true), transform0000, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name0001), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name0001, true), transform0001, "wrong child with name");
    
    XCTAssertEqual(transform000->hasChild(transform0000), true, "wrong hasChild");
    XCTAssertEqual(transform000->hasChild(transform0000, true), true, "wrong hasChild");
    XCTAssertEqual(transform000->hasChild(transform0001), true, "wrong hasChild");
    XCTAssertEqual(transform000->hasChild(transform0001, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0->hasChild(transform0000), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform0000, true), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform0001), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform0001, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0000->inParentHierarchy(transform000), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform0001->inParentHierarchy(transform000), true, "wrong inParentHierarchy");
    
    XCTAssertEqual(transform0000->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform0001->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform0000->inParentHierarchy(transform01), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform0001->inParentHierarchy(transform01), false, "wrong inParentHierarchy");

    // transform001
    XCTAssertEqual(transform001->getNumChildren(), 0, "wrong num children");
    XCTAssertEqual(transform001->childWithName(name0010), nullptr, "wrong child with name");
    XCTAssertEqual(transform001->childWithName(name0011), nullptr, "wrong child with name");
    XCTAssertEqual(transform001->hasChild(transform0010), false, "wrong hasChild");
    XCTAssertEqual(transform001->hasChild(transform0011), false, "wrong hasChild");
    XCTAssertEqual(transform0010->inParentHierarchy(transform001), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform0011->inParentHierarchy(transform001), false, "wrong inParentHierarchy");

    transform001->addChild(transform0010);
    XCTAssertEqual(transform001->getNumChildren(), 1, "wrong num children");
    parent = transform0010->getParent().lock();
    XCTAssertEqual(parent, transform001, "wrong parent");
    XCTAssertEqual(transform001->childAtIndex(0), transform0010, "wrong child at index");

    transform001->addChild(transform0011);
    XCTAssertEqual(transform001->getNumChildren(), 2, "wrong num children");
    parent = transform0011->getParent().lock();
    XCTAssertEqual(parent, transform001, "wrong parent");
    XCTAssertEqual(transform001->childAtIndex(0), transform0010, "wrong child at index");
    XCTAssertEqual(transform001->childAtIndex(1), transform0011, "wrong child at index");
    
    XCTAssertEqual(transform001->childWithName(name0010), transform0010, "wrong child with name");
    XCTAssertEqual(transform001->childWithName(name0010, true), transform0010, "wrong child with name");
    XCTAssertEqual(transform001->childWithName(name0011), transform0011, "wrong child with name");
    XCTAssertEqual(transform001->childWithName(name0011, true), transform0011, "wrong child with name");
    
    XCTAssertEqual(transform0->childWithName(name0010), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name0010, true), transform0010, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name0011), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name0011, true), transform0011, "wrong child with name");
    
    XCTAssertEqual(transform001->hasChild(transform0010), true, "wrong hasChild");
    XCTAssertEqual(transform001->hasChild(transform0010, true), true, "wrong hasChild");
    XCTAssertEqual(transform001->hasChild(transform0011), true, "wrong hasChild");
    XCTAssertEqual(transform001->hasChild(transform0011, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0->hasChild(transform0010), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform0010, true), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform0011), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform0011, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0010->inParentHierarchy(transform001), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform0011->inParentHierarchy(transform001), true, "wrong inParentHierarchy");
    
    XCTAssertEqual(transform0010->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform0011->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform0010->inParentHierarchy(transform01), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform0011->inParentHierarchy(transform01), false, "wrong inParentHierarchy");

    // transform01
    XCTAssertEqual(transform01->getNumChildren(), 0, "wrong num children");
    XCTAssertEqual(transform01->childWithName(name010), nullptr, "wrong child with name");
    XCTAssertEqual(transform01->childWithName(name011), nullptr, "wrong child with name");
    XCTAssertEqual(transform01->hasChild(transform010), false, "wrong hasChild");
    XCTAssertEqual(transform01->hasChild(transform011), false, "wrong hasChild");
    XCTAssertEqual(transform010->inParentHierarchy(transform01), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform011->inParentHierarchy(transform01), false, "wrong inParentHierarchy");

    transform01->addChild(transform010);
    XCTAssertEqual(transform01->getNumChildren(), 1, "wrong num children");
    parent = transform010->getParent().lock();
    XCTAssertEqual(parent, transform01, "wrong parent");
    XCTAssertEqual(transform01->childAtIndex(0), transform010, "wrong child at index");

    transform01->addChild(transform011);
    XCTAssertEqual(transform01->getNumChildren(), 2, "wrong num children");
    parent = transform011->getParent().lock();
    XCTAssertEqual(parent, transform01, "wrong parent");
    XCTAssertEqual(transform01->childAtIndex(0), transform010, "wrong child at index");
    XCTAssertEqual(transform01->childAtIndex(1), transform011, "wrong child at index");
    
    XCTAssertEqual(transform01->childWithName(name010), transform010, "wrong child with name");
    XCTAssertEqual(transform01->childWithName(name010, true), transform010, "wrong child with name");
    XCTAssertEqual(transform01->childWithName(name011), transform011, "wrong child with name");
    XCTAssertEqual(transform01->childWithName(name011, true), transform011, "wrong child with name");
    
    XCTAssertEqual(transform0->childWithName(name010), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name010, true), transform010, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name011), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name011, true), transform011, "wrong child with name");
    
    XCTAssertEqual(transform01->hasChild(transform010), true, "wrong hasChild");
    XCTAssertEqual(transform01->hasChild(transform010, true), true, "wrong hasChild");
    XCTAssertEqual(transform01->hasChild(transform011), true, "wrong hasChild");
    XCTAssertEqual(transform01->hasChild(transform011, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0->hasChild(transform010), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform010, true), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform011), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform011, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform010->inParentHierarchy(transform01), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform011->inParentHierarchy(transform01), true, "wrong inParentHierarchy");
    
    XCTAssertEqual(transform010->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform011->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform010->inParentHierarchy(transform00), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform011->inParentHierarchy(transform00), false, "wrong inParentHierarchy");
    
    // transform010
    XCTAssertEqual(transform010->getNumChildren(), 0, "wrong num children");
    XCTAssertEqual(transform010->childWithName(name0100), nullptr, "wrong child with name");
    XCTAssertEqual(transform010->hasChild(transform0100), false, "wrong hasChild");
    XCTAssertEqual(transform0100->inParentHierarchy(transform010), false, "wrong inParentHierarchy");

    transform010->addChild(transform0100);
    XCTAssertEqual(transform010->getNumChildren(), 1, "wrong num children");
    parent = transform0100->getParent().lock();
    XCTAssertEqual(parent, transform010, "wrong parent");
    XCTAssertEqual(transform010->childAtIndex(0), transform0100, "wrong child at index");
    
    XCTAssertEqual(transform010->childWithName(name0100), transform0100, "wrong child with name");
    XCTAssertEqual(transform010->childWithName(name0100, true), transform0100, "wrong child with name");
    
    XCTAssertEqual(transform0->childWithName(name0100), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name0100, true), transform0100, "wrong child with name");
    
    XCTAssertEqual(transform010->hasChild(transform0100), true, "wrong hasChild");
    XCTAssertEqual(transform010->hasChild(transform0100, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0->hasChild(transform0100), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform0100, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0100->inParentHierarchy(transform010), true, "wrong inParentHierarchy");
    
    XCTAssertEqual(transform0100->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform0100->inParentHierarchy(transform00), false, "wrong inParentHierarchy");

    // transform011
    XCTAssertEqual(transform011->getNumChildren(), 0, "wrong num children");
    XCTAssertEqual(transform011->childWithName(name0110), nullptr, "wrong child with name");
    XCTAssertEqual(transform011->hasChild(transform0110), false, "wrong hasChild");
    XCTAssertEqual(transform0110->inParentHierarchy(transform011), false, "wrong inParentHierarchy");
    
    transform011->addChild(transform0110);
    XCTAssertEqual(transform011->getNumChildren(), 1, "wrong num children");
    parent = transform0110->getParent().lock();
    XCTAssertEqual(parent, transform011, "wrong parent");
    XCTAssertEqual(transform011->childAtIndex(0), transform0110, "wrong child at index");
    
    XCTAssertEqual(transform011->childWithName(name0110), transform0110, "wrong child with name");
    XCTAssertEqual(transform011->childWithName(name0110, true), transform0110, "wrong child with name");
    
    XCTAssertEqual(transform0->childWithName(name0110), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name0110, true), transform0110, "wrong child with name");
    
    XCTAssertEqual(transform011->hasChild(transform0110), true, "wrong hasChild");
    XCTAssertEqual(transform011->hasChild(transform0110, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0->hasChild(transform0110), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform0110, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0110->inParentHierarchy(transform011), true, "wrong inParentHierarchy");
    
    XCTAssertEqual(transform0110->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform0110->inParentHierarchy(transform00), false, "wrong inParentHierarchy");
    
    // transform02
    XCTAssertEqual(transform02->getNumChildren(), 0, "wrong num children");
    XCTAssertEqual(transform02->childWithName(name020), nullptr, "wrong child with name");
    XCTAssertEqual(transform02->childWithName(name021), nullptr, "wrong child with name");
    XCTAssertEqual(transform02->hasChild(transform020), false, "wrong hasChild");
    XCTAssertEqual(transform02->hasChild(transform021), false, "wrong hasChild");
    XCTAssertEqual(transform020->inParentHierarchy(transform02), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform021->inParentHierarchy(transform02), false, "wrong inParentHierarchy");
    
    transform02->addChild(transform020);
    XCTAssertEqual(transform02->getNumChildren(), 1, "wrong num children");
    parent = transform020->getParent().lock();
    XCTAssertEqual(parent, transform02, "wrong parent");
    XCTAssertEqual(transform02->childAtIndex(0), transform020, "wrong child at index");
    
    transform02->addChild(transform021);
    XCTAssertEqual(transform02->getNumChildren(), 2, "wrong num children");
    parent = transform021->getParent().lock();
    XCTAssertEqual(parent, transform02, "wrong parent");
    XCTAssertEqual(transform02->childAtIndex(0), transform020, "wrong child at index");
    XCTAssertEqual(transform02->childAtIndex(1), transform021, "wrong child at index");
    
    XCTAssertEqual(transform02->childWithName(name020), transform020, "wrong child with name");
    XCTAssertEqual(transform02->childWithName(name020, true), transform020, "wrong child with name");
    XCTAssertEqual(transform02->childWithName(name021), transform021, "wrong child with name");
    XCTAssertEqual(transform02->childWithName(name021, true), transform021, "wrong child with name");
    
    XCTAssertEqual(transform0->childWithName(name020), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name020, true), transform020, "wrong child with name");
    XCTAssertEqual(transform0->childWithName(name021), nullptr, "wrong child with name");    // Should fail since is not descending
    XCTAssertEqual(transform0->childWithName(name021, true), transform021, "wrong child with name");
    
    XCTAssertEqual(transform02->hasChild(transform020), true, "wrong hasChild");
    XCTAssertEqual(transform02->hasChild(transform020, true), true, "wrong hasChild");
    XCTAssertEqual(transform02->hasChild(transform021), true, "wrong hasChild");
    XCTAssertEqual(transform02->hasChild(transform021, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform0->hasChild(transform020), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform020, true), true, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform021), false, "wrong hasChild");
    XCTAssertEqual(transform0->hasChild(transform021, true), true, "wrong hasChild");
    
    XCTAssertEqual(transform020->inParentHierarchy(transform02), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform021->inParentHierarchy(transform02), true, "wrong inParentHierarchy");
    
    XCTAssertEqual(transform020->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform021->inParentHierarchy(transform0), true, "wrong inParentHierarchy");
    XCTAssertEqual(transform020->inParentHierarchy(transform00), false, "wrong inParentHierarchy");
    XCTAssertEqual(transform021->inParentHierarchy(transform00), false, "wrong inParentHierarchy");
    
    // removeFromParent
    transform01->removeFromParent();
    XCTAssertEqual(transform0->getNumChildren(), 2, "wrong num children");
    parent = transform00->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    parent = transform01->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), transform00, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), transform02, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(2), nullptr, "wrong child at index");
    
    transform00->removeFromParent();
    XCTAssertEqual(transform0->getNumChildren(), 1, "wrong num children");
    parent = transform00->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    parent = transform01->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), transform02, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), nullptr, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(2), nullptr, "wrong child at index");
    
    transform02->removeFromParent();
    XCTAssertEqual(transform0->getNumChildren(), 0, "wrong num children");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    parent = transform01->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), nullptr, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), nullptr, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(2), nullptr, "wrong child at index");

    // insertChild
    transform0->insertChild(transform01, 0);
    XCTAssertEqual(transform0->getNumChildren(), 1, "wrong num children");
    parent = transform00->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    parent = transform01->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), transform01, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), nullptr, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(2), nullptr, "wrong child at index");
    
    transform0->insertChild(transform02, 0);
    XCTAssertEqual(transform0->getNumChildren(), 2, "wrong num children");
    parent = transform00->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    parent = transform01->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), transform02, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), transform01, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(2), nullptr, "wrong child at index");
    
    transform0->insertChild(transform00, 1);
    XCTAssertEqual(transform0->getNumChildren(), 3, "wrong num children");
    parent = transform00->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    parent = transform01->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), transform02, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), transform00, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(2), transform01, "wrong child at index");
    
    transform02->removeFromParent();
    transform0->insertChild(transform02, 10);
    XCTAssertEqual(transform0->getNumChildren(), 3, "wrong num children");
    parent = transform00->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    parent = transform01->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, transform0, "wrong parent");
    XCTAssertEqual(transform0->childAtIndex(0), transform00, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), transform01, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(2), transform02, "wrong child at index");
    
    // moveToParent
    transform020->moveToParent(transform0);
    XCTAssertEqual(transform0->getNumChildren(), 4, "wrong num children");
    XCTAssertEqual(transform0->childAtIndex(0), transform00, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(1), transform01, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(2), transform02, "wrong child at index");
    XCTAssertEqual(transform0->childAtIndex(3), transform020, "wrong child at index");
    XCTAssertEqual(transform02->getNumChildren(), 1, "wrong num children");
    
    //  removeAllChildren
    transform0->removeAllChildren();
    XCTAssertEqual(transform0->getNumChildren(), 0, "wrong num children");
    parent = transform00->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    parent = transform01->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
    parent = transform02->getParent().lock();
    XCTAssertEqual(parent, nullptr, "wrong parent");
}

- (void)testtransformHierarchAsserts {
    
}

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
