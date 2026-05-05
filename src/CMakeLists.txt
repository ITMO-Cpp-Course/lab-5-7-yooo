add_library(resource_core
        Document.cpp
        DocumentBuilder.cpp
        InvertedIndex.cpp
)
target_include_directories(resource_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)