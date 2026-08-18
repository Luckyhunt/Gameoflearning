#include "../../Utilities/ILogger.h"
#include "../../Utilities/Json.h"
#include "../../Utilities/Csv.h"
#include <iostream>

using namespace APLG;

/**
 * @brief Test suite for Utilities module
 */
int main() {
    std::cout << "=== Utilities Module Test ===" << std::endl;
    
    // Test Logger
    std::cout << "\n--- Testing Logger ---" << std::endl;
    Logger::instance().setLogLevel(LogLevel::Debug);
    Logger::instance().enableConsoleLogging(true);
    
    LOG_DEBUG("This is a debug message");
    LOG_INFO("This is an info message");
    LOG_WARNING("This is a warning message");
    LOG_ERROR("This is an error message");
    
    // Test JSON
    std::cout << "\n--- Testing JSON ---" << std::endl;
    
    // Create a JSON object
    JsonValue playerData;
    playerData["name"] = "TestPlayer";
    playerData["score"] = 1000;
    playerData["is_active"] = true;
    
    JsonValue::Array inventory;
    inventory.push_back("coin");
    inventory.push_back("key");
    playerData["inventory"] = inventory;
    
    std::string jsonStr = playerData.serialize(2);
    std::cout << "Serialized JSON:" << std::endl;
    std::cout << jsonStr << std::endl;
    
    // Parse JSON
    JsonValue parsed = JsonParser::parse(jsonStr);
    std::cout << "\nParsed name: " << parsed["name"].getString() << std::endl;
    std::cout << "Parsed score: " << parsed["score"].getNumber() << std::endl;
    
    // Test CSV
    std::cout << "\n--- Testing CSV ---" << std::endl;
    
    CsvFile csv;
    csv.setHeaders({"name", "score", "level"});
    csv.addRow({"Player1", "100", "5"});
    csv.addRow({"Player2", "200", "10"});
    csv.addRow({"Player3", "150", "7"});
    
    if (csv.save("test_output.csv")) {
        std::cout << "CSV file saved successfully" << std::endl;
    }
    
    // Load and read CSV
    CsvFile loadedCsv;
    if (loadedCsv.load("test_output.csv")) {
        std::cout << "CSV file loaded successfully" << std::endl;
        std::cout << "Row count: " << loadedCsv.rowCount() << std::endl;
        std::cout << "First row name: " << loadedCsv.getRow(0).get("name", loadedCsv.getHeaders()) << std::endl;
    }
    
    // Test Analytics Writer
    std::cout << "\n--- Testing Analytics Writer ---" << std::endl;
    
    if (AnalyticsWriter::createAnalyticsFile("analytics_test.csv")) {
        std::cout << "Analytics file created" << std::endl;
        
        PlayerMetrics metrics;
        metrics.deaths = 5;
        metrics.completionTime = 120.5f;
        metrics.jumpAccuracy = 0.85f;
        
        SkillScore skill;
        skill.overall = 0.75f;
        
        if (AnalyticsWriter::appendMetrics("analytics_test.csv", metrics, DifficultyLevel::Normal, skill)) {
            std::cout << "Metrics appended successfully" << std::endl;
        }
    }
    
    std::cout << "\n=== All Utilities Tests Complete ===" << std::endl;
    
    return 0;
}
