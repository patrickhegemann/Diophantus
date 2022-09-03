#pragma once

#include "diophantus/model/Equation.hpp"
#include "diophantus/model/Sum.hpp"
#include "diophantus/model/Term.hpp"
#include "diophantus/model/Variable.hpp"
#include "diophantus/model/util.hpp"
#include <cstddef>
#include <diophantus/model/EquationSystem.hpp>
#include <diophantus/model/numeric/BigInt.hpp>

#include <logging.hpp>

#include <functional>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

namespace cli
{
    template <typename TokenT>
    TokenT default_convert(const std::string&);

    template<> inline int default_convert<int>(const std::string& str)
    {
        return std::stoi(str);
    }

    template<> inline std::string default_convert<std::string>(const std::string& str)
    {
        return str;
    }

    template<typename TokenT=std::string, TokenT Convert(const std::string&)=default_convert<TokenT>>
    std::vector<TokenT> split_convert(std::string& line, const std::string& delimiter=" ")
    {
        size_t pos = 0;
        std::vector<TokenT> tokens;

        while ((pos = line.find(delimiter)) != std::string::npos)
        {
            tokens.push_back(Convert(line.substr(0, pos)));
            line.erase(0, pos + delimiter.length());
        }
        tokens.push_back(Convert(line));

        return tokens;
    }

    class Parser
    {
    public:
        template<diophantus::model::numeric::BigInt NumT>
        std::optional<diophantus::model::EquationSystem<NumT>> parse(std::filesystem::path path)
        {
            std::vector<std::shared_ptr<diophantus::model::Variable>> variables;
            std::vector<diophantus::model::Equation<NumT>> equations;

            std::ifstream file;
            file.exceptions(std::ifstream::badbit);

            try
            {
                file.open(path);

                std::string firstLine;
                std::getline(file, firstLine);

                std::vector<int> header = split_convert<int>(firstLine);
                
                size_t nEquations = header[0];
                size_t nVariables = header[1];

                variables = diophantus::model::make_variables(nVariables);

                size_t nAddedEquations = 0;
                for (std::string line; std::getline(file, line); )
                {
                    std::string lineCopy(line);
                    std::vector<int> tokens = split_convert<int>(lineCopy);

                    if (tokens.empty())
                    {
                        LOG_WARNING << "Empty line in problem file";
                        continue;
                    }

                    nAddedEquations++;
                    if (nAddedEquations > nEquations)
                    {
                        LOG_WARNING << "More equations in problem file than specified (" << nEquations << ")";
                        break;
                    }

                    size_t nTerms = tokens[0];
                    size_t nAddedTerms = 0;

                    std::vector<diophantus::model::Term<NumT>> terms;
                    NumT rightSide(0);
                    for (uint i = 1; i < tokens.size(); i+=2)
                    {
                        NumT coefficient = NumT(tokens[i]);
                        size_t varIndex = tokens[i+1];
                        if (varIndex == 0)
                        {
                            rightSide = coefficient;
                            nAddedTerms++;
                        }
                        else
                        {
                            terms.push_back({coefficient, variables[varIndex-1]});
                            nAddedTerms++;
                        }
                    }

                    if (nAddedTerms != nTerms)
                    {
                        LOG_WARNING << "Wrong number of terms (" << nAddedTerms << "/" << nTerms
                                    << ") specified for equation: " << line;
                    }

                    diophantus::model::Sum<NumT> sum{terms};
                    diophantus::model::Equation<NumT> equation{sum, rightSide};
                    equations.push_back(equation);
                }

                if (nAddedEquations != nEquations)
                {
                    LOG_WARNING << "Wrong number of equations specified. "
                                << "Found: " << nAddedEquations << ". "
                                << "Specified: " << nEquations;
                }

                file.close();
            }
            catch (const std::ifstream::failure& e)
            {
                LOG_ERROR << "Error reading file: " << path;
                LOG_ERROR << e.what();
                return std::nullopt;
            }

            return diophantus::model::EquationSystem<NumT>(variables, equations);
        }
    };
}
