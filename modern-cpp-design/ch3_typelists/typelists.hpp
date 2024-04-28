#ifndef MODERN_CPP_DESIGN_CH_3_H
#define MODERN_CPP_DESIGN_CH_3_H

#include <cstddef>

struct NullType {};
struct EmptyType {};

namespace m_cpp_d::ch3 {

// 3.2 Typelist
// Typelists are devoid of any value: Their bodies are empty, they don’t hold
// any state, and they don’t define any functionality. At runtime, typelists
// don’t carry any value at all.Their only raison d’être is to carry type
// information.

// It follows that any typelist processing must necessarily occur at compile
// time, not at runtime.Typelists are not meant to be instantiated, although
// there’s no harm in doing that.

template <typename T, typename U>
struct Typelist {
  using head = T;
  using tail = U;
};

// 3.5 Calculating Length
// Returns the length (number of elements) of a typelist
template <typename TList>
struct Length;

template <>
struct Length<NullType> {
  static constexpr size_t value = 0;
};

template <typename T, typename U>
struct Length<Typelist<T, U>> {
  static constexpr size_t value = 1U + Length<U>::value;
};

// 3.6 Indexed Access
// Returns type at index, or compile-time error for out-of-bounds error
template <typename TList, size_t index>
struct TypeAt;

template <typename Head, typename Tail>
struct TypeAt<Typelist<Head, Tail>, 0> {
  using Result = Head;
};

template <typename Head, typename Tail, size_t i>
struct TypeAt<Typelist<Head, Tail>, i> {
  using Result = typename TypeAt<Tail, i - 1>::Result;
};

// 3.7 Searching Typelists
// Returns index if the type T is in typelist, otherwise -1
template <typename TList, typename T>
struct IndexOf;

template <typename T>
struct IndexOf<NullType, T> {
  static const int value = -1;
};

template <typename T, typename Tail>
struct IndexOf<Typelist<T, Tail>, T> {
  static const int value = 0;
};

template <typename Head, typename Tail, typename T>
struct IndexOf<Typelist<Head, Tail>, T> {
 private:
  static const int temp = IndexOf<Tail, T>::value;

 public:
  static const int value = (temp == -1 ? -1 : 1 + temp);
};

// 3.8 Appending to Typelists
// Appends a type to a typelist and returns a new typelist
template <typename TList, typename T>
struct Append;

template <>
struct Append<NullType, NullType> {
  using Result = NullType;
};

template <typename T>
struct Append<NullType, T> {
  using Result = Typelist<T, NullType>;
};

template <typename Head, typename Tail>
struct Append<NullType, Typelist<Head, Tail>> {
  using Result = Typelist<Head, Tail>;
};

template <typename Head, typename Tail, typename T>
struct Append<Typelist<Head, Tail>, T> {
  using Result = Typelist<Head, typename Append<Tail, T>::Result>;
};

// 3.9 Erasing a Type from a Typelist
// Erases first occurrence of type T from typelist, if it exists
template <typename TList, typename T>
struct Erase;

template <typename T>
struct Erase<NullType, T> {
  using Result = NullType;
};

template <typename T, typename Tail>
struct Erase<Typelist<T, Tail>, T> {
  using Result = Tail;
};

template <typename Head, typename Tail, typename T>
struct Erase<Typelist<Head, Tail>, T> {
  using Result = Typelist<Head, typename Erase<Tail, T>::Result>;
};

// 3.10 Erasing Duplicates
// Erases all instances of type T from typelist
template <typename TList, typename T>
struct EraseAll;

template <typename T>
struct EraseAll<NullType, T> {
  using Result = NullType;
};

template <typename T, typename Tail>
struct EraseAll<Typelist<T, Tail>, T> {
  using Result = typename EraseAll<Tail, T>::Result;
};

template <typename Head, typename Tail, typename T>
struct EraseAll<Typelist<Head, Tail>, T> {
  using Result = Typelist<Head, typename EraseAll<Tail, T>::Result>;
};

// Removes all duplicates from a typelist
template <typename TList>
struct NoDuplicates;

template <>
struct NoDuplicates<NullType> {
  using Result = NullType;
};

template <typename Head, typename Tail>
struct NoDuplicates<Typelist<Head, Tail>> {
 private:
  using L1 = typename NoDuplicates<Tail>::Result;
  using L2 = typename Erase<L1, Head>::Result;

 public:
  using Result = Typelist<Head, L2>;
};

// 3.11 Replacing an Element in a Typelist
// Replaces first occurrence of T with U, if it is in the typelist
template <typename TList, typename T, typename U>
struct Replace;

template <typename T, typename U>
struct Replace<NullType, T, U> {
  using Result = NullType;
};

template <typename T, typename Tail, typename U>
struct Replace<Typelist<T, Tail>, T, U> {
  using Result = Typelist<U, Tail>;
};

template <typename Head, typename Tail, typename T, typename U>
struct Replace<Typelist<Head, Tail>, T, U> {
  using Result = Typelist<Head, typename Replace<Tail, T, U>::Result>;
};

// Replaces all occurrence of T with U, if there are any
template <typename TList, typename T, typename U>
struct ReplaceAll;

template <typename T, typename U>
struct ReplaceAll<NullType, T, U> {
  using Result = NullType;
};

template <typename T, typename Tail, typename U>
struct ReplaceAll<Typelist<T, Tail>, T, U> {
  using Result = Typelist<U, typename ReplaceAll<Tail, T, U>::Result>;
};

template <typename Head, typename Tail, typename T, typename U>
struct ReplaceAll<Typelist<Head, Tail>, T, U> {
  using Result = Typelist<Head, typename Replace<Tail, T, U>::Result>;
};
/*
// 3.12 Partially Ordering Typelist

// Returns the type that is most derived from T
template <typename TList, typename T>
struct MostDerived;

template <typename T>
struct MostDerived<NullType, T> {
  using Result = T;
};

template <typename Head, typename Tail, typename T>
struct MostDerived<Typelist<Head, Tail>, T> {
 private:
  using Candidate = typename MostDerived<Tail, T>::Result;

 public:
  typedef typename Chapter2::Select<SUPERSUBCLASS(Candidate, Head), Head,
                                    Candidate>::Result Result;
};

// Orders a typelist from most derived to least
template <typename TList>
struct DerivedToFront;

template <>
struct DerivedToFront<NullType> {
  using Result = NullType;
};

template <typename Head, typename Tail>
struct DerivedToFront<Typelist<Head, Tail>> {
 private:
  using TheMostDerived = typename MostDerived<Tail, Head>::Result;
  using Temp = typename Replace<Tail, TheMostDerived, Head>::Result;
  using L = typename DerivedToFront<Temp>::Result;

 public:
  using Result = Typelist<TheMostDerived, L>;
};




// Reverses a typelist, used for DerivedToBack
template <typename TList>
struct Reverse;

template <>
struct Reverse<NullType> {
  using Result = NullType;
};

template <typename Head, typename Tail>
struct Reverse<Typelist<Head, Tail>> {
  using Result = typename Append<typename Reverse<Tail>::Result, Head>::Result;
};

// Orders a typelist from least derived to most
template <typename TList>
struct DerivedToBack;

template <>
struct DerivedToBack<NullType> {
  using Result = NullType;
};

template <typename Head, typename Tail>
struct DerivedToBack<Typelist<Head, Tail>> {
 private:
  using TheMostDerived = typename MostDerived<Tail, Head>::Result;
  using Temp = typename Replace<Tail, TheMostDerived, Head>::Result;
  using L = typename DerivedToFront<Temp>::Result;

 public:
  using Result = typename Reverse<Typelist<TheMostDerived, L>>::Result;
};

}  // namespace m_cpp_d::ch3
* /

/*
3.13 Class Generation with Typelists
I've included some updates I found in the Loki source code that aren't mentioned
in the book. FieldTraits has been replaced with the FieldHelper struct I also
replaced the tuple with c++11 tuples
*/

/*

#include <tuple>

// TODO: write explanations
namespace HierarchyGenerators {
namespace Helper {
// Allows inheriting from same type twice
// So you cna have 2 int fields
template <typename, typename>
struct ScatterHierarchyTag;
}  // namespace Helper

// Generates scattered heiratchies
template <typename TList, template <typename> typename Unit>
typename GenScatterHierarchy;

template <typename Head, typename Tail, template <typename> typename Unit>
class GenScatterHierarchy<DerivedTypelist<Head, Tail>, Unit>
    : public GenScatterHierarchy<Helper::ScatterHierarchyTag<Head, Tail>, Unit>,
      public GenScatterHierarchy<Tail, Unit> {
 public:
  using TList = typename DerivedTypelist<Head, Tail>;
  // Make sure LeftBase is unique, in case of double inheritence
  typedef typename GenScatterHierarchy<Helper::ScatterHierarchyTag<Head, Tail>,
                                       Unit>
      LeftBase;
  using RightBase = typename GenScatterHierarchy<Tail, Unit>;
  template <typename T>
  struct Rebind {
    using Result = Unit<T>;
  };
};

// In the middle *unique* class that resolve possible ambiguity
template <typename Head, typename Tail, template <typename> typename Unit>
class GenScatterHierarchy<Helper::ScatterHierarchyTag<Head, Tail>, Unit>
    : public GenScatterHierarchy<Head, Unit> {};

// If the argument passed was not a TL
template <typename AtomicType, template <typename> typename Unit>
class GenScatterHierarchy : public Unit<AtomicType> {
  using LeftBase = typename Unit<AtomicType>;
  template <typename T>
  struct Rebind {
    using Result = Unit<T>;
  };
};

// If the argument passed was not a TL
template <template <typename> typename Unit>
class GenScatterHierarchy<NullType, Unit> {
  template <typename T>
  struct Rebind {
    using Result = Unit<T>;
  };
};

// Generate linear hierarchy
template <typename TList,
          template <typename AtomicType, typename Base> typename Unit,
          typename Root = EmptyType>
class GenLinearHierarchy;

template <typename Head, typename Tail,
          template <typename, typename> typename Unit, typename Root>
class GenLinearHierarchy<DerivedTypelist<Head, Tail>, Unit, Root>
    : public Unit<Head, GenLinearHierarchy<Tail, Unit, Root>> {};

template <typename T, template <typename, typename> typename Unit,
          typename Root>
typename GenLinearHierarchy<DerivedTypelist<T, NullType>, Unit, Root>
    : public Unit<T, Root>{};

template <template <typename, typename> typename Unit, typename Root>
class GenLinearHierarchy<NullType, Unit, Root> : public Root {};

template <typename T, typename H>
typename H::template Rebind<T>::Result& Field(H& obj) {
  return obj;
}
template <typename T, typename H>
const typename H::template Rebind<T>::Result& Field(const H& obj) {
  return obj;
}

template <typename H, unsigned int i>
struct FieldHelper;
template <typename H>
struct FieldHelper<H, 0> {
  using ElementType = typename H::TList::Head;
  using UnitType = typename H::template Rebind<ElementType>::Result;

  enum {
    isTuple = Chapter2::Conversion<UnitType, std::tuple<ElementType>>::sameType,
    isConst = TypeTraits<H>::isConst
  };

  using ConstLeftBase = const typename H::LeftBase;
  typedef typename Chapter2::Select<isConst, ConstLeftBase,
                                    typename H::LeftBase>::Result LeftBase;

  typedef typename Chapter2::Select<isTuple, ElementType, UnitType>::Result
      UnqualifiedResultType;

  typedef typename Chapter2::Select<isConst, const UnqualifiedResultType,
                                    UnqualifiedResultType>::Result ResultType;

  static ResultType& Do(H& obj) {
    LeftBase& leftBase = obj;
    return leftBase;
  }
};

template <typename H, unsigned int i>
struct FieldHelper {
  using ElementType = typename DerivedTypeAt<typename H::TList, i>::Result;
  using UnitType = typename H::template Rebind<ElementType>::Result;

  enum {
    isTuple = Chapter2::Conversion<UnitType, std::tuple<ElementType>>::sameType,
    isConst = TypeTraits<H>::isConst
  };

  using ConstRightBase = const typename H::RightBase;

  typedef typename Chapter2::Select<isConst, ConstRightBase,
                                    typename H::RightBase>::Result RightBase;

  typedef typename Chapter2::Select<isTuple, ElementType, UnitType>::Result
      UnqualifiedResultType;

  typedef typename Chapter2::Select<isConst, const UnqualifiedResultType,
                                    UnqualifiedResultType>::Result ResultType;

  static ResultType& Do(H& obj) {
    RightBase& rightBase = obj;
    return FieldHelper<RightBase, i - 1>::Do(rightBase);
  }
};

// Returns field by index
template <int i, typename H>
typename FieldHelper<H, i>::ResultType& Field(H& obj) {
  return FieldHelper<H, i>::Do(obj);
}

};  // namespace HierarchyGenerators

*/

// 3.3 Linearize Typelist Creation
#define TYPELIST_1(T1) DerivedTypelist<T1, NullType>
#define TYPELIST_2(T1, T2) DerivedTypelist<T1, TYPELIST_1(T2)>
#define TYPELIST_3(T1, T2, T3) DerivedTypelist<T1, TYPELIST_2(T2, T3)>
#define TYPELIST_4(T1, T2, T3, T4) DerivedTypelist<T1, TYPELIST_3(T2, T3, T4)>
#define TYPELIST_5(T1, T2, T3, T4, T5) \
  DerivedTypelist<T1, TYPELIST_4(T2, T3, T4, T5)>
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) \
  DerivedTypelist<T1, TYPELIST_5(T2, T3, T4, T5, T6)>
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) \
  DerivedTypelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7)>
#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
  DerivedTypelist<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8)>

}  // namespace m_cpp_d::ch3

#endif