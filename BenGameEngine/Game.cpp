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

void BGE::Game::outputValue(uint32_t numTabs, std::string format, va_list args) {
    std::string results = format;
    
    for (auto i=0;i<numTabs;i++) {
        printf("\t");
    }
    
    std::vector<AnyToString> vec;
    
    const char *str = format.c_str();
    bool inFormatSpecifier = false;
    char formatSpecifier[3]; // Indicates current format specifier
    auto formatSpecifierIndex = 0;
    
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

BGE::Game::Game()
{
    materialService_ = std::make_shared<MaterialService>();
    heartbeatService_ = std::make_shared<HeartbeatService>();
    scenePackageService_ = std::make_shared<ScenePackageService>();
    animationService_ = std::make_shared<AnimationService>();
    spaceService_ = std::make_shared<SpaceService>();
    fontService_ = std::make_shared<FontService>();
    inputService_ = std::make_shared<InputService>();
    eventService_ = std::make_shared<EventService>();
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
    
    getHeartbeatService()->registerListener("Game", std::bind(&Game::update, this, std::placeholders::_1), 0);
}

void BGE::Game::reset() {
}

void BGE::Game::enteringBackground() {
}

void BGE::Game::enteringForeground() {
}

void BGE::Game::pause() {
}

void BGE::Game::resume() {
}

void BGE::Game::destroy() {
}

void BGE::Game::update(double deltaTime) {
    NSLog(@"delta time %f", deltaTime);
    inputService_->process();
    animationService_->update(deltaTime);
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
    BGE::Game::outputValue(numTabs, "Num Material: %u/%u/%u (%u resizes)\n", materialService_->numMaterials(), materialService_->maxHandlesAllocated(), materialService_->maxHandles(), materialService_->numMaterials());
    BGE::Game::outputValue(numTabs, "Num Font: %u/%u/%u (%u resizes)\n", fontService_->numFonts(), fontService_->maxHandlesAllocated(), fontService_->maxHandles(), fontService_->numHandleResizes());
    BGE::Game::outputValue(numTabs, "Num TextureAtlas: %u/%u/%u (%u resizes)\n", textureService_->numTextureAtlases(), textureService_->maxTextureAtlasHandlesAllocated(), textureService_->maxTextureAtlasHandles(), textureService_->numTextureAtlasHandleResizes());
    BGE::Game::outputValue(numTabs, "Num Texture: %u/%u/%u (%u resizes)\n", textureService_->numTextures(), textureService_->maxTextureHandlesAllocated(), textureService_->maxTextureHandles(), textureService_->numTextureHandleResizes());
    BGE::Game::outputValue(numTabs, "Num Texture (sub): %u\n", textureService_->numSubTextures());
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
