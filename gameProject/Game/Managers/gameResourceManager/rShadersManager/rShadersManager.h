#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>

#include "../../../gameResources/rBaseResource/rBaseResource.h"
#include "../../../gameResources/gameResource/rShader/vignetteShader/vignetteShader.h"

class rShadersManager : public rBaseResource
{
private:
    mutable std::mutex mtx;
    std::unordered_map<std::string , std::shared_ptr<rShader>> shaders;
    bool loadShader(const std::string &name);

public:
    rShadersManager(std::string path) : rBaseResource(path) {}
    bool initialize() override;
	std::shared_ptr<rShader> getShader( const std::string & key ) const;
};