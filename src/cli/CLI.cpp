#include <cli/Parser.hpp>

#include <diophantus/Solver.hpp>
#include <diophantus/Validator.hpp>
#include <diophantus/model/Solution.hpp>
#include <diophantus/model/numeric/GmpBigInt.hpp>
#include <diophantus/model/EquationSystem.hpp>

#include <logging.h>

#include <argparse/argparse.hpp>

#include <iostream>
#include <stdexcept>
#include <string>


int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("diophantus", "1.0.0", argparse::default_arguments::help);

    program.add_argument("filename")
        .help("specify the input file")
        .required();

    program.add_argument("-v", "--verbosity")
        .help("logging verbosity level")
        .scan<'i', int>()
        .default_value(0);

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }
    catch (const std::invalid_argument& err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    auto inputFile = program.get<std::string>("filename");
    std::cout << inputFile << std::endl;

    // std::cout << "verbosity level: " << program.get<int>("--verbosity") << std::endl;

    cli::Parser parser;
    using NumT = diophantus::model::numeric::GmpBigInt;
    std::filesystem::path inputPath{inputFile};
    diophantus::model::EquationSystem<NumT> equationSystem = parser.parse<NumT>(inputPath);
    
    using Solver = diophantus::Solver<NumT>;
    Solver solver(equationSystem);

    std::optional<diophantus::model::Solution<NumT>> solution = solver.solve();
    if (solution.has_value())
    {
        LOG_INFO << "Solution found. Checking solution:";
        for (const auto& x : solution.value().assignments)
        {
            LOG_INFO << x;
        }
        diophantus::Validator<NumT> validator(equationSystem);
        if (validator.isValidSolution(solution.value()))
        {
            LOG_INFO << "Solution validated.";
        }
        else
        {
            LOG_ERROR << "ERROR! Solution is invalid.";
            std::exit(1);
        }
    }
    else
    {
        LOG_INFO << "No solution found.";
    }

    return 0;
}
