find_path(GMP_INCLUDE_DIR
    NAMES
        gmp.h gmpxx.h 
    PATHS
        /usr/include /usr/local/include
)

find_library(GMP_LIBRARY
    NAMES
        gmp libgmp 
    PATHS
        /usr/lib /usr/local/lib
)

if(GMP_INCLUDE_DIR AND GMP_LIBRARY)
    get_filename_component(GMP_LIBRARY_DIR ${GMP_LIBRARY} PATH)
    set(GMP_FOUND TRUE)
endif()

if(GMP_FOUND AND NOT GMP_FIND_QUIETLY)
    message(STATUS "Found GMP: ${GMP_LIBRARY}")
elseif(GMP_FOUND AND GMP_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find GMP. https://gmplib.org/")
endif()
