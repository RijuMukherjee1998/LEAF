//
// Created by Riju Mukherjee on 13-06-2025.
//

#include "../headers/ModelSlicer.h"
#include "../headers/Slice.h"
#include "../headers/SliceDirectory.h"
#include "../headers/SliceHash.h"
#include "../headers/Constants.h"

#include <fstream>
#include <memory>
using json = nlohmann::json;

ModelOps::ModelSlicer::ModelSlicer(const std::filesystem::path& modelPath, const std::string& modelName)
{
    m_ModelName = modelName;
    m_ModelPath = modelPath;
    m_Logger = Utils::Logger::getInstance();
    m_LayerIds = new std::vector<std::string>();
}

std::unique_ptr<std::vector<std::shared_ptr<Slice>>> ModelOps::ModelSlicer::SliceModel() const
{
     if (m_ModelPath.empty()) m_Logger->logCritical({"Model path is empty"});
    std::ifstream model_file(m_ModelPath.string(), std::ios::binary);
    if (!model_file) m_Logger->logCritical({"Model file could not be opened"});
    json model_data;
    model_file >> model_data;
    model_file.close();
    std::unique_ptr<std::vector<std::shared_ptr<Slice>>> slice_vec = std::make_unique<std::vector<std::shared_ptr<Slice>>>();
    if (model_data.contains("weights")) {
        json weights_json = model_data["weights"];
        // Iterate through each layer
        for (auto& [layer_name, layer_data] : weights_json.items()) {
            std::unique_ptr<std::vector<std::shared_ptr<std::vector<float>>>> kernel_vec = std::make_unique<std::vector<std::shared_ptr<std::vector<float>>>>();
            std::unique_ptr<std::vector<float>> bias_vec = std::make_unique<std::vector<float>>();
            if (layer_name == "top_level_model_weights") continue; // Skip empty top level

            m_Logger->logInfo({"Processing layer: " + layer_name});

            // Navigate to the actual weights: weights -> layer_name -> sequential -> layer_name
            if (layer_data.contains("sequential") &&
                layer_data["sequential"].contains(layer_name)) {

                json layer_weights = layer_data["sequential"][layer_name];
                m_LayerIds->emplace_back(layer_name);
                // Extract kernel weights
                if (layer_weights.contains("kernel")) {
                    json kernel = layer_weights["kernel"];
                    m_Logger->logInfo({"Kernel shape for " + layer_name + ": " +
                                     std::to_string(kernel.size()) + " x " +
                                     std::to_string(kernel[0].size())});

                    for (size_t i = 0; i < kernel[0].size(); ++i) {
                        std::string kernel_row = "Kernel[" + std::to_string(i) + "]: ";
                        std::shared_ptr<std::vector<float>> kernel_col_vec = std::make_shared<std::vector<float>>();
                        for (size_t j = 0; j < kernel.size(); ++j) {
                            kernel_col_vec->emplace_back(kernel[j][i].get<float>());
                            kernel_row += std::to_string(kernel[j][i].get<float>()) + " ";
                        }
                        kernel_vec->emplace_back(kernel_col_vec);
                        m_Logger->logInfo({kernel_row});
                    }
                }

                // Extract bias weights
                if (layer_weights.contains("bias")) {
                    json bias = layer_weights["bias"];
                    m_Logger->logInfo({"Bias shape for " + layer_name + ": " +
                                     std::to_string(bias.size())});

                    // Log first few bias values
                    std::string bias_values = "Bias values: ";
                    for (size_t i = 0; i < bias.size(); ++i) {
                        bias_values += std::to_string(bias[i].get<float>()) + " ";
                        bias_vec->emplace_back(bias[i].get<float>());
                    }
                    m_Logger->logInfo({bias_values});
                }
                std::shared_ptr<Slice> slice = std::make_shared<Slice>(std::move(kernel_vec), std::move(bias_vec));
                // Create a slice for the layer
                slice_vec->emplace_back(slice);
            }
        }
    }
    else
    {
        m_Logger->logCritical({"Model file does not contain weights"});
    }
    return slice_vec;
}
void ModelOps::ModelSlicer::CreateModelArch(const std::vector<size_t>& hashes) {
    m_ModelName += ".arch";
    std::filesystem::path model_name = m_ModelName;
    std::filesystem::path model_arch_path = MODEL_ARCH_DIR/model_name;
    std::ofstream model_arch_file(model_arch_path.string(), std::ios::binary);
    if (!model_arch_file) m_Logger->logCritical({"Model arch file could not be opened"});
    model_arch_file << m_LayerIds->size();
    model_arch_file << "\n";
    int count = 0;
    for (auto& layer_id : *m_LayerIds) {
        model_arch_file << layer_id;
        model_arch_file << ":";
        model_arch_file << hashes.at(count);
        model_arch_file << "\n";
        count++;
    }
    model_arch_file.close();
}

void ModelOps::ModelSlicer::InsertIntoSliceDirectory(SliceDirectory* sliceDir) {
    const std::unique_ptr<std::vector<std::shared_ptr<Slice>>> slices = SliceModel();
    std::vector<size_t> m_Hashes;
    for (size_t i=0; i<slices->size(); i++) {
        size_t hash = Utils::SliceHash{}(slices->at(i));
        m_Hashes.emplace_back(hash);
        sliceDir->addSlice(hash,slices->at(i));
    }
    CreateModelArch(m_Hashes);
}