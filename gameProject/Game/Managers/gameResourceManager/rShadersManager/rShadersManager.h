#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>

#include "../../../gameResources/rBaseResource/rBaseResource.h"
#include "../../../gameResources/gameResource/rShader/rShader.h"

class rShadersManager : public rBaseResource
{
private:
    std::mutex mtx;
    std::unordered_map<std::string, std::unique_ptr<rShader>> shaders;

    bool loadShader(const std::string &name);

public:
    rShadersManager(std::string path) : rBaseResource(path) {}

    bool initialize() override;
    void renderAllShaders(); // Exemplo simples de uso
    bool addShader(const std::string &key, const rShaderConfig &config);
};