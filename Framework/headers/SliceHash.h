//
// Created by riju on 6/28/25.
//

#ifndef SLICEHASH_H
#define SLICEHASH_H
#include <vector>
#include <cstdint>
#include <functional>
#include "Slice.h"
namespace Utils {
    struct SliceHash {
        size_t operator()(const std::shared_ptr<Slice>& slice)  const{
            // Start with slice size as part of the hash
            size_t hash = std::hash<uint32_t>()(slice->mSliceSize);

            // Hash weights if they exist
            if (slice->mWeights) {
                for (const auto& weight_vec_ptr : *slice->mWeights) {
                    if (weight_vec_ptr) {
                        // Combine hash of each float in the weight vector using XOR
                        // which is fast and provides good distribution
                        size_t vec_hash = 0;
                        for (float val : *weight_vec_ptr) {
                            // Hash each float value
                            size_t val_hash = std::hash<float>()(val);
                            // XOR is commutative but we get good mixing with the running hash
                            vec_hash ^= val_hash + 0x9e3779b9 + (vec_hash << 6) + (vec_hash >> 2);
                        }
                        hash ^= vec_hash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                    }
                }
            }

            // Hash biases if they exist
            if (slice->mBiases) {
                size_t bias_hash = 0;
                for (float val : *slice->mBiases) {
                    size_t val_hash = std::hash<float>()(val);
                    bias_hash ^= val_hash + 0x9e3779b9 + (bias_hash << 6) + (bias_hash >> 2);
                }
                hash ^= bias_hash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }

            return hash;
        }
    };
}
#endif //SLICEHASH_H