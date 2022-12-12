#include <cstddef>  //size_t
#include <iostream>
#include <utility>  //swap
namespace playground {

template <typename T>
class SimpleSharedPtr {
  // Modified from reference:
  // [lokiastari.com
  // post](https://lokiastari.com/blog/2015/01/15/c-plus-plus-by-example-smart-pointer-part-ii/index.html)
 public:
  SimpleSharedPtr() = default;
  explicit SimpleSharedPtr(T* rawPtr) try  // the rarely used try/catch for
                                           // exceptions in argument lists
      : ptr_(rawPtr), refCnt_(new size_t{1}) {
    std::cout << "Ctor for val=" << *ptr_ << '\n';
  } catch (...) {
    // If an exception is thrown during construction (and thus the destructor
    // will not be called) we must assume responsibility for making sure that
    // pointer is deleted before the exception escapes the constructor,
    // otherwise there will be a resultant leak of the pointer.
    delete ptr_;
    throw;
  }

  // Though nullptr can be converted to any other type the compiler can not know
  // what type you want in this situation so you would need to be explicit. But
  // it would be nice to allow auto conversion from nullptr directly.
  SimpleSharedPtr(std::nullptr_t) {}

  ~SimpleSharedPtr() { cleanup(); }

  SimpleSharedPtr(const SimpleSharedPtr& cpyFrom) {
    std::cout << "Copy ctor for val=" << *cpyFrom.ptr_ << '\n';
    cleanup();
    ptr_ = cpyFrom.ptr_;
    refCnt_ = cpyFrom.refCnt_;
    if (nullptr != ptr_) {
      (*refCnt_)++;
    }
  }

  SimpleSharedPtr& operator=(const SimpleSharedPtr& cpyFrom) {
    std::cout << "Copy assign for val=" << *cpyFrom.ptr_ << '\n';
    cleanup();
    ptr_ = cpyFrom.ptr_;
    refCnt_ = cpyFrom.refCnt_;
    if (nullptr != ptr_) {
      (*refCnt_)++;
    }
    return *this;
  }

  SimpleSharedPtr(SimpleSharedPtr&& movedFrom) noexcept {
    std::cout << "Move ctor for val=" << *movedFrom.ptr_ << '\n';
    this->swap(movedFrom);
  }

  SimpleSharedPtr& operator=(SimpleSharedPtr&& movedFrom) noexcept {
    std::cout << "Move assign for val=" << *movedFrom.ptr_ << '\n';
    this->swap(movedFrom);
    return *this;
  }

  T* operator->() const { return ptr_; }
  T& operator*() const { return *ptr_; }

  T* get() const { return ptr_; }
  explicit operator bool() const { return ptr_; }

  void swap(SimpleSharedPtr& rhs) {
    std::swap(ptr_, rhs.ptr_);
    std::swap(refCnt_, rhs.refCnt_);
  }

 private:
  void cleanup() {
    if (refCnt_ == nullptr) {
      return;
    }
    (*refCnt_)--;
    if (*refCnt_ == 0) {
      delete refCnt_;
      std::cout << "Delete for val=" << *ptr_ << '\n';
      delete ptr_;
    }
  }

 private:
  T* ptr_ = nullptr;

  // You must dynamically allocate memory for the counter so that it can be
  // shared by all instances (you can not tell how many there will be or the
  // order in which they will be deleted)...
  // So for shared pointer that points to the same memory space, the counter
  // need to be shared. As there must be some mapping between a corresponding
  // memory space to a counter. This can be achieved by making refCnt a pointer.
  // And when you do copy of shared pointer, you just copy the refCnt_ to point
  // to the same one. Noted that if SimpleSharedPtr(T*) is repeatedly called, it
  // will resolve to a double delete, like normal std::shared_ptr. Programmer
  // needs to avoid that by themselves.
  size_t* refCnt_ = nullptr;
};

}  // namespace playground