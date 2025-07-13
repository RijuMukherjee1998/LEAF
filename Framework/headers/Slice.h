//
// Created by riju on 6/14/25.
//

#ifndef SLICE_H
#define SLICE_H
#include <vector>
#include <cstdint>

struct Slice
{
public:
    uint32_t mNumRows   = 0;
    uint32_t mNumCols   = 0;
    uint32_t mNumBiases = 0;
    uint32_t mSliceSize = 0;
    std::unique_ptr<std::vector<std::shared_ptr<std::vector<float>>>> mWeights;
    std::unique_ptr<std::vector<float>>  mBiases;
    // Constructor
    Slice(std::unique_ptr<std::vector<std::shared_ptr<std::vector<float>>>> weights,
          std::unique_ptr<std::vector<float>> biases)
        : mWeights(std::move(weights)), mBiases(std::move(biases)) {
        if (mWeights && !mWeights->empty() && !mWeights->at(0)->empty()) {
            mNumRows = mWeights->size();
            mNumCols = mWeights->at(0)->size();
            mNumBiases = mBiases ? mBiases->size() : 0;
            mSliceSize = (mNumRows * mNumCols * sizeof(float)) +
                        (mNumBiases * sizeof(float)) + 4*sizeof(uint32_t);
        } else {
            mNumBiases = mBiases ? mBiases->size() : 0;
            mSliceSize = (mNumBiases * sizeof(float)) + 4*sizeof(uint32_t);
        }
    }

    // Move constructor
    Slice(Slice&& other) noexcept
        : mNumRows(other.mNumRows),
          mNumCols(other.mNumCols),
          mNumBiases(other.mNumBiases),
          mSliceSize(other.mSliceSize),
          mWeights(std::move(other.mWeights)),
          mBiases(std::move(other.mBiases)) {
        other.mNumRows = 0;
        other.mNumCols = 0;
        other.mNumBiases = 0;
        other.mSliceSize = 0;
    }

    // Move assignment operator
    Slice& operator=(Slice&& other) noexcept {
        if (this != &other) {
            mNumRows = other.mNumRows;
            mNumCols = other.mNumCols;
            mNumBiases = other.mNumBiases;
            mSliceSize = other.mSliceSize;
            mWeights = std::move(other.mWeights);
            mBiases = std::move(other.mBiases);
            other.mNumRows = 0;
            other.mNumCols = 0;
            other.mNumBiases = 0;
            other.mSliceSize = 0;
        }
        return *this;
    }

    // Explicitly delete copy constructor and copy assignment
    Slice(const Slice&) = delete;
    Slice& operator=(const Slice&) = delete;

    // Default destructor
    ~Slice() = default;
};

#endif //SLICE_H