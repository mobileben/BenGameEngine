//
//  BGERenderViewIOS.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/7/18.
//  Copyright © 2018 2n Productions. All rights reserved.
//

#include "BGERenderViewIOS.h"

BGE::BGERenderViewIOS::BGERenderViewIOS(BGEView *view) : RenderView(view.frame.origin.x, view.frame.origin.y, view.frame.size.width, view.frame.size.height), view(view) {
}
