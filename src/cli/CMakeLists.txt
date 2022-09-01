add_executable(diophantus_cli
    CLI.cpp

    Parser.hpp
)

target_link_libraries(diophantus_cli
    dio_common
    diophantus
    argparse
)
