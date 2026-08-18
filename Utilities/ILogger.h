#pragma once

#include "../Include/Common.h"
#include <string>
#include <sstream>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <ctime>

namespace APLG {

/**
 * @brief Log severity levels
 */
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

/**
 * @brief Interface for logging system
 * 
 * Provides thread-safe logging with multiple severity levels
 * and output destinations (console, file, etc.)
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a message
     * @param level Severity level
     * @param message Message to log
     */
    virtual void log(LogLevel level, const std::string& message) = 0;
    
    /**
     * @brief Set the minimum log level
     * @param level Minimum level to log
     */
    virtual void setLogLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current log level
     * @return Current log level
     */
    virtual LogLevel getLogLevel() const = 0;
    
    /**
     * @brief Enable or disable file logging
     * @param enable true to enable file logging
     * @param filename Log file path
     */
    virtual void enableFileLogging(bool enable, const std::string& filename = "aplg.log") = 0;
    
    /**
     * @brief Enable or disable console logging
     * @param enable true to enable console logging
     */
    virtual void enableConsoleLogging(bool enable) = 0;
};

/**
 * @brief Concrete implementation of ILogger
 * 
 * Thread-safe logger with console and file output support.
 * Implements Singleton pattern for global access.
 */
class Logger : public ILogger {
public:
    static Logger& instance() {
        static Logger inst;
        return inst;
    }
    
    void log(LogLevel level, const std::string& message) override {
        if (level < m_logLevel) return;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::string formattedMessage = formatMessage(level, message);
        
        if (m_consoleEnabled) {
            writeToConsole(level, formattedMessage);
        }
        
        if (m_fileEnabled && m_file.is_open()) {
            m_file << formattedMessage << std::endl;
        }
    }
    
    void setLogLevel(LogLevel level) override {
        m_logLevel = level;
    }
    
    LogLevel getLogLevel() const override {
        return m_logLevel;
    }
    
    void enableFileLogging(bool enable, const std::string& filename = "aplg.log") override {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_file.is_open()) {
            m_file.close();
        }
        
        m_fileEnabled = enable;
        if (enable) {
            m_file.open(filename, std::ios::out | std::ios::app);
            if (!m_file.is_open()) {
                std::cerr << "Failed to open log file: " << filename << std::endl;
                m_fileEnabled = false;
            }
        }
    }
    
    void enableConsoleLogging(bool enable) override {
        m_consoleEnabled = enable;
    }
    
    // Convenience methods
    void debug(const std::string& message) { log(LogLevel::Debug, message); }
    void info(const std::string& message) { log(LogLevel::Info, message); }
    void warning(const std::string& message) { log(LogLevel::Warning, message); }
    void error(const std::string& message) { log(LogLevel::Error, message); }
    void fatal(const std::string& message) { log(LogLevel::Fatal, message); }
    
private:
    Logger() 
        : m_logLevel(LogLevel::Info)
        , m_consoleEnabled(true)
        , m_fileEnabled(false)
    {
    }
    
    ~Logger() override {
        if (m_file.is_open()) {
            m_file.close();
        }
    }
    
    std::string formatMessage(LogLevel level, const std::string& message) {
        std::stringstream ss;
        ss << "[" << getTimestamp() << "] "
           << "[" << getLevelString(level) << "] "
           << message;
        return ss.str();
    }
    
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
            default:                return "UNKNOWN";
        }
    }
    
    void writeToConsole(LogLevel level, const std::string& message) {
        if (level >= LogLevel::Error) {
            std::cerr << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }
    
    LogLevel m_logLevel;
    bool m_consoleEnabled;
    bool m_fileEnabled;
    std::ofstream m_file;
    std::mutex m_mutex;
};

// Convenience macros
#define LOG_DEBUG(msg)   APLG::Logger::instance().debug(msg)
#define LOG_INFO(msg)    APLG::Logger::instance().info(msg)
#define LOG_WARNING(msg) APLG::Logger::instance().warning(msg)
#define LOG_ERROR(msg)   APLG::Logger::instance().error(msg)
#define LOG_FATAL(msg)   APLG::Logger::instance().fatal(msg)

} // namespace APLG
