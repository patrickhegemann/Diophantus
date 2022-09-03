#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/attributes/value_extraction.hpp>

#include <stdexcept>

#define LOG BOOST_LOG_TRIVIAL

#define LOG_TRACE   LOG(trace)
#define LOG_DEBUG   LOG(debug)
#define LOG_INFO    LOG(info)
#define LOG_WARNING LOG(warning)
#define LOG_ERROR   LOG(error)
#define LOG_FATAL   LOG(fatal)

inline void setLoggingLevel(const boost::log::trivial::severity_level level)
{
    boost::log::core::get()->set_filter([level](const boost::log::attribute_value_set& attr_set)
    {
        return attr_set["Severity"].extract<boost::log::trivial::severity_level>() >= level;
    });
}

inline void setLoggingLevel(size_t level)
{
    constexpr int N_LOG_LEVELS = 6;
    if (level >= N_LOG_LEVELS)
    {
        throw std::invalid_argument("invalid logging verbosity level");
    }

    using boost::log::trivial::severity_level;
    constexpr severity_level levels[N_LOG_LEVELS] = {
        severity_level::fatal,
        severity_level::error,
        severity_level::warning,
        severity_level::info,
        severity_level::debug,
        severity_level::trace
    };

    setLoggingLevel(levels[level]);
}
