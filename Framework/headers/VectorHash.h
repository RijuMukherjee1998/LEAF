//
// Created by Riju Mukherjee on 6/14/25.
//

#ifndef VECTORHASH_H
#define VECTORHASH_H
#include <city.h>
#include <vector>

namespace  Utils {
    // this hash is for unordered_maps
    struct CityHashVectorFloat {
        size_t operator()(const std::vector<float>& vec) const {
            if (vec.empty()) return 0;

            // Compute hash of the raw byte representation
            return CityHash64(
                reinterpret_cast<const char*>(vec.data()),
                vec.size() * sizeof(float)
            );
        }
    };
  inline uint64_t hash_float_matrix_optimized(const std::vector<std::vector<float>>& matrix) {
        if (matrix.empty()) return 0;

        uint64_t seed = CityHash64(
            reinterpret_cast<const char*>(matrix[0].data()),
            matrix[0].size() * sizeof(float)
        );

        for (size_t i = 1; i < matrix.size(); ++i) {
            uint64_t row_hash = CityHash64(
                reinterpret_cast<const char*>(matrix[i].data()),
                matrix[i].size() * sizeof(float)
            );
            // Combine row hashes (similar to boost::hash_combine)
            seed ^= row_hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        return seed;
    }
}
#endif //VECTORHASH_H