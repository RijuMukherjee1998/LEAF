//
// Created by riju on 7/13/25.
//

#include "Math.h"

std::unique_ptr<Slice> Math::MatrixMul::matMul(std::unique_ptr<Slice>& inp, std::shared_ptr<Slice>& currSlice) {
    std::unique_ptr<std::vector<std::shared_ptr<std::vector<float>>>> weights = std::make_unique<std::vector<std::shared_ptr<std::vector<float>>>>();
    std::unique_ptr<std::vector<float>> biases = std::make_unique<std::vector<float>>();
    if (inp->mNumCols != currSlice->mNumCols)
        return nullptr;
    std::shared_ptr<std::vector<float>> row = std::make_shared<std::vector<float>>();
    for (uint32_t i=0; i<currSlice->mNumRows; i++)
    {
        float val = 0.0f;
        for (uint32_t j=0; j<currSlice->mNumCols; j++)
        {
            val += inp->mWeights->at(0)->at(j) * currSlice->mWeights->at(i)->at(j);
        }
        val += currSlice->mBiases->at(i);
        row->emplace_back(val);
    }
    weights->emplace_back(row);
    std::unique_ptr<Slice> out = std::make_unique<Slice>(std::move(weights), std::move(biases));
    return out;
}
