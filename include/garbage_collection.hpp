#pragma once

#include <unordered_set>
#include <memory>

class Scope;
class Object;

class Heap final {
public:
    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    Heap(Heap&&) = delete;
    Heap& operator=(Heap&&) = delete;

    static Heap& Instance();

    template <typename T, typename... Args>
    Object* Make(Args&&... args);

    void MarkAndSweep(Scope* root);

private:
    Heap() = default;

private:
    std::unordered_set<std::unique_ptr<Object>> heap_;
};

template <typename T, typename... Args>
Object* Heap::Make(Args&&... args) {
    std::unique_ptr<T> object = std::make_unique<T>(std::forward<Args>(args)...);
    auto* ptr = object.get();
    heap_.insert(std::move(object));
    return ptr;
}
