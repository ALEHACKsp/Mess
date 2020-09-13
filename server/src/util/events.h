#pragma once

template <typename... Args>
struct event_t
{
    using func_type = std::function<void(Args...)>;
    std::vector<func_type> funcs;

    void add(const func_type &func)
    {
        funcs.emplace_back(func);
    }

    void call(Args... params)
    {
        for (auto &func : funcs)
        {
            if (func)
                func(std::forward<Args>(params)...);
        }
    }
};