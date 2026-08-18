#pragma once

#include "../Include/Common.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace APLG {

/**
 * @brief CSV row representation
 * 
 * Represents a single row in a CSV file with column access.
 */
class CsvRow {
public:
    CsvRow() = default;
    
    explicit CsvRow(const std::vector<std::string>& values) 
        : m_values(values) 
    {
    }
    
    /**
     * @brief Get number of columns
     */
    size_t size() const { return m_values.size(); }
    
    /**
     * @brief Get value by column index
     */
    std::string operator[](size_t index) const {
        if (index < m_values.size()) {
            return m_values[index];
        }
        return "";
    }
    
    /**
     * @brief Get value by column name (requires header)
     */
    std::string get(const std::string& columnName, const std::vector<std::string>& headers) const {
        auto it = std::find(headers.begin(), headers.end(), columnName);
        if (it != headers.end()) {
            size_t index = std::distance(headers.begin(), it);
            return (*this)[index];
        }
        return "";
    }
    
    /**
     * @brief Get value as integer
     */
    int32 getInt(size_t index) const {
        try {
            return std::stoi((*this)[index]);
        } catch (...) {
            return 0;
        }
    }
    
    /**
     * @brief Get value as float
     */
    float32 getFloat(size_t index) const {
        try {
            return std::stof((*this)[index]);
        } catch (...) {
            return 0.0f;
        }
    }
    
    /**
     * @brief Get value as double
     */
    float64 getDouble(size_t index) const {
        try {
            return std::stod((*this)[index]);
        } catch (...) {
            return 0.0;
        }
    }
    
    /**
     * @brief Get value as boolean
     */
    bool getBool(size_t index) const {
        std::string val = (*this)[index];
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        return val == "true" || val == "1" || val == "yes";
    }
    
    /**
     * @brief Add a value to the row
     */
    void push_back(const std::string& value) {
        m_values.push_back(value);
    }
    
    /**
     * @brief Convert row to CSV string
     */
    std::string toString() const {
        std::stringstream ss;
        for (size_t i = 0; i < m_values.size(); ++i) {
            if (i > 0) ss << ",";
            
            // Escape if contains comma or quote
            if (m_values[i].find(',') != std::string::npos || 
                m_values[i].find('"') != std::string::npos) {
                ss << "\"" << escapeString(m_values[i]) << "\"";
            } else {
                ss << m_values[i];
            }
        }
        return ss.str();
    }
    
private:
    std::string escapeString(const std::string& str) const {
        std::string result;
        for (char c : str) {
            if (c == '"') {
                result += "\"\"";
            } else {
                result += c;
            }
        }
        return result;
    }
    
    std::vector<std::string> m_values;
};

/**
 * @brief CSV file reader/writer
 * 
 * Handles reading and writing CSV files with proper escaping.
 */
class CsvFile {
public:
    CsvFile() = default;
    
    /**
     * @brief Load CSV from file
     */
    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        m_rows.clear();
        std::string line;
        
        // Read header
        if (std::getline(file, line)) {
            m_headers = parseLine(line);
        }
        
        // Read data rows
        while (std::getline(file, line)) {
            if (!line.empty()) {
                m_rows.push_back(CsvRow(parseLine(line)));
            }
        }
        
        return true;
    }
    
    /**
     * @brief Save CSV to file
     */
    bool save(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        if (!m_headers.empty()) {
            CsvRow headerRow(m_headers);
            file << headerRow.toString() << "\n";
        }
        
        // Write data rows
        for (const auto& row : m_rows) {
            file << row.toString() << "\n";
        }
        
        return true;
    }
    
    /**
     * @brief Set headers
     */
    void setHeaders(const std::vector<std::string>& headers) {
        m_headers = headers;
    }
    
    /**
     * @brief Get headers
     */
    const std::vector<std::string>& getHeaders() const {
        return m_headers;
    }
    
    /**
     * @brief Add a row
     */
    void addRow(const CsvRow& row) {
        m_rows.push_back(row);
    }
    
    /**
     * @brief Add a row from values
     */
    void addRow(const std::vector<std::string>& values) {
        m_rows.push_back(CsvRow(values));
    }
    
    /**
     * @brief Get number of rows
     */
    size_t rowCount() const {
        return m_rows.size();
    }
    
    /**
     * @brief Get row by index
     */
    const CsvRow& getRow(size_t index) const {
        static CsvRow emptyRow;
        if (index < m_rows.size()) {
            return m_rows[index];
        }
        return emptyRow;
    }
    
    /**
     * @brief Get all rows
     */
    const std::vector<CsvRow>& getRows() const {
        return m_rows;
    }
    
    /**
     * @brief Clear all data
     */
    void clear() {
        m_headers.clear();
        m_rows.clear();
    }
    
    /**
     * @brief Find rows matching a condition
     */
    std::vector<CsvRow> findRows(std::function<bool(const CsvRow&)> predicate) const {
        std::vector<CsvRow> result;
        for (const auto& row : m_rows) {
            if (predicate(row)) {
                result.push_back(row);
            }
        }
        return result;
    }
    
private:
    std::vector<std::string> parseLine(const std::string& line) const {
        std::vector<std::string> result;
        std::string current;
        bool inQuotes = false;
        
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            if (inQuotes) {
                if (c == '"' && i + 1 < line.length() && line[i + 1] == '"') {
                    current += '"';
                    i++; // Skip next quote
                } else if (c == '"') {
                    inQuotes = false;
                } else {
                    current += c;
                }
            } else {
                if (c == '"') {
                    inQuotes = true;
                } else if (c == ',') {
                    result.push_back(current);
                    current.clear();
                } else {
                    current += c;
                }
            }
        }
        
        result.push_back(current);
        return result;
    }
    
    std::vector<std::string> m_headers;
    std::vector<CsvRow> m_rows;
};

/**
 * @brief Helper for writing player analytics to CSV
 */
class AnalyticsWriter {
public:
    /**
     * @brief Create a new analytics CSV file with headers
     */
    static bool createAnalyticsFile(const std::string& filename) {
        CsvFile csv;
        csv.setHeaders({
            "timestamp",
            "deaths",
            "damage_taken",
            "completion_time",
            "jump_accuracy",
            "enemy_hit_rate",
            "platform_misses",
            "idle_time",
            "reaction_time",
            "lives_remaining",
            "checkpoint_usage",
            "coins_collected",
            "enemies_defeated",
            "difficulty_level",
            "skill_score"
        });
        return csv.save(filename);
    }
    
    /**
     * @brief Append player metrics to analytics file
     */
    static bool appendMetrics(const std::string& filename, 
                              const PlayerMetrics& metrics,
                              DifficultyLevel difficulty,
                              const SkillScore& skill) {
        CsvFile csv;
        if (!csv.load(filename)) {
            return false;
        }
        
        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        csv.addRow({
            ss.str(),
            std::to_string(metrics.deaths),
            std::to_string(metrics.damageTaken),
            std::to_string(metrics.completionTime),
            std::to_string(metrics.jumpAccuracy),
            std::to_string(metrics.enemyHitRate),
            std::to_string(metrics.platformMisses),
            std::to_string(metrics.idleTime),
            std::to_string(metrics.reactionTime),
            std::to_string(metrics.livesRemaining),
            std::to_string(metrics.checkpointUsage),
            std::to_string(metrics.coinsCollected),
            std::to_string(metrics.enemiesDefeated),
            std::to_string(static_cast<int>(difficulty)),
            std::to_string(skill.overall)
        });
        
        return csv.save(filename);
    }
};

} // namespace APLG
