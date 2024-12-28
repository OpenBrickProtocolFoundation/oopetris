

#pragma once

#include <functional>

template<typename T, typename Arg>
struct ScopeDefer {
private:
    using CallbackType = std::function<T(const Arg& arg)>;
    const CallbackType m_callback;
    const Arg m_cleanup_value;

public:
    ScopeDefer(const ScopeDefer&) = delete;            // no copy constructor
    ScopeDefer& operator=(const ScopeDefer&) = delete; // no self-assignments (aka copy assignment)

    ScopeDefer(CallbackType&& callback, Arg cleanup_value)
        : m_callback{ std::move(callback) },
          m_cleanup_value{ cleanup_value } { }

    ~ScopeDefer() {
        this->m_callback(this->m_cleanup_value);
    }
};


template<typename T, typename Arg>
struct ScopeDeferMultiple {
private:
    using CallbackType = std::function<T(const Arg& arg)>;
    std::vector<std::pair<CallbackType, Arg>> m_values{};

public:
    ScopeDeferMultiple(const ScopeDeferMultiple&) = delete;            // no copy constructor
    ScopeDeferMultiple& operator=(const ScopeDeferMultiple&) = delete; // no self-assignments (aka copy assignment)

    ScopeDeferMultiple() = default;

    void add(CallbackType&& callback, Arg cleanup_value) {
        m_values.emplace_back(std::move(callback), std::move(cleanup_value));
    }

    ~ScopeDeferMultiple() {
        for (auto it = m_values.rbegin(); it != m_values.rend(); ++it) {
            const auto& [callback, value] = *it;
            callback(value);
        }
    }
};
