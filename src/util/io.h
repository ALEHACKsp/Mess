#pragma once

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace io
{
    template <typename... Args>
    void log(const std::string_view str, Args... params)
    {
        spdlog::info(str.data(), std::forward<Args>(params)...);
    }

    template <typename... Args>
    void log_err(const std::string_view str, Args... params)
    {
        spdlog::error(str.data(), std::forward<Args>(params)...);
    }

    static bool read_file(std::string_view name, std::vector<uint8_t> &out)
    {
        std::ifstream file(name.data(), std::ios::binary);
        if (!file.good())
        {
            return false;
        }

        file.unsetf(std::ios::skipws);

        file.seekg(0, std::ios::end);
        const size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        out.resize(size);

        file.read((char *)out.data(), size);

        file.close();

        return true;
    }
}; // namespace io