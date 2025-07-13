//
// Created by riju on 7/13/25.
//

#ifndef MATH_H
#define MATH_H
#include <memory>
#include "../headers/Slice.h"

namespace Math {
    class MatrixMul{
    public:
        static std::unique_ptr<Slice> matMul(std::unique_ptr<Slice>& inp,std::shared_ptr<Slice>& currSlice);
    };
}

#endif //MATH_H