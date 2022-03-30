#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
namespace flyweight {

// Imagine a 2-D game map editor

enum class TextureType : char {
    // flyweight types
    Ground = 'G',
    Grass = 'W',
    Rock = 'R',
    Nothing = 'N',
};

class Texture {
public:
    friend class TextureFactory;
    char draw() const {
        // should have referred to the big size image
        // just return the char as demonstration
        return mimicDraw_;
    }
private:
    Texture(TextureType type, std::string_view filePath)
        : mimicDraw_(static_cast<char>(type))
        , someBigFileImage_(filePath){
        // assuming the image is super huge, hence needs to be
        // put as intrinsic state of a flyweight texture
        std::cout << "create Texture " << static_cast<char>(type)
            << " using image@" << someBigFileImage_ << '\n';
    }
    char mimicDraw_;
    std::string someBigFileImage_;
};

class TextureFactory {
public:
    static Texture& getTexture(TextureType type) {
        static std::unordered_map<TextureType, std::unique_ptr<Texture>> cache_;
        auto it = cache_.find(type);
        if (it != cache_.end()) {
            return *(it->second);
        }
        cache_[type].reset(new Texture(type, "some path"));
        return *cache_[type];
    }
};

class MapBuilder {
public:
    // Map is basically the context, the index i, j of a Texture
    // are basically the extrinsic states of a MapGrid
    MapBuilder(int m, int n) {
        twoDMap_.resize(m,
            std::vector<Texture*>(n,
                &TextureFactory::getTexture(TextureType::Nothing)));
    }
    void build(int i, int j, TextureType type) {
        //assert(i < m && j < n);
        twoDMap_[i][j] = &TextureFactory::getTexture(type);
    }

    void draw() const {
        for (int i = 0; i < twoDMap_.size(); ++i) {
            std::ostringstream os;
            for (int j = 0; j < twoDMap_[i].size(); ++j) {
                os << twoDMap_[i][j]->draw() << ' ';
            }
            std::cout << os.str() << '\n';
        }
    }
private:
    std::vector<std::vector<Texture*>> twoDMap_;
};

void demo();
} // namespace flyweight