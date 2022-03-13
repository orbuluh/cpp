#pragma once

#include <memory>
#include <iostream>
namespace bridge {

// Pimpl is a demonstration of bridge
class SecreteStrategy {
public:
    SecreteStrategy();
    bool shouldBuy();
private:
    class SecreteStrategyImpl;
    std::unique_ptr<SecreteStrategyImpl> pimpl_;
};

//----------Bridge example 2----------

struct Renderer {
    virtual void render_circle(float x, float y, float radius) = 0;
};

struct VectorRenderer: public Renderer {
    void render_circle(float x, float y, float radius) override {
        std::cout << "VectorRender the circle of radius " << radius
            << " at (" << x << "," << y << ")\n";
    }
};

struct RasterRenderer: public Renderer {
    void render_circle(float x, float y, float radius) override {
        std::cout << "RasterRenderer the circle of radius " << radius
            << " at (" << x << "," << y << ")\n";
    }
};

struct Shape {
protected:
    Renderer& renderer;
    Shape(Renderer& rd) : renderer(rd) {}
public:
    virtual void draw() = 0;
    virtual void resize(float factor) = 0;
};

struct Circle : Shape {
    float x, y, radius;
    // we use the bridge to connect the Circle (which has information
    // about its location and size) to the process of rendering.
    void draw() override { renderer.render_circle(x, y, radius); }

    // If you need to fine-tune the circle, you can resize() it,
    // and the rendering will still work just fine, as the renderer doesn’t know or care
    // about the Circle and doesn’t even take it as reference!
    void resize(float factor) override { radius *= factor; }
    Circle(Renderer& renderer, float x, float y, float radius)
        : Shape{renderer}, x{x}, y{y}, radius{radius} {}
};

void demo();
} // namespace bridge