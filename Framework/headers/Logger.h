//
// Created by riju on 6/14/25.
//

#ifndef LOGGER_H
#define LOGGER_H


#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>


namespace Utils
{
    class Logger {
    private:
        std::shared_ptr<spdlog::logger> file_logger;
        std::shared_ptr<spdlog::logger> con_logger;
        std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink;
        std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
        Logger();
    public:
        static Logger* getInstance();
        void logInfo(std::initializer_list<const std::string> msgs) const;
        void logWarn(std::initializer_list<const std::string> msgs) const;
        void logError(std::initializer_list<const std::string> msgs) const;
        void logCritical(std::initializer_list<const std::string> msgs) const;
    };
}

#endif //LOGGER_H