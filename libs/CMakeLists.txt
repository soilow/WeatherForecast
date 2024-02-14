add_library(application app.cpp app.h)

target_link_libraries(application PRIVATE utils)
target_link_libraries(application PRIVATE images)
target_link_libraries(application PRIVATE cpr::cpr)
target_link_libraries(application PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(application PUBLIC ${PROJECT_SOURCE_DIR})
