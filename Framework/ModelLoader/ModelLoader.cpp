//
// Created by Riju Mukherjee on 13-06-2025.
//

#include "../headers/ModelLoader.h"

#include <filesystem>
#include <fstream>
std::vector<std::string> ModelOps::ModelLoader::m_modelList;

ModelOps::ModelLoader::ModelLoader(const std::string& modelName) {
    m_modelName = modelName;
    m_logger = Utils::Logger::getInstance();
    m_sliceCache = SliceCache::getInstance();
}

bool ModelOps::ModelLoader::searchModel()
{
    if (m_modelList.empty()) {
        std::filesystem::path allModelsPath = MODEL_ARCH_DIR;
        for (const auto& entry : std::filesystem::directory_iterator(allModelsPath)) {
            if (entry.is_regular_file()) {
                m_modelList.push_back(entry.path().filename().string());
            }
        }
    }
    for (const auto& model : m_modelList) {
        if (model == m_modelName) {
            return true;
        }
    }
    return false;
}

void ModelOps::ModelLoader::loadModel()
{
    if (searchModel() != true) {
        m_logger->logError({"Model not found", m_modelName});
        return;
    }
    std::filesystem::path modelFileName = m_modelName;
    std::filesystem::path modelFilePath = MODEL_ARCH_DIR / modelFileName;
    std::ifstream inputModel (modelFilePath);
    if (!inputModel) {
        m_logger->logError({"Model file not found", m_modelName});
        return;
    }
    std::string line;
    int lineNumber = 0;
    int numLayers = 0;
    while (std::getline(inputModel, line)) {
        if (lineNumber == 0) {
            numLayers = std::stoi(line);
        }
        else {
            size_t colon_pos = line.find(':');
            if (colon_pos == std::string::npos) {
                m_logger->logError({"Invalid model file ... Maybe corrupt", m_modelName});
                return;
            }
            std::string key = line.substr(0, colon_pos);
            std::string value_str = line.substr(colon_pos + 1);
            m_currModel.emplace_back(key, std::stoll(value_str));
        }
        lineNumber++;
    }
    if (lineNumber != numLayers + 1) { // +1 for the number of layers
        m_logger->logError({"Invalid model file ... Maybe corrupt", m_modelName});
        return;
    }
}

void ModelOps::ModelLoader::runModel(std::unique_ptr<Slice>& inputLayer) {
    loadModel();
    // will refer slice cache to get the current slices required for the computation.
    for (const auto& layer :  m_currModel) {
        std::shared_ptr<Slice> currSlice = m_sliceCache->getSlice(layer.second);
        if (currSlice == nullptr) {
            m_logger->logError({"Slice not found ... Aborting can't run", std::to_string(layer.second)});
            return;
        }
        inputLayer = Math::MatrixMul::matMul(inputLayer, currSlice);
        if (inputLayer == nullptr) {
            m_logger->logCritical({"Model computation failed matrix size mismatch... Aborting"});
        }
        printSliceLayer(currSlice.get());
    }
    // printing the output
    printSliceLayer(inputLayer.get());
}

void ModelOps::ModelLoader::printSliceLayer(Slice* currSlice) {
    static int id = 0;
    std::string kernel_layer = "Kernel Layer " + std::to_string(id++) + "::";
    m_logger->logInfo({kernel_layer});
    for (uint32_t i=0; i<currSlice->mNumRows; i++) {
        std::string row_str = "row" + std::to_string(i) + "->";
        for (uint32_t j=0; j<currSlice->mNumCols; j++) {
            float val = currSlice->mWeights->at(i)->at(j);
             row_str+= std::to_string(val);
        }
        m_logger->logInfo({row_str});
    }
}