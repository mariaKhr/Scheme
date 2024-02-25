#include <garbage_collection.hpp>
#include <func.hpp>

void Heap::MarkAndSweep(Scope* root) {
    if (root) {
        for (const auto& [name, obj] : *root) {
            obj->Mark();
        }
    }

    for (auto it = heap_.begin(), end = heap_.end(); it != end;) {
        if (!(*it)->marked_) {
            it = heap_.erase(it);
            continue;
        }
        ++it;
    }

    if (root) {
        for (const auto& [name, obj] : *root) {
            obj->Unmark();
        }
    }
}

Heap& Heap::Instance() {
    static auto heap = Heap();
    return heap;
}
