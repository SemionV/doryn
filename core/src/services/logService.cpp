#include <dory/core/services/logService.h>

namespace dory::core::services
{
    TerminalSink::TerminalSink(std::unique_ptr<spdlog::sinks::sink> sink, Registry& registry):
            _sink(std::move(sink)), _registry(registry)
    {}

    void TerminalSink::log(const spdlog::details::log_msg& msg)
    {
        auto terminal = _registry.get<devices::ITerminalDevice>();
        if(terminal)
        {
            auto isCommandMode = terminal->isCommandMode();
            if(isCommandMode)
            {
                terminal->exitCommandMode();
            }

            try
            {
                _sink->log(msg);
            }
            catch(...)
            {}

            if(isCommandMode)
            {
                terminal->enterCommandMode();
            }
        }
    }

    void TerminalSink::flush()
    {
        _sink->flush();
    }

    void TerminalSink::set_pattern(const std::string& pattern)
    {
        _sink->set_pattern(pattern);
    }

    void TerminalSink::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter)
    {
        _sink->set_formatter(std::move(sink_formatter));
    }
}