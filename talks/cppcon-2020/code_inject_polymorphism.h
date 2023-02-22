// A back up from https://godbolt.org/z/bUDyhG for offline learning
// From talk: Dynamic Polymorphism with Metaclasses and Code Injection - Sy Brand

#include <experimental/meta>
#include <utility>

namespace meta = std::experimental::meta;

// Only normal member functions should have wrappers generated for them
consteval bool should_generate_function_for(meta::info i) {
  return meta::is_normal(i) and not meta::is_copy_assignment_operator(i) and
         not meta::is_move_assignment_operator(i);
}

// Loop over all the functions declared in the given typeclass and call f
// with each of them along with their return type and parameters
template <class F>
consteval void for_each_declared_function(meta::info typeclass, F&& f) {
  for (auto func : meta::member_fn_range(typeclass)) {
    if (should_generate_function_for(func)) {
      f(func, meta::return_type_of(func), meta::param_range(func));
    }
  }
}

// Stores information about the erased type necessary for doing SBO and such
struct storage_info {
  std::size_t size;
  std::size_t alignment;
};

// Defines the vtable layout for a given facade
template <class Facade>
struct vtable {
  consteval {
    for_each_declared_function(
        reflexpr(Facade), [](auto func, auto ret, auto params) consteval {
          ->__fragment struct {
            typename(ret) (*unqualid(func))(void* ptr, ->params);
          };
        });
  }
  void (*destroy_)(void* ptr);
  void (*copy_construct_)(void* ptr, void* rhs);
  void (*move_construct_)(void* ptr, void* rhs);
  storage_info (*storage_info_)();
};

// Fills in the vtable for a given concrete type
template <class Facade, class T>
consteval vtable<Facade> create_vtable_for() {
  vtable<Facade> table{};

  consteval {
    auto table_cap = reflexpr(table);
    for_each_declared_function(
        reflexpr(Facade), [table_cap](auto func, auto ret,
                                      auto params) consteval {
          ->__fragment {
            idexpr(table_cap).unqualid(func) = [](void* ptr, ->params) {
              return static_cast<T*>(ptr)->unqualid(func)(unqualid(... params));
            };
          };
        });
  }
  table.destroy_ = [](void* ptr) { static_cast<T*>(ptr)->~T(); };
  table.copy_construct_ = [](void* ptr, void* rhs) {
    new (ptr) T(*static_cast<T*>(rhs));
  };
  table.move_construct_ = [](void* ptr, void* rhs) {
    new (ptr) T(std::move(*static_cast<T*>(rhs)));
  };
  table.storage_info_ = [] { return storage_info{sizeof(T), alignof(T)}; };

  return table;
};

template <class Facade, class T>
constexpr vtable<Facade> vtable_for = create_vtable_for<Facade, T>();

// Storage with a Small Buffer Optimization
template <std::size_t Size, std::size_t Align = Size>
struct sbo_storage {
  void* t_;
  std::aligned_storage_t<Size, Align> sbo_;

  bool on_heap() { return t_ != &sbo_; }

  template <class T>
  sbo_storage(T&& t) {
    if constexpr (sizeof(T) <= Size) {
      new (&sbo_) T(std::forward<T>(t));
      t_ = &sbo_;
    } else {
      t_ = std::malloc(sizeof(std::decay_t<T>));
      new (t_) T(std::forward<T>(t));
    }
  }

  template <class VTable>
  sbo_storage(sbo_storage const& rhs, VTable const& vtable) {
    if (rhs.on_heap()) {
      t_ = std::malloc(vtable.storage_info_().size);
      vtable.copy_construct_(t_, rhs.t_);
    } else {
      vtable.copy_construct_(&sbo_, rhs.t_);
      t_ = &sbo_;
    }
  }

  template <class VTable>
  sbo_storage(sbo_storage&& rhs, VTable const& vtable) {
    if (rhs.on_heap()) {
      t_ = rhs.t_;
      rhs.t_ = nullptr;
    } else {
      vtable.move_construct_(&sbo_, rhs.t_);
      t_ = &sbo_;
    }
  }

  template <class OldVtable, class NewVtable>
  void assign(sbo_storage const& rhs, OldVtable const& old_vtable,
              NewVtable const& new_vtable) {
    destroy(old_vtable);

    if (rhs.on_heap()) {
      t_ = std::malloc(new_vtable.storage_info_().size);
      new_vtable.copy_construct_(t_, rhs.t_);
    } else {
      new_vtable.copy_construct_(&sbo_, rhs.t_);
      t_ = &sbo_;
    }
  }

  template <class OldVtable, class NewVtable>
  void assign(sbo_storage&& rhs, OldVtable const& old_vtable,
              NewVtable const& new_vtable) {
    destroy(old_vtable);

    if (rhs.on_heap()) {
      t_ = rhs.t_;
      rhs.t_ = nullptr;
    } else {
      new_vtable.move_construct_(&sbo_, rhs.t_);
      t_ = &sbo_;
    }
  }

  template <class VTable>
  void destroy(VTable const& vtable) {
    if (on_heap()) {
      if (!t_) return;
      vtable.destroy_(t_);
      std::free(t_);
    } else {
      vtable.destroy_(t_);
    }
  }

  void* get() { return t_; }
};

// Storage which is always on the heap
struct remote_storage {
  void* t_;

  template <class T>
  remote_storage(T&& t) : t_(std::malloc(sizeof(std::decay_t<T>))) {
    new (t_) std::decay_t<T>(std::forward<T>(t));
  }

  template <class Vtable>
  remote_storage(remote_storage const& rhs, Vtable const& vtable)
      : t_(std::malloc(vtable.storage_info_().size)) {
    vtable.copy_construct_(t_, rhs.t_);
  }

  template <class Vtable>
  remote_storage(remote_storage&& rhs, Vtable const& vtable) : t_(rhs.t_) {
    rhs.t_ = nullptr;
  }

  template <class OldVtable, class NewVtable>
  void assign(remote_storage const& rhs, OldVtable const& old_vtable,
              NewVtable const& new_vtable) {
    destroy(old_vtable);
    t_ = std::malloc(new_vtable.storage_info_().size);
    new_vtable.copy_construct_(t_, rhs.t_);
  }

  template <class OldVtable, class NewVtable>
  void assign(remote_storage&& rhs, OldVtable const& old_vtable,
              NewVtable const& new_vtable) {
    destroy(old_vtable);
    t_ = rhs.t_;
    rhs.t_ = nullptr;
  }

  template <class VTable>
  void destroy(VTable const& vtable) {
    if (!t_) return;
    vtable.destroy_(t_);
    std::free(t_);
    t_ = nullptr;
  }

  void* get() { return t_; }
};

template <class Facade, class Storage = remote_storage>
struct typeclass_for {
  vtable<Facade> const* vtable_;
  Storage storage_;

  template <class T>
  typeclass_for(T&& t)
      : storage_(std::forward<T>(t)), vtable_(&vtable_for<Facade, T>) {}

  consteval {
    for_each_declared_function(
        reflexpr(Facade), [](auto func, auto ret, auto params) consteval {
          ->__fragment struct {
            typename(ret) unqualid(func)(->params) {
              return this->vtable_->unqualid(func)(this->storage_.get(),
                                                   unqualid(... params));
            }
          };
        });
  }

  ~typeclass_for() { storage_.destroy(*vtable_); }

  typeclass_for(typeclass_for const& rhs)
      : storage_(rhs.storage_, *vtable_), vtable_(rhs.vtable_) {}

  typeclass_for(typeclass_for&& rhs)
      : storage_(std::move(rhs.storage_), *vtable_),
        vtable_(std::move(rhs.vtable_)) {}

  typeclass_for& operator=(typeclass_for const& rhs) {
    storage_.assign(rhs.storage_, *vtable_, *rhs.vtable_);
    vtable_ = rhs.vtable_;
    return *this;
  }

  typeclass_for& operator=(typeclass_for&& rhs) {
    storage_.assign(std::move(rhs.storage_), *vtable_, *rhs.vtable_);
    vtable_ = std::move(rhs.vtable_);
    return *this;
  }
};

#include <iostream>

struct animal_facade {
  void speak();
};

struct cat {
  void speak() { std::cout << "meow"; }
};

struct dog {
  void speak() { std::cout << "woof"; }
};

int main() {
  typeclass_for<animal_facade> c = cat{};
  c.speak();

  typeclass_for<animal_facade> d = dog{};
  d.speak();
}