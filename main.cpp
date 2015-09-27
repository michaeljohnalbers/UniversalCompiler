/**
 * @file main.cpp
 * @brief Entry point of program
 *
 * @author Michael Albers
 */

#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include "ErrorWarningTracker.h"
#include "GrammarAnalyzer.h"
#include "Scanner.h"

int main(int argc, char **argv)
{
  try
  {
    bool printGrammar = false;
    bool printTokens = false;

    extern int optind;

    while (true)
    {
      enum Option
      {
        Grammar,
        Tokens,
      };

      static struct option options[] = {
        {"grammar", no_argument, 0, Grammar},
        {"tokens", no_argument, 0, Tokens},
        {0, 0, 0,  0 }
      };

      int optionIndex = 0;
      auto c = ::getopt_long(argc, argv, "",
                             options, &optionIndex);
      if (c == -1)
        break;

      switch (c) {
        case Grammar:
          printGrammar = true;
          break;

        case Tokens:
          printTokens = true;
          break;

        default:
          throw std::runtime_error("");
      }
    }

    if (argc - optind != 2)
    {
      throw std::runtime_error("No input files provided.");
    }

    std::string grammarFile(argv[optind + 0]);
    std::string sourceFile(argv[optind + 1]);

    GrammarAnalyzer grammar(grammarFile);
    ErrorWarningTracker ewTracker(sourceFile);
    Scanner scanner(sourceFile, ewTracker);

    if (printGrammar)
    {
      std::cout << "Grammar Information" << std::endl
                << "===================" << std::endl
                << grammar << std::endl;
    }

    if (printTokens)
    {
      std::cout << "Tokens from " << sourceFile << std::endl
                << "=====================" << std::endl;
      DriverTable::TokenCode tokenCode;
      std::string token;
      do
      {
        scanner.scan(token, tokenCode);
        std::cout << "Token: " << DriverTable::getDescription(tokenCode)
                  << " (\"" << token << "\")" << std::endl;
      }
      while (tokenCode != DriverTable::TokenCode::EofSym);
    }
  }
  catch (const std::exception &exception)
  {
    if (std::string(exception.what()).size() > 0)
    {
      std::cerr << argv[0] << ": error: " << exception.what() << std::endl;
    }
    std::cerr << "Usage: " << argv[0]
              << " [OPTIONS...] [grammer file] [source file]" << std::endl
              << " --tokens  print tokens in source file" << std::endl
              << " --grammar print grammar information" << std::endl;
    return 1;
  }

  return 0;
}
