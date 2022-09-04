#include <cli/Parser.hpp>

#include <diophantus/Solver.hpp>
#include <diophantus/Validator.hpp>
#include <diophantus/model/Solution.hpp>
#include <diophantus/model/numeric/GmpBigInt.hpp>
#include <diophantus/model/EquationSystem.hpp>

#include <logging.hpp>

#include <argparse/argparse.hpp>

#include <optional>
#include <iostream>
#include <exception>
#include <string>

argparse::ArgumentParser parseArguments(int argc, char *argv[])
{
    argparse::ArgumentParser program("diophantus", "1.0.0", argparse::default_arguments::help);

    program.add_argument("filename")
        .help("specify the input file")
        .required();

    program.add_argument("-v", "--verbosity")
        .help("logging verbosity level (between 0 and 5)")
        .scan<'i', unsigned int>()
        .default_value(3u);

    program.add_argument("--validate")
        .help("enable validation of the solution")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--progress")
        .help("show progress of the algorithm while solving")
        .default_value(false)
        .implicit_value(true);
    
    try
    {
        program.parse_args(argc, argv);
        setLoggingLevel(program.get<unsigned int>("--verbosity"));
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl << program;
        std::exit(1);
    }

    return program;
}

int main(int argc, char *argv[])
{
    argparse::ArgumentParser args = parseArguments(argc, argv);

    using NumT = diophantus::model::numeric::GmpBigInt;
    using EquationSystem = diophantus::model::EquationSystem<NumT>;
    using Solver = diophantus::Solver<NumT>;
    using Solution = diophantus::model::Solution<NumT>;
    using Validator = diophantus::Validator<NumT>;

    // Parse input file to get equation system to solve
    LOG_INFO << "Parsing input file.";
    cli::Parser parser;
    std::filesystem::path inputPath {args.get<std::string>("filename")};
    std::optional<EquationSystem> equationSystem {parser.parse<NumT>(inputPath)};

    if (!equationSystem.has_value())
    {
        LOG_FATAL << "No input equation system. Exiting.";
        std::exit(1);
    }
    
    // Solve equation system and output the result
    Solver solver(equationSystem.value(), Solver::Parameters{
        .doShowProgress = args.get<bool>("--progress")
    });

    LOG_INFO << "Solving equation system.";
    std::optional<Solution> solution = solver.solve();
    if (solution.has_value())
    {
        LOG_INFO << "Solution found:";
        for (const auto& x : solution.value().assignments)
        {
            LOG_INFO << x;
        }

        bool doValidation = args.get<bool>("--validate");
        if (doValidation)
        {
            LOG_INFO << "Checking solution:";
            Validator validator(equationSystem.value());
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
    }
    else
    {
        LOG_INFO << "No solution found.";
    }

    return 0;
}
