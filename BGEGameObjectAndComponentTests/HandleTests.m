//
//  HandleTests.m
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#import <XCTest/XCTest.h>
#include "Handle.h"
#include "HandleService.h"
#include "Object.h"

@interface HandleTests : XCTestCase

@end

@implementation HandleTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testHandle {
    struct tagHandle{};
    BGE::Handle<tagHandle> handle;

    XCTAssertEqual(handle.getIndex(), 0, @"Index should be 0");
    XCTAssertEqual(handle.getMagic(), 0, @"Magic should be 0");
    XCTAssertEqual(handle.getHandle(), 0, @"Handle should be 0");
    XCTAssertTrue(handle.isNull(), @"Handle not null");
    
    uint32_t index = 10;
    
    handle.init(index);
    
    XCTAssertEqual(handle.getIndex(), index, @"Index should be 10");
    XCTAssertEqual(handle.getMagic(), 1, @"Magic should be 1");
    XCTAssertEqual(handle.getHandle(), (handle.getMagic() << BGE::Handle<tagHandle>::NUM_INDEX_BITS) | index, @"Handle should be 0");
}

- (void)testHandleService {
    struct tagHandle{};
    typedef BGE::Handle<tagHandle> HandleType;
    
    struct Data : public BGE::Object {
        uint32_t val1;
        uint32_t val2;
        Data(uint32_t id) : Object(id) {}
        Data(uint32_t val1, uint32_t val2) : val1(val1), val2(val2) {}
    };
    
    // Reserve 0
    try {
        BGE::HandleService<Data, HandleType> hService(0, 5);
        XCTFail(@"Reserve == 0");
    } catch (const std::exception& e) {
    }
    
    // Max limit < reserve
    try {
        BGE::HandleService<Data, HandleType> hService(10, 5);
        XCTFail(@"Limit < reserve");
    } catch (const std::exception& e) {
    }
    
    // Reserve == max limit
    {
        BGE::HandleService<Data, HandleType> hService(10, 10);
    }
    // Max limit == 0
    {
        BGE::HandleService<Data, HandleType> hService(10, 0);
    }
    
    // Test capacity with no max limit
    {
        uint32_t reserve = 10;
        
        BGE::HandleService<Data, HandleType> hService(reserve, 0);
        
        // Capacity should be 10
        XCTAssertEqual(hService.capacity(), reserve, @"Capacity should be %d", reserve);
        
        // Now allocate 10, capacity should be 10
        std::vector<HandleType> handles;
        
        for (auto i=0;i<reserve;i++) {
            HandleType handle;
            
            hService.allocate(handle);
            
            handles.push_back(handle);
        }

        // Capacity should be 10
        XCTAssertEqual(hService.capacity(), reserve, @"Capacity should be %d", reserve);
        
        // Allocate one more
        HandleType handle;
        
        hService.allocate(handle);
        
        handles.push_back(handle);
        
        // Capacity should be > 10
        XCTAssertGreaterThan(hService.capacity(), reserve, @"Capacity should be > %d", reserve);
    }
    
    // Test capacity with max limit
    {
        uint32_t reserve = 10;
        
        BGE::HandleService<Data, HandleType> hService(reserve, reserve);
        
        // Capacity should be 10
        XCTAssertEqual(hService.capacity(), reserve, @"Capacity should be %d", reserve);
        
        // Now allocate 10, capacity should be 10
        std::vector<HandleType> handles;
        
        for (auto i=0;i<reserve;i++) {
            HandleType handle;
            
            hService.allocate(handle);
            
            handles.push_back(handle);
        }
        
        // Capacity should be 10
        XCTAssertEqual(hService.capacity(), reserve, @"Capacity should be %d", reserve);
        
        // Allocate one more
        HandleType handle;
        
        XCTAssertEqual(hService.allocate(handle), nullptr, @"Pointer should be nil");
        XCTAssertTrue(handle.isNull(), @"Handle should be null");
        
        // Capacity should be 10
        XCTAssertEqual(hService.capacity(), reserve, @"Capacity should be %d", reserve);
    }
    
    {
        uint32_t reserve = 20;
        BGE::HandleService<Data, HandleType> hService(reserve, reserve);
        Data data[] = {
            {
                0,
                100
            },
            {
                1,
                101
            },
            {
                2,
                102
            },
            {
                3,
                103
            },
            {
                4,
                104
            },
            {
                5,
                105
            },
            {
                6,
                106
            },
            {
                7,
                107
            },
            {
                8,
                108
            },
            {
                9,
                109
            },
            {
                10,
                110
            },
            {
                11,
                111
            },
            {
                12,
                112
            },
            {
                13,
                113
            },
            {
                14,
                114
            },
            {
                15,
                115
            },
            {
                16,
                116
            },
            {
                17,
                117
            },
            {
                18,
                118
            },
            {
                19,
                119
            },
        };
        
        std::vector<HandleType> handles;
        
        // Now test data integrity
        for (auto i=0;i<reserve;i++) {
            HandleType handle;
            Data* dataPtr = hService.allocate(handle);
            
            XCTAssertNotEqual(dataPtr, nullptr, @"dataPtr should not be nullptr");
            XCTAssertFalse(handle.isNull(), @"handle should not be null");
            XCTAssertEqual(handle.getIndex(), i, @"handle index should equal i");
            XCTAssertEqual(hService.numUsedHandles(), i+1, @"Num used handles should match!");
            *dataPtr = data[i];
            handles.push_back(handle);
        }
        
        // Validate values
        for (auto i=0;i<reserve;i++) {
            HandleType handle = handles[i];

            XCTAssertFalse(handle.isNull(), @"handle should not be null");
            
            Data* dataPtr = hService.dereference(handle);
            
            XCTAssertNotEqual(dataPtr, nullptr, @"dataPtr should not be nullptr");
            
            XCTAssertEqual(dataPtr->val1, data[i].val1, @"%d val1 values not equal!", i);
            XCTAssertEqual(dataPtr->val2, data[i].val2, @"%d val2 values not equal!", i);
        }
        
        
        // Finally test release and allocating new items
        
        // We will free index 7, index 17, index 4, and index 12 (in that order)
        HandleType oldHandle7 = handles[7];
        HandleType oldHandle17 = handles[17];
        HandleType oldHandle4 = handles[4];
        HandleType oldHandle12 = handles[12];
        HandleType newHandle7;
        HandleType newHandle17;
        HandleType newHandle4;
        HandleType newHandle12;
        
        // Now free each one
        hService.release(oldHandle7);
        XCTAssertEqual(hService.numUsedHandles(), reserve - 1, @"Num used handles wrong");
        XCTAssertEqual(hService.numFreeSlots(), 1, @"Num free slots wrong");
        
        hService.release(oldHandle17);
        XCTAssertEqual(hService.numUsedHandles(), reserve - 2, @"Num used handles wrong");
        XCTAssertEqual(hService.numFreeSlots(), 2, @"Num free slots wrong");
        
        hService.release(oldHandle4);
        XCTAssertEqual(hService.numUsedHandles(), reserve - 3, @"Num used handles wrong");
        XCTAssertEqual(hService.numFreeSlots(), 3, @"Num free slots wrong");
        
        hService.release(oldHandle12);
        XCTAssertEqual(hService.numUsedHandles(), reserve - 4, @"Num used handles wrong");
        XCTAssertEqual(hService.numFreeSlots(), 4, @"Num free slots wrong");
        
        // Double check the order of the free slots
        XCTAssertEqual(hService.freeIndexAtIndex(0), 7, @"Free index is wrong");
        XCTAssertEqual(hService.freeIndexAtIndex(1), 17, @"Free index is wrong");
        XCTAssertEqual(hService.freeIndexAtIndex(2), 4, @"Free index is wrong");
        XCTAssertEqual(hService.freeIndexAtIndex(3), 12, @"Free index is wrong");

        // Dereference old handles, they should result in null
        XCTAssertEqual(hService.dereference(oldHandle7), nullptr, @"Handle should dereference to null");
        XCTAssertEqual(hService.dereference(oldHandle17), nullptr, @"Handle should dereference to null");
        XCTAssertEqual(hService.dereference(oldHandle4), nullptr, @"Handle should dereference to null");
        XCTAssertEqual(hService.dereference(oldHandle12), nullptr, @"Handle should dereference to null");

        // Make sure all other handles still work
        for (auto i=0;i<reserve;i++) {
            HandleType handle = handles[i];
            
            if (handle == oldHandle7 || handle == oldHandle17 || handle == oldHandle4 || handle == oldHandle12) {
                
            } else {
                Data *d = hService.dereference(handle);
                
                XCTAssertNotEqual(d, nullptr, @"Data should not be null");
                XCTAssertEqual(d->val1, data[i].val1, @"%d val1 values not equal!", i);
                XCTAssertEqual(d->val2, data[i].val2, @"%d val2 values not equal!", i);
            }
        }

        // Make sure the magic is 0
        for (auto i=0;i<reserve;i++) {
            HandleType handle = handles[i];
            
            if (handle == oldHandle7 || handle == oldHandle17 || handle == oldHandle4 || handle == oldHandle12) {
                XCTAssertEqual(hService.magicAtIndex(i), 0, @"Magic should be 0");
            } else {
                XCTAssertNotEqual(hService.magicAtIndex(i), 0, @"Magic should not be 0");
            }
        }
        
        // Now create new handles
        Data *newData = hService.allocate(newHandle12);
        
        // new Data should be old data
        XCTAssertEqual(newData->val1, data[12].val1, @"%d val1 values not equal!", 12);
        XCTAssertEqual(newData->val2, data[12].val2, @"%d val2 values not equal!", 12);
        
        // This should be index 12
        XCTAssertEqual(newHandle12.getIndex(), 12, @"Handle index is wrong");
        XCTAssertEqual(hService.numFreeSlots(), 3, @"Num free slots is wrong");
        XCTAssertEqual(hService.numUsedHandles(), reserve - 3, @"Num used handles is wrong");
        
        // Now compare handles
        XCTAssertFalse(oldHandle12 == newHandle12, @"Handles should not match");
        XCTAssertTrue(oldHandle12.getIndex() == newHandle12.getIndex(), @"Handles getIndex should not match");
        XCTAssertFalse(oldHandle12.getMagic() == newHandle12.getMagic(), @"Handles getMagic should not match");
        XCTAssertFalse(oldHandle12.getHandle() == newHandle12.getHandle(), @"Handles getHandle should not match");
        
        // Make sure dereference works
        newData = hService.dereference(newHandle12);
        XCTAssertNotEqual(newData, nullptr, @"New handle should not be null");
        XCTAssertEqual(newData->val1, data[12].val1, @"%d val1 values not equal!", 12);
        XCTAssertEqual(newData->val2, data[12].val2, @"%d val2 values not equal!", 12);
        
        newData = hService.allocate(newHandle4);
        
        // new Data should be old data
        XCTAssertEqual(newData->val1, data[4].val1, @"%d val1 values not equal!", 4);
        XCTAssertEqual(newData->val2, data[4].val2, @"%d val2 values not equal!", 4);
        
        // This should be index 4
        XCTAssertEqual(newHandle4.getIndex(), 4, @"Handle index is wrong");
        XCTAssertEqual(hService.numFreeSlots(), 2, @"Num free slots is wrong");
        XCTAssertEqual(hService.numUsedHandles(), reserve - 2, @"Num used handles is wrong");
        
        // Now compare handles
        XCTAssertFalse(oldHandle4 == newHandle4, @"Handles should not match");
        XCTAssertTrue(oldHandle4.getIndex() == newHandle4.getIndex(), @"Handles getIndex should not match");
        XCTAssertFalse(oldHandle4.getMagic() == newHandle4.getMagic(), @"Handles getMagic should not match");
        XCTAssertFalse(oldHandle4.getHandle() == newHandle4.getHandle(), @"Handles getHandle should not match");
        
        // Make sure dereference works
        newData = hService.dereference(newHandle4);
        XCTAssertNotEqual(newData, nullptr, @"New handle should not be null");
        XCTAssertEqual(newData->val1, data[4].val1, @"%d val1 values not equal!", 4);
        XCTAssertEqual(newData->val2, data[4].val2, @"%d val2 values not equal!", 4);

        newData = hService.allocate(newHandle17);
        
        // new Data should be old data
        XCTAssertEqual(newData->val1, data[17].val1, @"%d val1 values not equal!", 17);
        XCTAssertEqual(newData->val2, data[17].val2, @"%d val2 values not equal!", 17);
        
        // This should be index 4
        XCTAssertEqual(newHandle17.getIndex(), 17, @"Handle index is wrong");
        XCTAssertEqual(hService.numFreeSlots(), 1, @"Num free slots is wrong");
        XCTAssertEqual(hService.numUsedHandles(), reserve - 1, @"Num used handles is wrong");
        
        // Now compare handles
        XCTAssertFalse(oldHandle17 == newHandle17, @"Handles should not match");
        XCTAssertTrue(oldHandle17.getIndex() == newHandle17.getIndex(), @"Handles getIndex should not match");
        XCTAssertFalse(oldHandle17.getMagic() == newHandle17.getMagic(), @"Handles getMagic should not match");
        XCTAssertFalse(oldHandle17.getHandle() == newHandle17.getHandle(), @"Handles getHandle should not match");
        
        // Make sure dereference works
        newData = hService.dereference(newHandle17);
        XCTAssertNotEqual(newData, nullptr, @"New handle should not be null");
        XCTAssertEqual(newData->val1, data[17].val1, @"%d val1 values not equal!", 17);
        XCTAssertEqual(newData->val2, data[17].val2, @"%d val2 values not equal!", 17);

        newData = hService.allocate(newHandle7);
        
        // new Data should be old data
        XCTAssertEqual(newData->val1, data[7].val1, @"%d val1 values not equal!", 7);
        XCTAssertEqual(newData->val2, data[7].val2, @"%d val2 values not equal!", 7);
        
        // This should be index 4
        XCTAssertEqual(newHandle7.getIndex(), 7, @"Handle index is wrong");
        XCTAssertEqual(hService.numFreeSlots(), 0, @"Num free slots is wrong");
        XCTAssertEqual(hService.numUsedHandles(), reserve - 0, @"Num used handles is wrong");
        
        // Now compare handles
        XCTAssertFalse(oldHandle7 == newHandle7, @"Handles should not match");
        XCTAssertTrue(oldHandle7.getIndex() == newHandle7.getIndex(), @"Handles getIndex should not match");
        XCTAssertFalse(oldHandle7.getMagic() == newHandle7.getMagic(), @"Handles getMagic should not match");
        XCTAssertFalse(oldHandle7.getHandle() == newHandle7.getHandle(), @"Handles getHandle should not match");
        
        // Make sure dereference works
        newData = hService.dereference(newHandle7);
        XCTAssertNotEqual(newData, nullptr, @"New handle should not be null");
        XCTAssertEqual(newData->val1, data[7].val1, @"%d val1 values not equal!", 7);
        XCTAssertEqual(newData->val2, data[7].val2, @"%d val2 values not equal!", 7);

        // Now delete all the handles
        for (auto i=0;i<reserve;i++) {
            HandleType handle = handles[i];
            
            switch(i) {
                case 4:
                    handle = newHandle4;
                    break;
                    
                case 12:
                    handle = newHandle12;
                    break;
                    
                case 7:
                    handle = newHandle7;
                    break;
                    
                case 17:
                    handle = newHandle17;
                    break;
                    
                default:
                    handle = handles[i];
            }
            
            hService.release(handle);
            XCTAssertEqual(hService.numFreeSlots(), i + 1, @"Num free slots is wrong");
            XCTAssertEqual(hService.numUsedHandles(), reserve - i - 1, @"Num used handles is wrong");
            
            XCTAssertEqual(hService.dereference(handle), nullptr, @"Handle should dereference to null");
        }
        
        XCTAssertEqual(hService.numFreeSlots(), reserve, @"Num free slots is wrong");
        XCTAssertEqual(hService.numUsedHandles(), 0, @"Num used handles is wrong");
    }
}

@end
