//
// Created by Riju Mukherjee on 6/14/25.
//
#include <fstream>
#include "../headers/SliceDirectory.h"
#include "../headers/Constants.h"

// Define the static member variables
std::mutex ModelOps::SliceDirectory::mut;
ModelOps::SliceDirectory* ModelOps::SliceDirectory::instance = nullptr;


ModelOps::SliceDirectory* ModelOps::SliceDirectory::getInstance() {
    std::lock_guard<std::mutex>lock(mut);
    if (instance == nullptr) {
        instance = new SliceDirectory();
    }
    return instance;
}

ModelOps::SliceDirectory::SliceDirectory() : mSliceDirPath(SLICE_DIR_FILE_PATH){
    mLogger = Utils::Logger::getInstance();
    mSliceMap = std::make_unique<std::unordered_map<size_t, SDEntry>>();
    mDiskManager = DiskOps::DiskManager::getInstance(SLICE_DATA_FILE_PATH);
    loadSliceDirectory();
}

ModelOps::SliceDirectory::~SliceDirectory() {
    saveSliceDirectory();
}

void ModelOps::SliceDirectory::loadSliceDirectory() {
    if (!std::filesystem::exists(mSliceDirPath)) {
        mLogger->logInfo({"Slice directory does not exist. Creating it"});
        Serialize();
        return;
    }
    std::vector<SDEntry> entries = Deserialize();
    mCurrOffset = 0;
    for (const auto& entry : entries)
    {
        (*mSliceMap)[entry.hashKey] = entry;
        mCurrOffset = std::max(mCurrOffset, entry.sliceOffset + entry.sliceSize);
    }
    mLogger->logInfo({"Slice directory loaded"});
}

void ModelOps::SliceDirectory::addSlice(size_t& sliceHash, const std::shared_ptr<Slice>& slice)
{
    if (mSliceMap->find(sliceHash) != mSliceMap->end()) {
        mLogger->logWarn({"Slice already exists in slice directory, increasing ref count"});
        mSliceMap->at(sliceHash).refCount++;
        return;
    }
    SDEntry entry;
    entry.hashKey = sliceHash;
    entry.sliceOffset = mCurrOffset;
    entry.sliceSize = slice->mSliceSize;
    entry.exists = true;
    entry.refCount = 1;
    mSliceMap->insert({sliceHash, entry});
    mLogger->logInfo({"Slice added to slice directory"});
    mDiskManager->writeSliceToDisk(mCurrOffset,slice);
    mCurrOffset += slice->mSliceSize;
}
ModelOps::SDEntry ModelOps::SliceDirectory::searchSlice(const size_t& sliceHash) {
    if (mSliceMap->find(sliceHash) == mSliceMap->end()) {
        return {};
    }
    return mSliceMap->at(sliceHash);
}
void ModelOps::SliceDirectory::removeSlice(const size_t& sliceHash) {
    if (mSliceMap->find(sliceHash) == mSliceMap->end() || (*mSliceMap)[sliceHash].refCount == 0 ) {
        mLogger->logError({"Slice does not exist in slice directory"});
    }
    (*mSliceMap)[sliceHash].refCount--;
    if ((*mSliceMap)[sliceHash].refCount == 0) {
        mSliceMap->erase(sliceHash);
    }
}

void ModelOps::SliceDirectory::saveSliceDirectory() {
    if (!std::filesystem::exists(mSliceDirPath))
        mLogger->logError({"Slice directory does not exist"});
    Serialize();
}
void ModelOps::SliceDirectory::Serialize()
{
    if (!std::filesystem::exists(mSliceDirPath)) {
        mLogger->logInfo({"Creating slice directory file"});
        std::ofstream createFile(mSliceDirPath.string(), std::ios::binary);
        if (!createFile)
            mLogger->logCritical({"Slice directory creation failed: " ,mSliceDirPath.string()});
        createFile.close();
    }
    // want to re-write the entire file not append to it so std::ios::trunc option is used
    std::ofstream sd_file(mSliceDirPath.string(), std::ios::binary|std::ios::trunc);
    if (!sd_file)
    {
        mLogger->logCritical({"Slice directory file could not be opened for write: " ,mSliceDirPath.string()});
    }
    const size_t count = mSliceMap->size();
    sd_file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (const auto& entry : *mSliceMap)
    {
        SDEntry entryToWrite = entry.second;
        sd_file.write(reinterpret_cast<const char*>(&entryToWrite), sizeof(entryToWrite));
    }
    sd_file.close();
}

std::vector<ModelOps::SDEntry> ModelOps::SliceDirectory::Deserialize()
{
    std::ifstream sd_file(mSliceDirPath.string(),std::ios::binary);
    if (!sd_file)
    {
        mLogger->logCritical({"Page directory could not be opened: " + mSliceDirPath.string()});
    }
    //read the no of entries
    size_t count;
    sd_file.read(reinterpret_cast<char*>(&count), sizeof(count));
    std::vector<ModelOps::SDEntry> entries(count);
    for (auto& entry : entries)
    {
        sd_file.read(reinterpret_cast<char*>(&entry), sizeof(ModelOps::SDEntry));
    }
    sd_file.close();
    return entries;
}
