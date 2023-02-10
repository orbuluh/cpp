# Breaking Dependencies - C++ Type Erasure - The Implementation Details - Klaus Iglberger


## A motivating example

Would you provide an abstraction for callable by means of an inheritance hierarchy?

```cpp
class Command {
public:
    virtual void operator()(int) const = 0;
};
class PrintCommand : public Command { /**/ };
class SearchCommand : public Command { /**/ };
class ExecuteCommand : public Command { /**/ };

void f(Command* command);
```

NO, you wouldn't. `std::function` is probably a better approach! Type erasure instead of inheritance provides:

- no inheritance hierarchies
- non-intrusive
- less dependencies
- less pointers
- no manual dynamic allocation (and thus no manual life-time management)
- value semantics
- less code to write
- potentially better performance

```cpp
class PrintCommand { /**/ };
class SearchCommand { /**/ };
class ExecuteCommand { /**/ };

void f(std::function<void(int)> command);
```

> "Inheritance is rarely the answer. (Andrew Hunt, David Thomas, The pragmatic programmer)"

## Type erasure - terminology

Type erasure is not ...

- a `void*`
- a pointer-to-base
- a `std::variant`

Type erasure IS

- a **templated constructor** plus
- a completely **non-virtual interface** polus
- **External polymorphism + Bridge + Prototype**

## A type-erased Shape

```cpp
class Circle {
 public:
  explicit Circle(double rad)
      : radius{rad},  //...
  {}
  double getRadius() const noexcept;
  // getCenter(), getRotation(), ...
 private:
  double radius;
  // remaining data members
};

class Square {
 public:
  explicit Square(double s)
      : side{s},  //...
  {}

  double getSide() const noexcept;
  // getCenter(), getRotation(), ...
 private:
  double side;
  // remaining data members
};
```

So `Circle` and `Square` are ...

- don't need a base class
- don't know about each other
- don't know anything about their operations (affordances)

```cpp
struct ShapeConcept {
  virtual ~ShapeConcept() = default;

  virtual void do_serialize(/*...*/) const = 0;
  virtual void do_draw(/*...*/) const = 0;
};

template <typename ShapeT>
struct ShapeModel : public ShapeConcept {
  ShapeModel(ShapeT shape) : shape_(std::move(shape)) {}

  void do_serialize(/*...*/) const override { serialize(shape_, /*...*/); }
  void do_draw(/*...*/) const override { draw(shape_, /*...*/); }

  ShapeT shape_;
};
```

- So the key part of the `serialize` and `draw` call is that, the implementation of the virtual functions `do_serialize`/`do_draw` in the `ShapeModel` defines the affordances required by the type `T`.
- `serialize` and `draw` are free functions that is required to make compile to work. It's basically the same requirement that your base class enforces you.
- The `ShapeConcept` and `ShapeModel` are the external polymorphism design: we create a separate hierarchy, separate model for some independent types. By doing so, we extract the isolated operations out from class.

## The external polymorphism design pattern

- Allows any `shape_` to be treated polymorphically
- extracts implementation details (single responsibility principle)
- removes dependencies to operations (affordances)
- creates the opportunity for easy extension (open close principle)

## Continue the example

The affordances

- These functions resolve the requirements posed by the External Polymorphism design pattern
- There can be many implementation, spread over many header/source files (e.g. for OpenGL, Metal, ...)


```cpp
void serialize(const Circle&, /*...*/);
void draw(const Circle&, /*...*/);

void serialize(const Square&, /*...*/);
void draw(const Square&, /*...*/);
```

Usage

```cpp
void drawAllShapes(const std::vector<std::unique_ptr<ShapeConcept>>& shapes) {
  for (const auto& shape : shapes) {
    shape->draw();
  }
}

void main() {
  using Shapes = std::vector<std::unique_ptr<ShapeConcept>>;
  Shapes shapes;
  shapes.emplace_back(std::make_unique<ShapeModel<Circle>>{2.0});
  shapes.emplace_back(std::make_unique<ShapeModel<Square>>{1.5});
  shapes.emplace_back(std::make_unique<ShapeModel<Circle>>{2.0});
  drawAllShapes(shapes);
}
```





