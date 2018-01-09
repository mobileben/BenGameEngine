//
//  Profiling.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 1/6/18.
//  Copyright © 2018 2n Productions. All rights reserved.
//

#include "Profiling.h"

#include <limits>

BGE::profiling::FrameRateCalculator::FrameRateCalculator() : fps(0), frames(0), index(0), sum(0) {
    samples = std::vector<int64_t>(NumSamples, 0);
    lastTimestamp = EpochTime::timeInMilliSec();
}

BGE::profiling::FrameRateCalculator::~FrameRateCalculator() {
}

void BGE::profiling::FrameRateCalculator::reset() {
    fps = 0;
    frames = 0;
    index = 0;
    sum = 0;

    lastTimestamp = EpochTime::timeInMilliSec();
    for (auto i=0;i<samples.size();++i) {
        samples[i] = 0;
    }
}

double BGE::profiling::FrameRateCalculator::nextFrame() {
    auto now = EpochTime::timeInMilliSec();
    int64_t delta;
    if (now >= lastTimestamp) {
        delta = now - lastTimestamp;
    } else {
        // Wrapping of timer
        if (now >= 0) {
            auto upper = std::numeric_limits<int64_t>::max();
            delta = upper - lastTimestamp;
            delta += now;
        } else {
            // Otherwise ignore
            delta = 0;
        }
    }
    lastTimestamp = now;
    sum -= samples[index];
    sum += delta;
    samples[index] = delta;
    currentFrameTime = delta;
    
    if (sum > 0) {
        auto size = samples.size();
        ++frames;
        ++index;
        if (index >= size) {
            index = 0;
        }
        // 1000 is convert milliseconds to seconds
        double avgTick = static_cast<double>(sum) / static_cast<double>(size * 1000);
        fps = 1.0 / avgTick;
    } else {
        fps = 0;
    }
    return fps;
}
