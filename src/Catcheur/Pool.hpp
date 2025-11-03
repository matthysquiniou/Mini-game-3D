#pragma once

#include <vector>
#include <memory>
#include <Geometries/Geometries.h>

template<typename T>
class Pool {
public:
    T* acquire() {
        if (!freeList.empty()) {
            T* obj = freeList.back();
            freeList.pop_back();
            return obj;
        }
        pool.emplace_back(std::make_unique<T>());
        return pool.back().get();
    }

    sr::Geometry* acquireCustomDonut(float32 majorRadius, float32 minorRadius, uint32 majorSegments, uint32 minorSegments) {
        if (!freeList.empty()) {
            auto* obj = freeList.back();
            freeList.pop_back();
            return obj;
        }
        pool.emplace_back(std::make_unique<sr::Geometry>(
            gce::GeometryFactory::CreateDonutGeo(majorRadius, minorRadius, majorSegments, minorSegments)
        ));
        return pool.back().get();
    }

    void release(T* obj) {
        freeList.push_back(obj);
    }

private:
    std::vector<std::unique_ptr<T>> pool;
    std::vector<T*> freeList;
};