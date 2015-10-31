/**
 * @file Parser.cpp
 * @brief Implementation of Parser class
 *
 * @author Michael Albers
 */

#include <deque>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "ActionSymbol.h"
#include "EOPSymbol.h"
#include "ErrorWarningTracker.h"
#include "Grammar.h"
#include "GrammarAnalyzer.h"
#include "Lambda.h"
#include "NonTerminalSymbol.h"
#include "Parser.h"
#include "PredictTable.h"
#include "Production.h"
#include "Scanner.h"
#include "SemanticRecord.h"
#include "SemanticRoutines.h"
#include "SemanticStack.h"
#include "Token.h"

//*******************************************************
// Parser::Parser
//*******************************************************
Parser::Parser(Scanner &theScanner,
               const Grammar &theGrammar,
               const PredictTable &thePredictTable,
               SemanticStack &theSemanticStack,
               SemanticRoutines &theSemanticRoutines,
               ErrorWarningTracker &theEWTracker,
               bool thePrintParse,
               bool thePrintGeneration) :
  myEWTracker(theEWTracker),
  myGrammar(theGrammar),
  myPredictTable(thePredictTable),
  myPrintGeneration(thePrintGeneration),
  myPrintParse(thePrintParse),
  myScanner(theScanner),
  mySemanticRoutines(theSemanticRoutines),
  mySemanticStack(theSemanticStack)
{
  parse();
}

//*******************************************************
// Parser::parse
//*******************************************************
void Parser::parse()
{
  static const uint32_t ACTION_WIDTH = 17;
  static const uint32_t STACK_WIDTH = 1;

  // Make the second column the correct maximum size, always.
  std::ostringstream dummy1;
  Token dummy2;
  printTokens(dummy1, dummy2);
  static uint32_t TOKENS_WIDTH = dummy1.str().size();

  if (myPrintParse)
  {
    std::cout << std::left
              << std::setw(ACTION_WIDTH) << "Parser Action" << " | "
              << std::setw(TOKENS_WIDTH) << "Remaining Tokens" << " | "
              << std::setw(STACK_WIDTH) << "Stack" << std::endl
              << std::right;
  }

  mySemanticStack.initialize();
  myStack.push(myGrammar.getStartSymbol());

  printState();

  Token token{myScanner.scan()};

  while (myStack.size() > 0)
  {
    // variables for printing the parse
    std::ostringstream stackContents;
    std::ostringstream remainingTokens;
    std::ostringstream predictValue;

    if (myPrintParse)
    {
      printTokens(remainingTokens, token);
      printStack(stackContents, myStack);
    }

    auto expectedSymbol = myStack.top();

    if (typeid(*expectedSymbol.get()) == typeid(NonTerminalSymbol))
    {
      auto productionNumber = myPredictTable.getProductionNumber(
        expectedSymbol, token.getTerminal());

      if (productionNumber > 0)
      {
        predictValue << "Predict(" << productionNumber << ")";

        myStack.pop();
        myStack.push(mySemanticStack.getEOPSymbol());

        auto production = myGrammar.getProduction(productionNumber);
        auto rhs = production->getRHS();
        auto rhsIter = rhs.rbegin();
        uint32_t numberGrammarSymbols = 0;
        while (rhsIter != rhs.rend())
        {
          std::shared_ptr<Symbol> rhsSymbol = *rhsIter;
          if (GrammarAnalyzer::isGrammarSymbol(rhsSymbol))
          {
            ++numberGrammarSymbols;
          }

          if (!(*rhsSymbol == *Lambda::getInstance()))
          {
            myStack.push(rhsSymbol);
          }
          ++rhsIter;
        }
        mySemanticStack.expand(numberGrammarSymbols);
      }
      else
      {
        std::ostringstream error;
        error << "No production found for symbol " << *expectedSymbol
              << " and token " << *(token.getTerminal()) << ".";
        myEWTracker.reportError(token.getLine(), token.getColumn(),
                                error.str());

        // Error recovery
        myStack.pop(); // Move past the bad symbol.
      }
    }
    else if (typeid(*expectedSymbol.get()) == typeid(TerminalSymbol))
    {
      if (*expectedSymbol == *(token.getTerminal()))
      {
        predictValue << "Match";

        mySemanticStack.replaceAtCurrentIndex(SemanticRecord(
                                                PlaceholderRecord(token)));

        myStack.pop();
        token = myScanner.scan();
      }
      else
      {
        std::ostringstream error;
        error << "Expected " << *expectedSymbol << ", instead found "
              << *(token.getTerminal()) << ".";
        myEWTracker.reportError(token.getLine(), token.getColumn(),
                                error.str());

        // Error recovery
        myStack.pop(); // Move past the bad symbol.
      }
    }
    else if (typeid(*expectedSymbol.get()) == typeid(ActionSymbol))
    {
      myStack.pop();
      mySemanticRoutines.executeSemanticRoutine(expectedSymbol);
    }
    else if (typeid(*expectedSymbol.get()) == typeid(EOPSymbol))
    {
      mySemanticStack.restore(expectedSymbol);
      myStack.pop();
    }

    if (myPrintParse && ! myEWTracker.hasError())
    {
      std::cout << std::setw(ACTION_WIDTH) << predictValue.str() << " | "
                << std::setw(TOKENS_WIDTH) << remainingTokens.str() << " | "
                << std::setw(STACK_WIDTH) << stackContents.str()
                << std::endl;
    }

    printState();
  }
}

//*******************************************************
// Parser::printStack
//*******************************************************
void Parser::printStack(std::ostream &theOS,
                        std::stack<std::shared_ptr<Symbol>> theStack)
{
  while (theStack.size() > 0)
  {
    theOS << *(theStack.top());
    theStack.pop();
    if (theStack.size() > 0)
    {
      theOS << " ";
    }
  }
}

//*******************************************************
// Parser::printState
//*******************************************************
void Parser::printState() noexcept
{
  if (myPrintGeneration)
  {
    auto remainingTokens(myScanner.getRemainingTokens());
    auto semanticStack(mySemanticStack.getStack());
    auto generatedCode(mySemanticRoutines.getCode());

    // Bit of a hack to account for the fact that you cannot
    // iterate over a std::stack.
    std::deque<std::shared_ptr<Symbol>> parseStack;
    while (! myStack.empty())
    {
      parseStack.push_back(myStack.top());
      myStack.pop();
    }

    for (auto iter = parseStack.rbegin(); iter != parseStack.rend(); ++iter)
    {
      myStack.push(*iter);
    }

    auto tokenIter = remainingTokens.begin();
    auto semanticIter = semanticStack.begin();
    semanticIter++; // Bottom element is a placeholder (skip it)
    auto generatedIter = generatedCode.begin();
    auto parseIter = parseStack.begin();

    auto remainingTokensCheck = [&]()->bool
    {
      return tokenIter != remainingTokens.end();
    };
    auto remainingSemanticCheck = [&]()->bool
    {
      return semanticIter != semanticStack.end();
    };
    auto remainingGeneratedCheck = [&]()->bool
    {
      return generatedIter != generatedCode.end();
    };
    auto remainingParseCheck = [&]()->bool
    {
      return parseIter != parseStack.end();
    };
    auto remainingData = [&]()->bool
    {
      bool remainingData = (remainingTokensCheck() ||
                            remainingSemanticCheck() ||
                            remainingParseCheck() ||
                            remainingGeneratedCheck());
      return remainingData;
    };

    // Sized to fit GenInfix action symbol
    static const uint32_t WIDTH = 22;

    auto printDivider = []()
    {
      for (int ii = 0; ii < 4; ++ii)
      {
        std::cout << std::setw(WIDTH+2) << std::setfill('-') << "";
      }
      std::cout << std::endl << std::setfill(' ');
    };

    static bool printedHeader = false;
    if (! printedHeader)
    {
      printedHeader = true;
      std::cout << std::setw(WIDTH) << "Remaining Tokens" << " | "
                << std::setw(WIDTH) << "Parse Stack" << " | "
                << std::setw(WIDTH) << "Semantic Stack" << " | "
                << std::setw(WIDTH) << "Generated Code" << std::endl;
      printDivider();
   }

    // Remaining Input, Parse stack, semantic stack, code
    while (remainingData())
    {
      if (remainingTokensCheck())
      {
        uint32_t usedWidth = 0;
        while (remainingTokensCheck() &&
               usedWidth + (tokenIter->getToken().size() + 1) < WIDTH)
        {
          std::cout << tokenIter->getToken() << " ";
          usedWidth += tokenIter->getToken().size() + 1;
          ++tokenIter;
        }
        while (usedWidth < WIDTH)
        {
          std::cout << " ";
          ++usedWidth;
        }
      }
      else
      {
        std::cout << std::setw(WIDTH) << " ";
      }
      std::cout << " | ";

      if (remainingParseCheck())
      {
        std::cout << std::setw(WIDTH) << **parseIter;
        ++parseIter;
      }
      else
      {
        std::cout << std::setw(WIDTH) << " ";
      }
      std::cout << " | ";

      if (remainingSemanticCheck())
      {
        std::cout << std::setw(WIDTH) << semanticIter->extract();
        ++semanticIter;
      }
      else
      {
        std::cout << std::setw(WIDTH) << " ";
      }
      std::cout << " | ";

      if (remainingGeneratedCheck())
      {
        std::cout << std::setw(WIDTH) << std::left << *generatedIter
                  << std::right;
        ++generatedIter;
      }
      else
      {
        std::cout << std::setw(WIDTH) << " ";
      }

      std::cout << std::endl;
    }

    printDivider();
  }
}

//*******************************************************
// Parser::printTokens
//*******************************************************
void Parser::printTokens(std::ostream &theOS, const Token &theLookAheadToken)
{
  // If prevents last parser step print from printing "$ $" as remaining
  // symbols.
  if (theLookAheadToken.getToken() != "$")
  {
    theOS << theLookAheadToken.getToken();
  }

  for (auto token : myScanner.getRemainingTokens())
  {
    theOS << " " << token.getToken();
  }
}
