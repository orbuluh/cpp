## C.168: Define overloaded operators in the namespace of their operands
- Readability. Ability for find operators using ADL. Avoiding inconsistent definition in different namespaces
```cpp
namespace N {
struct S { };
bool operator==(S, S);   // OK: in the same namespace as S, and even next to S
}

N::S s;
bool x = (s == s);  // finds N::operator==() by ADL
```
```cpp
// Here, the meaning of !s differs in N and M. This can be most confusing.
// Remove the definition of namespace M and the confusion is replaced by
// an opportunity to make the mistake.
struct S { };
S s;

namespace N {
    S::operator!(S a) { return true; }
    S not_s = !s;
}

namespace M {
    S::operator!(S a) { return false; }
    S not_s = !s;
}
```