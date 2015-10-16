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
#include "Grammar.h"
#include "GrammarAnalyzer.h"
#include "Parser.h"
#include "PredictTable.h"
#include "Scanner.h"
#include "ScannerTable.h"

int main(int argc, char **argv)
{
  try
  {
    bool printGrammar = false;
    bool printParse = false;
    bool printPredictTable = false;
    bool printTokens = false;

    extern int optind;

    while (true)
    {
      enum Option
      {
        Grammar,
        Parse,
        PredictTable,
        Tokens,
      };

      static struct option options[] = {
        {"grammar", no_argument, 0, Grammar},
        {"parse", no_argument, 0, Parse},
        {"predict-table", no_argument, 0, PredictTable},
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

        case Parse:
          printParse = true;
          break;

        case PredictTable:
          printPredictTable = true;
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

    ErrorWarningTracker ewTracker(sourceFile);
    ScannerTable scannerTable;

    Grammar grammar(grammarFile, ewTracker, scannerTable);
    GrammarAnalyzer grammarAnalyzer(grammar);
    PredictTable predictTable(grammar);

    if (printGrammar)
    {
      std::cout << grammar << std::endl
                << grammarAnalyzer << std::endl;
    }

    if (printPredictTable)
    {
      std::cout << predictTable << std::endl;
    }

    Scanner scanner(sourceFile, scannerTable, ewTracker, printTokens);

    Parser parser(scanner, grammar, predictTable, ewTracker, printParse);
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
              << " --grammar print grammar information" << std::endl
              << " --predict-table print predict table" << std::endl;
    return 1;
  }

  return 0;
}
