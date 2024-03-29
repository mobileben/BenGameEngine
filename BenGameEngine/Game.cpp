//
//  Game.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Game.h"
#include <locale>
#include <sstream>

#include "TransformComponent.h"

template<typename T>
struct Sep : public std::numpunct<T>
{
    virtual std::string do_grouping()      const   {return "\003";}
};

std::shared_ptr<BGE::Game> BGE::Game::getInstance() {
    static std::shared_ptr<Game> instance = std::make_shared<Game>();
    return instance;
}

std::string BGE::Game::AnyToString::toString() const {
    std::stringstream ss;
    ss.imbue(std::locale(std::locale(""), new Sep <char>()));
    
    switch(mType) {
        case type::Int:
            ss << std::fixed << mData.INT;
            break;
        case type::UInt:
            ss << std::fixed << mData.UINT;
            break;
        case type::Double:
            ss << std::fixed << mData.DOUBLE;
            break;
        case type::SizeT:
            ss << std::fixed << mData.SIZET;
            break;
            
        case type::String:
            ss << std::fixed << mData.STRING;
    }
    
    return ss.str();
}

void BGE::Game::outputValue(uint32_t numTabs, const std::string& format, va_list args) {
    std::string results = format;
    
    for (auto i=0u;i<numTabs;i++) {
        printf("\t");
    }
    
    std::vector<AnyToString> vec;
    
    const char *str = format.c_str();
    bool inFormatSpecifier = false;
    char formatSpecifier[3]; // Indicates current format specifier
    auto formatSpecifierIndex = 0u;
    
    formatSpecifier[0] = '\0';
    formatSpecifier[1] = '\0';
    formatSpecifier[2] = '\0';
    
    while (*str != '\0') {
        auto ch = *str++;
        
        if (inFormatSpecifier) {
            bool reset = false;
            // Now process the format specifier
            auto fmtSpec = "%" + std::string(formatSpecifier);
            std::string sVal;
            
            if (!strcmp(formatSpecifier, "d")) {
                int32_t i = va_arg(args, int32_t);
                AnyToString n(i);
                sVal = n.toString();
                reset = true;
            } else if (!strcmp(formatSpecifier, "u")) {
                uint32_t u = va_arg(args, uint32_t);
                AnyToString n(u);
                sVal = n.toString();
                reset = true;
            } else if (!strcmp(formatSpecifier, "f")) {
                double d = va_arg(args, double);
                AnyToString n(d);
                sVal = n.toString();
                reset = true;
            } else if (!strcmp(formatSpecifier, "ld") || !strcmp(formatSpecifier, "zd")) {
                size_t s = va_arg(args, size_t);
                AnyToString n(s);
                sVal = n.toString();
                reset = true;
            } else if (!strcmp(formatSpecifier, "s")) {
                char * c = va_arg(args, char *);
                sVal = std::string(c);
                reset = true;
            } else {
                formatSpecifier[formatSpecifierIndex++] = ch;
                assert(formatSpecifierIndex<sizeof(formatSpecifier));
            }
            
            if (reset) {
                auto f = results.find(fmtSpec);
                results.replace(f, fmtSpec.length(), sVal);

                inFormatSpecifier = false;
                
                formatSpecifierIndex = 0;
                formatSpecifier[0] = '\0';
                formatSpecifier[1] = '\0';
                formatSpecifier[2] = '\0';
            } else {
            }
        } else {
            if (ch == '%') {
                inFormatSpecifier = true;
            }
        }
    }
    
    // Handle and format specifier we happen to be in right now
    if (inFormatSpecifier) {
        // Now process the format specifier
        auto fmtSpec = "%" + std::string(formatSpecifier);
        std::string sVal;
        
        if (!strcmp(formatSpecifier, "d")) {
            int32_t i = va_arg(args, int32_t);
            AnyToString n(i);
            sVal = n.toString();
        } else if (!strcmp(formatSpecifier, "u")) {
            uint32_t u = va_arg(args, uint32_t);
            AnyToString n(u);
            sVal = n.toString();
        } else if (!strcmp(formatSpecifier, "f")) {
            double d = va_arg(args, double);
            AnyToString n(d);
            sVal = n.toString();
        } else if (!strcmp(formatSpecifier, "ld") || !strcmp(formatSpecifier, "zd")) {
            size_t s = va_arg(args, size_t);
            AnyToString n(s);
            sVal = n.toString();
        } else if (!strcmp(formatSpecifier, "s")) {
            char * c = va_arg(args, char *);
            sVal = std::string(c);
        } else {
            assert(false);
        }
        
        auto f = results.find(fmtSpec);
        results.replace(f, fmtSpec.length(), sVal);
    }
    
    printf("%s", results.c_str());
}

BGE::Game::Game() : paused_(false), wireframeMode_(WireframeMode::Off), showCollisionRects_(false)
{
    handleServicesInitialize();
    
    materialService_ = std::make_shared<MaterialService>();
    heartbeatService_ = std::make_shared<HeartbeatService>();
    scenePackageService_ = std::make_shared<ScenePackageService>();
    eventService_ = std::make_shared<EventService>();
    animationService_ = std::make_shared<AnimationService>(eventService_);
    inputService_ = std::make_shared<InputService>(eventService_);
    spaceService_ = std::make_shared<SpaceService>();
    fontService_ = std::make_shared<FontService>();
    logicService_ = std::make_shared<LogicService>();
    audioService_ = std::make_shared<AudioService>(eventService_);
}

void BGE::Game::provide(std::shared_ptr<RenderService> renderService) {
    renderService_ = renderService;
}

void BGE::Game::provide(std::shared_ptr<TextureService> textureService) {
    textureService_ = textureService;
}

void BGE::Game::initialize() {
    // Create these here because they rely on statics and cannot be created during cxx_global_var_init
    componentService_ = std::make_shared<ComponentService>();
    gameObjectService_ = std::make_shared<GameObjectService>();
    
    // Must call this during initialize since doing it during ctor involves statics from other classes not being
    ComponentService::registerComponents();
    
    renderService_->setComponentService(componentService_);
    
    materialService_->initialize();
    eventService_->initialize();
    animationService_->initialize();
    inputService_->initialize();
    spaceService_->initialize();
    fontService_->initialize();
    logicService_->initialize();
    audioService_->initialize();
    scenePackageService_->initialize();
    renderService_->initialize();
    heartbeatService_->initialize();
}

void BGE::Game::start() {
    getHeartbeatService()->registerListener("Game", std::bind(&Game::update, this, std::placeholders::_1), 0);
 }

void BGE::Game::reset() {
}

void BGE::Game::platformSuspending() {
    Service::platformSuspending();
    
    if (renderService_) {
        renderService_->platformSuspending();
    }
    if (textureService_) {
        textureService_->platformSuspending();
    }
    if (fontService_) {
        fontService_->platformSuspending();
    }
    if (heartbeatService_) {
        heartbeatService_->platformSuspending();
    }
    if (materialService_) {
        materialService_->platformSuspending();
    }
    if (scenePackageService_) {
        scenePackageService_->platformSuspending();
    }
    if (animationService_) {
        animationService_->platformSuspending();
    }
    if (spaceService_) {
        spaceService_->platformSuspending();
    }
    if (inputService_) {
        inputService_->platformSuspending();
    }
    if (logicService_) {
        logicService_->platformSuspending();
    }
    if (eventService_) {
        eventService_->platformSuspending();
    }
}

void BGE::Game::platformResuming() {
    Service::platformResuming();
    
    if (renderService_) {
        renderService_->platformResuming();
    }
    if (textureService_) {
        textureService_->platformResuming();
    }
    if (fontService_) {
        fontService_->platformResuming();
    }
    if (heartbeatService_) {
        heartbeatService_->platformResuming();
    }
    if (materialService_) {
        scenePackageService_->platformResuming();
    }
    if (animationService_) {
        animationService_->platformResuming();
    }
    if (spaceService_) {
        spaceService_->platformResuming();
    }
    if (inputService_) {
        inputService_->platformResuming();
    }
    if (logicService_) {
        logicService_->platformResuming();
    }
    if (eventService_) {
        eventService_->platformResuming();
    }
}

void BGE::Game::pause() {
    Service::pause();
    
    if (renderService_) {
        renderService_->pause();
    }
    if (textureService_) {
        textureService_->pause();
    }
    if (fontService_) {
        fontService_->pause();
    }
    if (heartbeatService_) {
        heartbeatService_->pause();
    }
    if (materialService_) {
        materialService_->pause();
    }
    if (scenePackageService_) {
        scenePackageService_->pause();
    }
    if (animationService_) {
        animationService_->pause();
    }
    if (spaceService_) {
        spaceService_->pause();
    }
    if (inputService_) {
        inputService_->pause();
    }
    if (logicService_) {
        logicService_->pause();
    }
    if (eventService_) {
        eventService_->pause();
    }
}

void BGE::Game::resume() {
    Service::resume();
    
    if (renderService_) {
        renderService_->resume();
    }
    if (textureService_) {
        textureService_->resume();
    }
    if (fontService_) {
        fontService_->resume();
    }
    if (heartbeatService_) {
        heartbeatService_->resume();
    }
    if (materialService_) {
        materialService_->resume();
    }
    if (scenePackageService_) {
        scenePackageService_->resume();
    }
    if (animationService_) {
        animationService_->resume();
    }
    if (spaceService_) {
        spaceService_->resume();
    }
    if (inputService_) {
        inputService_->resume();
    }
    if (logicService_) {
        logicService_->resume();
    }
    if (eventService_) {
        eventService_->resume();
    }
}

void BGE::Game::destroy() {
}

void BGE::Game::garbageCollect() {
    handleServicesLock();
    
    if (renderService_) {
        renderService_->garbageCollect();
    }
    if (textureService_) {
        textureService_->garbageCollect();
    }
    if (fontService_) {
        fontService_->garbageCollect();
    }
    if (heartbeatService_) {
        heartbeatService_->garbageCollect();
    }
    if (materialService_) {
        materialService_->garbageCollect();
    }
    if (scenePackageService_) {
        scenePackageService_->garbageCollect();
    }
    if (animationService_) {
        animationService_->garbageCollect();
    }
    if (spaceService_) {
        spaceService_->garbageCollect();
    }
    if (inputService_) {
        inputService_->garbageCollect();
    }
    if (logicService_) {
        logicService_->garbageCollect();
    }
    if (eventService_) {
        eventService_->garbageCollect();
    }
    if (audioService_) {
        audioService_->garbageCollect();
    }
    if (gameObjectService_) {
        gameObjectService_->garbageCollect();
    }
    if (componentService_) {
        componentService_->garbageCollect();
    }
    
    Service::garbageCollect();
    
    handleServicesUnlock();
}

void BGE::Game::servicesSpaceReset(Space *space) {
    logicService_->spaceReset(space);
    animationService_->spaceReset(space);
    inputService_->spaceReset(space);
    eventService_->spaceReset(space);
}

void BGE::Game::update(double deltaTime) {
    // Input
    inputService_->update(deltaTime);

    // Updating
    logicService_->update(deltaTime);
    animationService_->update(deltaTime);

    // Physics here
    
    // Update transforms
    //updateTransforms();

    // Garbage collect
    garbageCollect();
}

void BGE::Game::updateTransforms() {
    spaceService_->lock();
    auto spaces = spaceService_->getSpaces();
    std::vector<TransformComponent *> xforms;
    
    for (auto &handle : spaces) {
        auto space = spaceService_->getSpace(handle);
        
        // Non visible spaces can still process transforms
        if (space && space->isActive()) {
            space->getTransforms(xforms);
        }
    }
    
    for (auto xform : xforms) {
        xform->updateMatrix();
    }
    spaceService_->unlock();
}

void BGE::Game::updateRootTransforms() {
    spaceService_->lock();
    auto spaces = spaceService_->getSpaces();
    std::vector<TransformComponent *> xforms;
    
    for (auto &handle : spaces) {
        auto space = spaceService_->getSpace(handle);
        
        // Non visible spaces can still process transforms
        if (space && space->isActive()) {
            space->getRootTransforms(xforms);
        }
    }
    
    for (auto xform : xforms) {
        xform->updateMatrixAndChildren();
    }
    
    spaceService_->unlock();
}


void BGE::Game::outputResourceUsage() const {
    uint32_t numTabs = 0;
    
    printf("\n-------- Resource Usage START --------\n");
    BGE::Game::outputValue(numTabs, "Num GameObject: %u\n", gameObjectService_->numGameObjects());
    BGE::Game::outputValue(numTabs, "Num Component: %u\n", componentService_->totalNumComponents());
    componentService_->outputResourceBreakdown(numTabs + 1);
    BGE::Game::outputValue(numTabs, "Num Space: %u\n", spaceService_->numSpaces());
    spaceService_->outputResourceBreakdown(numTabs + 1);
    BGE::Game::outputValue(numTabs, "Num ScenePackage: %u\n", scenePackageService_->numScenePackages());
    BGE::Game::outputValue(numTabs, "Num Material: %u\n", materialService_->numMaterials());
    BGE::Game::outputValue(numTabs, "Num Font: %u\n", fontService_->numFonts());
    BGE::Game::outputValue(numTabs, "Num TextureAtlas: %u\n", textureService_->numTextureAtlases());
    BGE::Game::outputValue(numTabs, "Num Texture: %u\n", textureService_->numTextures());
    BGE::Game::outputValue(numTabs,"Num Texture (sub): %u\n", textureService_->numSubTextures());
    printf("-------- Resource Usage END --------\n\n");
    
}

void BGE::Game::setPaused(bool paused) {
    paused_ = paused;
}

void BGE::Game::outputResourceBreakdown(uint32_t numTabs) const {
    printf("\n-------- Resource Breakdown START --------\n");
    assert(gameObjectService_->numGameObjects() == gameObjectService_->numUsedHandles());
    BGE::Game::outputValue(numTabs, "Num GameObject: %u/%u/%u (%u resizes)\n", gameObjectService_->numGameObjects(), gameObjectService_->maxHandlesAllocated(), gameObjectService_->maxHandles(), gameObjectService_->Service::numHandleResizes());
    BGE::Game::outputValue(numTabs, "Num Component: %u/%u/%u\n", componentService_->totalNumComponents(), componentService_->totalMaxHandlesAllocated(), componentService_->totalMaxHandles());
    componentService_->outputResourceBreakdown(numTabs + 1);
    BGE::Game::outputValue(numTabs, "Num Space: %u/%u/%u (%u resizes)\n", spaceService_->numSpaces(), spaceService_->maxHandlesAllocated(), spaceService_->maxHandles(), spaceService_->numHandleResizes());
    spaceService_->outputResourceBreakdown(numTabs + 1);
    BGE::Game::outputValue(numTabs, "Num ScenePackage: %u/%u/%u (%u resizes)\n", scenePackageService_->numScenePackages(), scenePackageService_->maxHandlesAllocated(), scenePackageService_->maxHandles(), scenePackageService_->numHandleResizes());
    scenePackageService_->outputResourceBreakdown(numTabs + 1);
    BGE::Game::outputValue(numTabs, "Num Material: %u/%u/%u (%u resizes)\n", materialService_->numMaterials(), materialService_->maxHandlesAllocated(), materialService_->maxHandles(), materialService_->numHandleResizes());
    BGE::Game::outputValue(numTabs, "Num Font: %u/%u/%u (%u resizes)\n", fontService_->numFonts(), fontService_->maxHandlesAllocated(), fontService_->maxHandles(), fontService_->numHandleResizes());
    BGE::Game::outputValue(numTabs, "Num TextureAtlas: %u/%u/%u (%u resizes)\n", textureService_->numTextureAtlases(), textureService_->maxTextureAtlasHandlesAllocated(), textureService_->maxTextureAtlasHandles(), textureService_->numTextureAtlasHandleResizes());
    BGE::Game::outputValue(numTabs, "Num Texture: %u/%u/%u (%u resizes)\n", textureService_->numTextures(), textureService_->maxTextureHandlesAllocated(), textureService_->maxTextureHandles(), textureService_->numTextureHandleResizes());
    BGE::Game::outputValue(numTabs, "Num Texture (sub): %u\n", textureService_->numSubTextures());
    BGE::Game::outputValue(numTabs, "Num Event Handlers: %u/%u/%u (%u resizes)\n", eventService_->numEventHandlerHandles(), eventService_->maxHandlesAllocated(), eventService_->maxHandles(), eventService_->numHandleResizes());
    printf("-------- Resource Breakdown END --------\n\n");
}

void BGE::Game::outputMemoryUsage() const {
    size_t totalMemory = 0;
    uint32_t numTabs = 0;
    
    // TODO: Shaders
    
    printf("\n-------- Memory Usage START --------\n");
    BGE::Game::outputValue(numTabs, "GameObject: %zd bytes\n", gameObjectService_->totalHandleMemory());
    totalMemory += gameObjectService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Component: %zd bytes\n", componentService_->totalHandleMemory());
    totalMemory += componentService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Space: %zd bytes\n", spaceService_->totalHandleMemory());
    totalMemory += spaceService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "ScenePackage: %zd bytes\n", scenePackageService_->totalHandleMemory());
    totalMemory += scenePackageService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "ScenePackage (FixedArray): %zd bytes\n", scenePackageService_->totalMemory());
    totalMemory += scenePackageService_->totalMemory();
    BGE::Game::outputValue(numTabs, "Material: %zd bytes\n", materialService_->totalHandleMemory());
    totalMemory += materialService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Font: %zd bytes\n", fontService_->totalHandleMemory());
    totalMemory += fontService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Texture/TextureAtlas: %zd bytes\n", textureService_->totalHandleMemory());
    totalMemory += textureService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Texture/TextureAtlas (Textures): %zd bytes\n", textureService_->totalTextureMemory());
    totalMemory += textureService_->totalTextureMemory();
    BGE::Game::outputValue(numTabs, "Render Buffers: %zd bytes\n", renderService_->totalMemory());
    totalMemory += renderService_->totalMemory();
    BGE::Game::outputValue(numTabs, "Total: %zd bytes\n", totalMemory);
    printf("-------- Memory Usage END --------\n\n");
}

void BGE::Game::outputMemoryBreakdown(uint32_t numTabs) const {
    size_t totalMemory = 0;
    
    // TODO: Shaders
    
    printf("\n-------- Memory Breakdown START --------\n");
    BGE::Game::outputValue(numTabs, "GameObject (sizeof=%zd): %zd/%zd bytes\n", sizeof(GameObject), gameObjectService_->usedHandleMemory(), gameObjectService_->totalHandleMemory());
    totalMemory += gameObjectService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Component: %zd/%zd bytes\n", componentService_->usedHandleMemory(), componentService_->totalHandleMemory());
    totalMemory += componentService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Space (sizeof=%zd): %zd/%zd bytes\n", sizeof(Space), spaceService_->unusedHandleMemory(), spaceService_->totalHandleMemory());
    totalMemory += spaceService_->totalHandleMemory();
    spaceService_->outputMemoryBreakdown(numTabs + 1);
    BGE::Game::outputValue(numTabs, "ScenePackage (sizeof=%zd): %zd/%zd bytes\n", sizeof(ScenePackage), scenePackageService_->unusedHandleMemory(), scenePackageService_->totalHandleMemory());
    totalMemory += scenePackageService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "ScenePackage (FixedArray): %zd bytes\n", scenePackageService_->totalMemory());
    totalMemory += scenePackageService_->totalMemory();
    scenePackageService_->outputMemoryBreakdown(numTabs+1);
    BGE::Game::outputValue(numTabs, "Material (sizeof=%zd): %zd/%zd bytes\n", sizeof(Material), materialService_->unusedHandleMemory(), materialService_->totalHandleMemory());
    totalMemory += materialService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Font (sizeof=%zd): %zd/%zd bytes\n", sizeof(Font), fontService_->unusedHandleMemory(), fontService_->totalHandleMemory());
    totalMemory += fontService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Texture/TextureAtlas (sizeof=%zd/%zd): %zd/%zd bytes\n", sizeof(Texture), sizeof(TextureAtlas), textureService_->unusedHandleMemory(), textureService_->totalHandleMemory());
    totalMemory += textureService_->totalHandleMemory();
    BGE::Game::outputValue(numTabs, "Texture/TextureAtlas (Textures): %zd bytes\n", textureService_->totalTextureMemory());
    textureService_->outputMemoryBreakdown(numTabs + 1);
    totalMemory += textureService_->totalTextureMemory();
    BGE::Game::outputValue(numTabs, "Render Buffers: %zd bytes\n", renderService_->totalMemory());
    totalMemory += renderService_->totalMemory();
    BGE::Game::outputValue(numTabs, "Total: %zd bytes\n", totalMemory);
    printf("-------- Memory Breakdown END --------\n\n");
}
