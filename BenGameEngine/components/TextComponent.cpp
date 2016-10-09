//
//  TextComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/8/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextComponent.h"
#include "Game.h"

uint32_t BGE::TextComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::TextComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::TextComponent::type_index_ = typeid(BGE::TextComponent);

BGE::TextComponent::TextComponent() : RenderComponent() {
}

void BGE::TextComponent::materialsUpdated() {
}

void BGE::TextComponent::setTextReference(const TextReference& textRef) {
    color_ = textRef.color;
    horizAlignment_ = textRef.alignment;
    vertAlignment_ = FontVerticalAlignment::Center;
    leading_ = textRef.leading;
    multiline_ = textRef.multiline;
    fontHandle_ = textRef.fontHandle;
    boundsWidth_ = textRef.width;
    boundsHeight_ = textRef.height;
    
    setText(textRef.text);
}

void BGE::TextComponent::setText(std::string const &text) {
    text_ = text;

    multiText_.clear();
    textY_.clear();
    
    buildLines();
    
    updateBoundingBox();
}

void BGE::TextComponent::setFont(FontHandle fontHandle) {
    fontHandle_ = fontHandle;
    
    updateBoundingBox();
}

float BGE::TextComponent::getWidth(bool minimum) {
    auto font = Game::getInstance()->getFontService()->getFont(fontHandle_);
    float width = 0;
    
    if (font) {
        width = font->getStringWidth(text_);
    }
    
    return width;
}

void BGE::TextComponent::getWidthHeight(float &width, float &height) {
    auto font = Game::getInstance()->getFontService()->getFont(fontHandle_);
    
    if (font) {
        width = font->getStringWidth(text_);
        height = font->getHeight();
    } else {
        width = height = 0;
    }
}

void BGE::TextComponent::setMultiline(bool multi) {
    multiline_ = multi;
    
    // Set the test just in case we need to change multitext
    setText(text_);
}

void BGE::TextComponent::updateBoundingBox() {
    float width, height;
    auto gameObjHandle = getGameObjectHandle();
    auto gameObj = getSpace()->getGameObject(gameObjHandle);
    auto bbox = gameObj->getComponent<BoundingBoxComponent>();
    
    getWidthHeight(width, height);
    
    // TODO: Need to factor in alignment
    bbox->x = 0;
    bbox->y = 0;
    
    if (multiline_) {
        
    } else {
        bbox->width = width;
        bbox->height = height;
    }
}

void BGE::TextComponent::buildLines() {
    multiText_.clear();
    textY_.clear();

    if (multiline_) {
        auto font = Game::getInstance()->getFontService()->getFont(fontHandle_);

        uint32_t length;
        uint32_t currLineLength;
        BOOL inWord = false;
        uint32_t currWordStart = 0;
        uint32_t lastWordEnd = -1;
        std::string line;
        float maxTextWidth = boundsWidth_;
        bool needNewline = false;
        int32_t nextPosition = 0;
        float lineHeight = 0;

        int32_t rangeStart = 0;
        int32_t rangeLen = 0;

        float substrWidth;

        length = (int32_t) text_.length();
        
        lineHeight = font->getHeight();
        // A leading value of 1 represents 110% fo the lineHeight for the distance.
        lineHeight *= 1.0 + leading_ * 0.1f;

        for (uint32_t i=0;i<length;i++) {
            if (i < nextPosition) {
                continue;
            }
            
            unichar ch = text_.at(i);
            
            currLineLength = (i - rangeStart) + 1;
            
            rangeLen = currLineLength;
            
            line = text_.substr(rangeStart, rangeLen);
            substrWidth = font->getStringWidth(line);
            
            if (substrWidth > maxTextWidth) {
                needNewline = YES;
            }
            
            if (ch == ' ') {
                if (inWord) {
                    lastWordEnd = i - 1;
                }
                
                inWord = NO;
                
                if (needNewline) {
                    // Stay on this position
                    nextPosition = i;
                }
            } else if (ch == '\n' || ch == '\r') {
                if (inWord) {
                    lastWordEnd = i - 1;
                }
                
                // Newline
                inWord = NO;
                needNewline = YES;
                
                // Skip over this character
                nextPosition = i + 1;
            } else {
                // We assume we are in a word
                if (needNewline) {
                    // Let's find out if our string is going to be longer than
                    if (inWord) {
                        int32_t end = length;
                        
                        for (int32_t j=i+1;j<length;j++) {
                            char futureCh = text_.at(j);
                            
                            if (futureCh == ' ' || futureCh == '\n' || futureCh == '\r') {
                                if (j >= i+1) {
                                    end = j;
                                    break;
                                }
                            }
                        }
                        
                        auto currWordRangeStart = currWordStart;
                        auto currWordRangeLen = end - currWordStart;
                        
                        auto currWord = text_.substr(currWordRangeStart, currWordRangeLen);
                        
                        auto currWordLen = font->getStringWidth(currWord);
                        
                        if (currWordLen > maxTextWidth) {
                            // Current word is never going to fit, so just split it here
                            nextPosition = i;
                            currWordStart = nextPosition;
                        } else {
                            nextPosition = currWordStart;
                        }
                    } else {
                        if (i > 0) {
                            nextPosition = i - 1;
                        } else {
                            nextPosition = 0;
                        }
                        
                        currWordStart = i;
                    }
                    
                    if (lastWordEnd < rangeStart) {
                        // Use next position to dictate the end
                        rangeLen = nextPosition - rangeStart;
                    } else {
                        rangeLen = lastWordEnd - rangeStart + 1;
                        
                    }
                    
                    line = text_.substr(rangeStart, rangeLen);
                } else if (!inWord) {
                    // Indicate we are at the start of our word
                    currWordStart = i;
                    inWord = YES;
                }
            }
            
            if (needNewline) {
                multiText_.push_back(line);
                
                rangeStart = nextPosition;
                
                needNewline = NO;
                
                // TODO: Based on alignment trim leading, trailing, or both for white spaces
            }
        }
        
        if (rangeStart < length) {
            // We need to add the very last line
            rangeLen = length - rangeStart;
            
            line = text_.substr(rangeStart, rangeLen);
            
            multiText_.push_back(line);
        }
        
        // Now create all our nodes
        // Get the height
        float height = lineHeight + (multiText_.size() - 1) * lineHeight;
        float y = height / 2 - lineHeight / 2;

        for (auto i=0;i<multiText_.size();++i) {
            textY_.push_back(y);
            y -= lineHeight;
        }
    }
}

