add_library(dio_common
    logging.hpp
)

set_target_properties(dio_common
    PROPERTIES
        LINKER_LANGUAGE CXX
)

target_link_libraries(dio_common
    Boost::log
)

target_include_directories(dio_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
