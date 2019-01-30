//
//  Profiling.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 1/6/18.
//  Copyright © 2018 2n Productions. All rights reserved.
//

#ifndef Profiling_hpp
#define Profiling_hpp

#include <stdio.h>
#include <chrono>
#include <vector>

namespace BGE {
    namespace profiling {
        namespace int_type {
            const int64_t MillisecondsInSeconds       = 1000;
            const int64_t MicrosecondsInSeconds       = 1000000;
            const int64_t MicrosecondsInMilliseconds  = 1000;
            const int64_t NanosecondsInSeconds        = 1000000000;
            const int64_t NanosecondsInMilliseconds   = 1000000;
            const int64_t NanosecondsInMicroSeconds   = 1000;

            inline int64_t millisecondsToSeconds(int64_t milli) { return milli / MillisecondsInSeconds; }
            inline int64_t millisecondsToMicroseconds(int64_t milli) { return milli * MicrosecondsInMilliseconds; }
            inline int64_t secondsToMilliseconds(int64_t sec) { return sec * MillisecondsInSeconds; }
            inline int64_t microsecondsToSeconds(int64_t micro) { return micro / MicrosecondsInSeconds; }
            inline int64_t microsecondsToMilliseconds(int64_t micro) { return micro / MicrosecondsInMilliseconds; }
            inline int64_t secondsToMicroseconds(int64_t sec) { return sec * MicrosecondsInSeconds; }
            inline int64_t nanosecondsToSeconds(int64_t nano) { return nano / NanosecondsInSeconds; }
            inline int64_t nanosecondsToMillieconds(int64_t nano) { return nano / NanosecondsInMilliseconds; }
            inline int64_t nanosecondsToMicroseconds(int64_t nano) { return nano / NanosecondsInMicroSeconds; }
            inline int64_t secondsToNanoseconds(int64_t sec) { return sec * NanosecondsInSeconds; }
        }

        namespace double_type {
            const double MillisecondsInSeconds       = 1000;
            const double MicrosecondsInSeconds       = 1000000;
            const double MicrosecondsInMilliseconds  = 1000;
            const double NanosecondsInSeconds        = 1000000000;
            const double NanosecondsInMilliseconds   = 1000000;
            const double NanosecondsInMicroSeconds   = 1000;

            inline double millisecondsToSeconds(double milli) { return milli / MillisecondsInSeconds; }
            inline double millisecondsToMicroseconds(double milli) { return milli * MicrosecondsInMilliseconds; }
            inline double secondsToMilliseconds(double sec) { return sec * MillisecondsInSeconds; }
            inline double microsecondsToSeconds(double micro) { return micro / MicrosecondsInSeconds; }
            inline double microsecondsToMilliseconds(double micro) { return micro / MicrosecondsInMilliseconds; }
            inline double secondsToMicroseconds(double sec) { return sec * MicrosecondsInSeconds; }
            inline double nanosecondsToSeconds(double nano) { return nano / NanosecondsInSeconds; }
            inline double nanosecondsToMillieconds(double nano) { return nano / NanosecondsInMilliseconds; }
            inline double nanosecondsToMicroseconds(double nano) { return nano / NanosecondsInMicroSeconds; }
            inline double secondsToNanoseconds(double sec) { return sec * NanosecondsInSeconds; }
        }


        class EpochTime {
        public:
            static int64_t timeInSec() {
                const auto epoch   = std::chrono::high_resolution_clock::now().time_since_epoch();
                const auto time = std::chrono::duration_cast<std::chrono::seconds>(epoch);
                return time.count();
            }

            static int64_t timeInMilliSec() {
                const auto epoch   = std::chrono::high_resolution_clock::now().time_since_epoch();
                const auto time = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
                return time.count();
            }

            static int64_t timeInMicroSec() {
                const auto epoch   = std::chrono::high_resolution_clock::now().time_since_epoch();
                const auto time = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
                return time.count();
            }

            static int64_t timeInNanoSec() {
                const auto epoch   = std::chrono::high_resolution_clock::now().time_since_epoch();
                const auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);
                return time.count();
            }
        };

        struct FrameRateCalculator {
            const int32_t NumSamples = 64;

            double getFps(void) const { return fps; }
            int64_t frameCount(void) const { return frames; }
            int64_t getCurrentFrameTime(void) const { return currentFrameTime; }

            FrameRateCalculator();
            ~FrameRateCalculator();

            void reset();
            double nextFrame();

        private:
            double                      fps;
            int64_t                     frames;
            int64_t                     currentFrameTime;

            int64_t                     lastTimestamp;
            std::vector<int64_t>        samples;
            uint32_t                    index;
            int64_t                     sum;
        };
    }
}

#endif /* Profiling_hpp */
