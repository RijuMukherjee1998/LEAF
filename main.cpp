#include <iostream>
#include "./Framework/headers/ModelSlicer.h"
#include "Framework/headers/ModelLoader.h"

void AddNewModel(ModelOps::SliceDirectory* slice_directory) {
    ModelOps::ModelSlicer model_slicer("/home/riju/Desktop/Dev/LEAF/ExampleModels/model_dump.json", "my_model");
    model_slicer.InsertIntoSliceDirectory(slice_directory);
    //slice_directory->printSliceDirectory();
}
void RunModel(std::string model_name, std::unique_ptr<Slice>& input) {
    std::cout << "Running model: " << model_name << std::endl;
    ModelOps::ModelLoader model_loader(model_name);
    model_loader.runModel(input);
}
int main()
{
    std::cout << "Running Light AI Framework" << std::endl;
    ModelOps::SliceDirectory* slice_directory = ModelOps::SliceDirectory::getInstance();
    AddNewModel(slice_directory);
    //creating an input for an example run
    std::unique_ptr<std::vector<std::shared_ptr<std::vector<float>>>> weights = std::make_unique<std::vector<std::shared_ptr<std::vector<float>>>>();
    std::unique_ptr<std::vector<float>> biases = std::make_unique<std::vector<float>>();
    std::vector<float> v = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    std::shared_ptr<std::vector<float>> row = std::shared_ptr<std::vector<float>>(new std::vector<float>(v));
    weights->emplace_back(row);
    std::unique_ptr<Slice> inp = std::make_unique<Slice>(std::move(weights), std::move(biases));
    RunModel("my_model.arch",inp);
    delete(slice_directory);
    return 0;
}