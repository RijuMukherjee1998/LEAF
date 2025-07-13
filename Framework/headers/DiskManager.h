//
// Created by riju on 6/29/25.
//

#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <fstream>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include "Logger.h"
#include "Slice.h"

namespace DiskOps
{
    class DiskManager
    {
        std::recursive_mutex slice_mut;
        std::recursive_mutex read_mut;
        std::recursive_mutex write_mut;
        std::filesystem::path currFilePath;
        Utils::Logger* logger = Utils::Logger::getInstance();
        DiskManager() = delete;
        DiskManager(const std::filesystem::path& currModelPath);
        void fileWrite(uint64_t pg_offset, const std::unique_ptr<char[]>&, size_t slice_size);
        std::shared_ptr<Slice> fileRead(uint64_t slice_offset, size_t);
        std::unique_ptr<char[]> convertSliceToBuffer(const std::shared_ptr<Slice>&);
        static std::unique_ptr<Slice> convertBufferToSlice(const std::unique_ptr<char[]>&);
    public:
        static DiskManager* getInstance(const std::filesystem::path& currModelPath){
            static DiskManager* instance = nullptr;
            static std::mutex mtx;
            mtx.lock();
            if (instance == nullptr)
            {
                instance = new DiskManager(currModelPath);
            }
            mtx.unlock();
            return instance;
        }
        void writeSliceToDisk(uint64_t page_offset, const std::shared_ptr<Slice>&);
        std::shared_ptr<Slice> readSliceFromDisk(uint64_t page_offset, size_t slice_size);
    };
} // DiskOps

#endif //DISKMANAGER_H