# Testing configuration
enable_testing()

# Google Test setup (if available)
find_package(GTest QUIET)

if(GTest_FOUND)
    add_executable(GenerationTests
        Generation/CellularAutomataTest.cpp
        Generation/RandomWalkTest.cpp
        Generation/LevelValidationTest.cpp
    )
    
    target_link_libraries(GenerationTests
        GTest::GTest
        GTest::Main
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(GenerationTests PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME GenerationTests COMMAND GenerationTests)
    
    # Stress test
    add_executable(GenerationStressTest
        Generation/GenerationStressTest.cpp
    )
    
    target_link_libraries(GenerationStressTest
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(GenerationStressTest PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME GenerationStressTest COMMAND GenerationStressTest)
    
    # Utilities test
    add_executable(UtilitiesTest
        Utilities/UtilitiesTest.cpp
    )
    
    target_link_libraries(UtilitiesTest
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(UtilitiesTest PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME UtilitiesTest COMMAND UtilitiesTest)
    
    # Physics test
    add_executable(PhysicsTest
        Physics/PhysicsTest.cpp
    )
    
    target_link_libraries(PhysicsTest
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(PhysicsTest PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME PhysicsTest COMMAND PhysicsTest)
    
    # Player test disabled - needs update for private member access
    # add_executable(PlayerTest
    #     Player/PlayerTest.cpp
    # )
    # 
    # target_link_libraries(PlayerTest
    #     AdaptiveProceduralLevelGeneration
    # )
    # 
    # add_test(NAME PlayerTest COMMAND PlayerTest)
    
    # Generation test
    add_executable(GenerationTest
        Generation/GenerationTest.cpp
    )
    
    target_link_libraries(GenerationTest
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(GenerationTest PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME GenerationTest COMMAND GenerationTest)
    
    # Level validation test
    add_executable(LevelValidationTest
        Generation/LevelValidationTest.cpp
    )
    
    target_link_libraries(LevelValidationTest
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(LevelValidationTest PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME LevelValidationTest COMMAND LevelValidationTest)
    
    # Difficulty test
    add_executable(DifficultyTest
        Difficulty/DifficultyTest.cpp
    )
    
    target_link_libraries(DifficultyTest
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(DifficultyTest PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME DifficultyTest COMMAND DifficultyTest)
    
    # Unified level generation pipeline test
    add_executable(LevelGenerationPipelineTest
        LevelEngine/LevelGenerationPipelineTest.cpp
    )
    
    target_link_libraries(LevelGenerationPipelineTest
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(LevelGenerationPipelineTest PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME LevelGenerationPipelineTest COMMAND LevelGenerationPipelineTest)
    
    # AI test
    add_executable(AITest
        AI/AITest.cpp
    )
    
    target_link_libraries(AITest
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(AITest PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME AITest COMMAND AITest)
    
    # Gameplay test disabled - needs update for private entity classes
    # add_executable(GameplayTest
    #     Gameplay/GameplayTest.cpp
    # )
    # 
    # target_link_libraries(GameplayTest
    #     AdaptiveProceduralLevelGeneration
    # )
    # 
    # add_test(NAME GameplayTest COMMAND GameplayTest)
    
    # Save system test
    add_executable(SaveSystemTest
        Utilities/SaveSystemTest.cpp
    )
    
    target_link_libraries(SaveSystemTest
        AdaptiveProceduralLevelGeneration
    )
    
    if(WIN32)
        set_target_properties(SaveSystemTest PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
    
    add_test(NAME SaveSystemTest COMMAND SaveSystemTest)
    
    # UI test disabled - needs update for new UI implementation
    # add_executable(UITest
    #     UI/UITest.cpp
    # )
    # 
    # target_link_libraries(UITest
    #     AdaptiveProceduralLevelGeneration
    # )
    # 
    # add_test(NAME UITest COMMAND UITest)
else()
    message(STATUS "GTest not found, building simple test executables")
    
    # Simple test executables without GTest
    add_executable(GenerationStressTest
        Generation/GenerationStressTest.cpp
    )
    target_link_libraries(GenerationStressTest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(GenerationStressTest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME GenerationStressTest COMMAND GenerationStressTest)
    
    add_executable(UtilitiesTest
        Utilities/UtilitiesTest.cpp
    )
    target_link_libraries(UtilitiesTest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(UtilitiesTest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME UtilitiesTest COMMAND UtilitiesTest)
    
    add_executable(PhysicsTest
        Physics/PhysicsTest.cpp
    )
    target_link_libraries(PhysicsTest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(PhysicsTest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME PhysicsTest COMMAND PhysicsTest)
    
    add_executable(GenerationTest
        Generation/GenerationTest.cpp
    )
    target_link_libraries(GenerationTest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(GenerationTest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME GenerationTest COMMAND GenerationTest)
    
    add_executable(LevelValidationTest
        Generation/LevelValidationTest.cpp
    )
    target_link_libraries(LevelValidationTest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(LevelValidationTest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME LevelValidationTest COMMAND LevelValidationTest)
    
    add_executable(DifficultyTest
        Difficulty/DifficultyTest.cpp
    )
    target_link_libraries(DifficultyTest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(DifficultyTest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME DifficultyTest COMMAND DifficultyTest)
    
    add_executable(LevelGenerationPipelineTest
        LevelEngine/LevelGenerationPipelineTest.cpp
    )
    target_link_libraries(LevelGenerationPipelineTest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(LevelGenerationPipelineTest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME LevelGenerationPipelineTest COMMAND LevelGenerationPipelineTest)
    
    add_executable(AITest
        AI/AITest.cpp
    )
    target_link_libraries(AITest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(AITest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME AITest COMMAND AITest)
    
    add_executable(SaveSystemTest
        Utilities/SaveSystemTest.cpp
    )
    target_link_libraries(SaveSystemTest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(SaveSystemTest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME SaveSystemTest COMMAND SaveSystemTest)

    add_executable(ReachabilityRegressionTest
        Validation/ReachabilityRegressionTest.cpp
    )
    target_link_libraries(ReachabilityRegressionTest
        AdaptiveProceduralLevelGeneration
    )
    if(WIN32)
        set_target_properties(ReachabilityRegressionTest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    add_test(NAME ReachabilityRegressionTest COMMAND ReachabilityRegressionTest)
endif()

