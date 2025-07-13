//
// Created by teama on 13-06-2025.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H
#include <string>
#include <vector>
#include "Constants.h"
#include "Logger.h"
#include "SliceCache.h"
#include "../library/Math.h"
namespace ModelOps
{
    class ModelLoader
    {
    private:
        std::string m_modelName;
        std::vector<std::pair<std::string,size_t>> m_currModel; // {layer1 : hash_id1, layer2 : hash_id2, ....}
        bool searchModel();
        void loadModel();
        Utils::Logger* m_logger;
        SliceCache* m_sliceCache;
    public:
        static std::vector<std::string> m_modelList;
        ModelLoader(const std::string& modelName);
        void runModel(std::unique_ptr<Slice>& inp_layer);
        void printSliceLayer(Slice* currSlice);
    };
}
#endif //MODELLOADER_H
