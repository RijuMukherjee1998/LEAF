//
// Created by Riju Mukherjee on 6/29/25.
//

#include "../headers/DiskManager.h"

#include <valarray>

#include "../headers/Slice.h"

DiskOps::DiskManager::DiskManager(const std::filesystem::path &currModelPath) {
    this->currFilePath = currModelPath;
}

void DiskOps::DiskManager::fileWrite(uint64_t slice_offset, const std::unique_ptr<char[]>& slice_buffer, size_t slice_size) {
    const std::filesystem::path unified_model_name = "unified_model.leaf";
    const std::filesystem::path unified_model_path = currFilePath / unified_model_name;
    std::lock_guard<std::recursive_mutex> lock(write_mut);
    if (!std::filesystem::exists(unified_model_path)) {
        std::ofstream umf(unified_model_path, std::ios::binary);
        umf.close();
    }
    std::ofstream unified_model_file(unified_model_path, std::ios::binary|std::ios::in | std::ios::out);
    if (!unified_model_file) {
        logger->logCritical({"Failed to open unifed model file for write", unified_model_path.string()});
        return;
    }
    unified_model_file.seekp(slice_offset, std::ios::beg);
    unified_model_file.write(slice_buffer.get(), slice_size);
    unified_model_file.close();
}

std::shared_ptr<Slice> DiskOps::DiskManager::fileRead(uint64_t slice_offset, size_t slice_size) {
    const std::filesystem::path unified_model_name = "unified_model.leaf";
    const std::filesystem::path unified_model_path = currFilePath / unified_model_name;
    std::lock_guard<std::recursive_mutex> file_lock(read_mut);
    std::ifstream inFile(unified_model_path, std::ios::binary | std::ios::in);
    if (!inFile.is_open())
    {
        logger->logWarn({"Read failed ... Unable to open unified model file"});
        return nullptr;
    }
    inFile.seekg(slice_offset);
    if (!inFile.good())
    {
        logger->logCritical({"Read failed ... Unable to seek to offset in umf"});
        return nullptr;
    }

    const auto slice_buffer = std::make_unique<char[]>(slice_size);
    inFile.read(slice_buffer.get(), slice_size);
    if (!inFile.good())
    {
        /*This is always not an error as you have to understand that after a new PDE the page is never added to the
          buffer so the page at this offset dosen't exist have to handle it in a better way will think about it later*/
        logger->logWarn({"Read failed ... Unable to read the entire buffer" , unified_model_path.string()});
        return nullptr;
    }
    return convertBufferToSlice(slice_buffer);
}

std::unique_ptr<Slice> DiskOps::DiskManager::convertBufferToSlice(const std::unique_ptr<char[]>& slice_buffer) {
    char* buffer_ptr = slice_buffer.get();
    //Deserialize numRows, numCols, numBiases, slice size
    uint32_t numRows;
    std::memcpy(&numRows, buffer_ptr, sizeof(uint32_t));
    buffer_ptr += sizeof(uint32_t);
    uint32_t numCols;
    std::memcpy(&numCols, buffer_ptr, sizeof(uint32_t));
    buffer_ptr += sizeof(uint32_t);
    uint32_t numBiases;
    std::memcpy(&numBiases, buffer_ptr, sizeof(uint32_t));
    buffer_ptr += sizeof(uint32_t);
    uint32_t slice_size;
    std::memcpy(&slice_size, buffer_ptr, sizeof(uint32_t));
    buffer_ptr += sizeof(uint32_t);

    //Deserialize list of weights from buffer
    std::unique_ptr<std::vector<std::shared_ptr<std::vector<float>>>> all_weights = std::make_unique<std::vector<std::shared_ptr<std::vector<float>>>>(numRows);
    for (uint32_t i = 0; i < numRows; i++) {
        std::shared_ptr<std::vector<float>> weights = std::make_shared<std::vector<float>>(numCols);
        for (auto& weight : *weights)
        {
            std::memcpy(&weight, buffer_ptr, sizeof(float));
            buffer_ptr += sizeof(float);
        }
        all_weights->at(i) = weights;
    }
    std::unique_ptr<std::vector<float>> biases = std::make_unique<std::vector<float>>(numBiases);
    for (auto& bias : *biases) {
        std::memcpy(&bias, buffer_ptr, sizeof(float));
        buffer_ptr += sizeof(float);
    }
    auto slice = std::make_unique<Slice>(std::move(all_weights), std::move(biases));

    return slice;
}

std::unique_ptr<char[]> DiskOps::DiskManager::convertSliceToBuffer(const std::shared_ptr<Slice>& slice) {
    size_t slice_size = slice->mSliceSize;
    std::unique_ptr<char[]> slice_buffer = std::make_unique<char[]>(slice_size);
    std::lock_guard<std::recursive_mutex> slice_lock(slice_mut);
    char* buffer_ptr = slice_buffer.get();

    //Copy all type of slice info into buffer
    std::memcpy(buffer_ptr, &slice->mNumRows, sizeof(uint32_t));
    buffer_ptr += sizeof(uint32_t);

    std::memcpy(buffer_ptr, &slice->mNumCols, sizeof(uint32_t));
    buffer_ptr += sizeof(uint32_t);

    std::memcpy(buffer_ptr, &slice->mNumBiases, sizeof(uint32_t));
    buffer_ptr += sizeof(uint32_t);

    std::memcpy(buffer_ptr, &slice->mSliceSize, sizeof(uint32_t));
    buffer_ptr += sizeof(uint32_t);

    //Copy all weights into buffer
    for (uint32_t i = 0; i < slice->mNumRows; i++) {
        for (const auto& weight : *(slice->mWeights->at(i))) {
            std::memcpy(buffer_ptr, &weight, sizeof(float));
            buffer_ptr += sizeof(float);
        }
    }
    // copy all biases into buffer
    for (const auto& bias : *(slice->mBiases)) {
        std::memcpy(buffer_ptr, &bias, sizeof(float));
        buffer_ptr += sizeof(float);
    }
    return slice_buffer;
}

void DiskOps::DiskManager::writeSliceToDisk(uint64_t slice_offset, const std::shared_ptr<Slice> & slice) {
    size_t slice_size = slice->mSliceSize;
    const std::unique_ptr<char[]> slice_buffer = convertSliceToBuffer(slice);
    fileWrite(slice_offset, slice_buffer, slice_size);
}

std::shared_ptr<Slice> DiskOps::DiskManager::readSliceFromDisk(uint64_t slice_offset, size_t slice_size) {
    std::shared_ptr<Slice> slice_from_disk = fileRead(slice_offset, slice_size);
    return slice_from_disk;
}
