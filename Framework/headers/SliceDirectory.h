//
// Created by Riju Mukherjee on 6/14/25.
//

#ifndef SLICEDIRECTORY_H
#define SLICEDIRECTORY_H

#include <unordered_map>
#include <filesystem>
#include "Logger.h"
#include "Slice.h"
#include "DiskManager.h"

namespace ModelOps {
    struct SDEntry
    {
        size_t hashKey{};
        uint64_t sliceOffset = 0;
        size_t sliceSize = 0;
        bool exists = false;
        uint32_t refCount = 0;
    };
    class SliceDirectory {
    private:
        std::unique_ptr<std::unordered_map<size_t, SDEntry>> mSliceMap;
        DiskOps::DiskManager* mDiskManager;
        const std::filesystem::path mSliceDirPath;
        Utils::Logger* mLogger = nullptr;
        uint64_t mCurrOffset = 0;
        void Serialize();
        std::vector<SDEntry> Deserialize();
        SliceDirectory();
        static std::mutex mut;
        static SliceDirectory* instance;
        // Friend function to allow std::make_shared to access private constructor
        template<typename T, typename... Args>
        friend std::shared_ptr<T> std::make_shared(Args&&... args);

    public:
        static SliceDirectory* getInstance();
        void loadSliceDirectory();
        void saveSliceDirectory();
        void addSlice(size_t& sliceHash, const std::shared_ptr<Slice>& slice);
        SDEntry searchSlice(const size_t& sliceHash);
        void removeSlice(const size_t& sliceHash);
        void printSliceDirectory() {
            for (const auto& [hash, entry] : *mSliceMap) {
                mLogger->logInfo({std::to_string(hash), std::to_string(entry.sliceOffset), std::to_string(entry.sliceSize),
                    std::to_string(entry.exists),std::to_string(entry.refCount)});
            }
        }
        ~SliceDirectory();

    };
}
#endif //SLICEDIRECTORY_H