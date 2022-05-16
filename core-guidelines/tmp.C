namespace Interface {
class Smiley : public virtual Circle { // pure interface
public:
    // ...
};
} // namespace Interface

namespace Impl {
class Smiley : public virtual Interface::Smiley, public Impl::Circle {   // implementation
public:
    // constructors, destructor
    // ...
}
} // namespace Impl