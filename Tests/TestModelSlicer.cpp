//
// Created by riju on 6/28/25.
//

#include <gtest/gtest.h>
#include "../headers/ModelSlicer.h"

ModelOps::ModelSlicer model_slicer("../ExampleModels/model_dump.json");
TEST(ModelSlicer, SLICEMODEL) {
    ASSERT_NO_THROW(model_slicer.SliceModel());
}