//
// Created by Riju Mukherjee on 13-06-2025.
//

#ifndef MODELSLICER_H
#define MODELSLICER_H

#include <filesystem>
#include<nlohmann/json.hpp>
#include "Logger.h"
#include "Slice.h"
#include "SliceDirectory.h"
namespace ModelOps {
    class ModelSlicer
    {
    private:
        std::filesystem::path m_ModelPath;
        std::string m_ModelName;
        Utils::Logger* m_Logger;
        std::vector<std::string>* m_LayerIds;
        std::unique_ptr<std::vector<std::shared_ptr<Slice>>> SliceModel() const;
    public:
        ModelSlicer(const std::filesystem::path& modelPath,const std::string& modelName);
        void InsertIntoSliceDirectory(SliceDirectory* sliceDir);
        void CreateModelArch(const std::vector<size_t>& hashes);
    };
}
#endif //MODELSLICER_H
