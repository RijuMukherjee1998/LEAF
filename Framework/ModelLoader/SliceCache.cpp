//
// Created by riju on 7/5/25.
//

#include "../headers/SliceCache.h"

#include "../headers/Constants.h"

// Define the static member variables
std::mutex SliceCache::mut;
SliceCache* SliceCache::instance = nullptr;


SliceCache* SliceCache::getInstance() {
    std::lock_guard<std::mutex> lock(mut);
    if (instance == nullptr) {
        instance = new SliceCache();
    }
    return instance;
}
SliceCache::SliceCache() : m_sliceDirectory(ModelOps::SliceDirectory::getInstance()), mLogger(Utils::Logger::getInstance()) {
    m_sliceCache = std::make_unique<std::unordered_map<size_t, std::tuple<std::shared_ptr<Slice>, uint32_t, int>>>();
}
std::shared_ptr<Slice> SliceCache::getSlice(size_t slice_hash) {
    if (m_sliceCache->find(slice_hash) != m_sliceCache->end()) {
        ++std::get<2>(m_sliceCache->at(slice_hash));
        return std::get<0>(m_sliceCache->at(slice_hash));
    }
    else {
        //search the slice directory
        if (m_sliceDirectory->searchSlice(slice_hash).exists == true) {
            auto entry = m_sliceDirectory->searchSlice(slice_hash);
            uint32_t refCount = entry.refCount;
            std::shared_ptr<Slice> slice = DiskOps::DiskManager::getInstance(SLICE_DATA_FILE_PATH)->readSliceFromDisk(entry.sliceOffset, entry.sliceSize);
            m_sliceCache->emplace(slice_hash, std::make_tuple(slice, refCount, 1));
            return slice;
        }
    }
    mLogger->logError({"Slice not found ... Aborting can't run", std::to_string(slice_hash)});
    return nullptr;
}
