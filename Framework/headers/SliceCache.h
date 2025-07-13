//
// Created by riju on 7/5/25.
//

#ifndef SLICECACHE_H
#define SLICECACHE_H
#include <memory>
#include <unordered_map>
#include <list>
#include "Slice.h"
#include "SliceDirectory.h"
#include "DiskManager.h"
#include "Logger.h"

class RefCountCacheAlgorithm {

};

class SliceCache {
private:
    std::unique_ptr<std::unordered_map<size_t, std::tuple<std::shared_ptr<Slice>, uint32_t, int>>> m_sliceCache;
    std::mutex m_write_mut;
    std::mutex m_read_mut;
    static SliceCache* instance;
    ModelOps::SliceDirectory* m_sliceDirectory;
    Utils::Logger* mLogger = nullptr;
    SliceCache();
public:
    static std::mutex mut;
    static SliceCache* getInstance();
    std::shared_ptr<Slice> getSlice(size_t slice_hash);
};



#endif //SLICECACHE_H