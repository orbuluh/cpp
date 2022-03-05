#pragma once

template <typename T> struct Maybe;

template <typename T> 
auto maybe(T* context) { return Maybe<T>(context); }

template <typename T> struct Maybe {
    T* context_;
    Maybe(T* context) : context_(context) {};

    template <typename Fnc>
    auto With(Fnc evaluator) {
        return context_ != nullptr? 
            // if context_ is not nullptr, create Maybe<decltype(*context_)>
            maybe(evaluator(context_)): nullptr;
    }

    template <typename Fnc>
    auto Do(Fnc fnc) {
        if (context_ != nullptr) 
            fnc(context_);
        // won't call the function if context_ is nullptr
        return *this;
    }
};
