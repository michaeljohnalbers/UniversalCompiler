/**
 * @file Parser.cpp
 * @brief Implementation of Parser class
 *
 * @author Michael Albers
 */

#include <iomanip>
#include <iostream>
#include <sstream>

#include "ErrorWarningTracker.h"
#include "Grammar.h"
#include "Lambda.h"
#include "NonTerminalSymbol.h"
#include "Parser.h"
#include "PredictTable.h"
#include "Production.h"
#include "Scanner.h"
#include "Token.h"

//*******************************************************
// Parser::Parser
//*******************************************************
Parser::Parser(Scanner &theScanner,
               const Grammar &theGrammar,
               const PredictTable &thePredictTable,
               ErrorWarningTracker &theEWTracker,
               bool thePrintParse) :
  myEWTracker(theEWTracker),
  myGrammar(theGrammar),
  myPredictTable(thePredictTable),
  myPrintParse(thePrintParse),
  myScanner(theScanner)
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

  myStack.push(myGrammar.getStartSymbol());

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

        auto production = myGrammar.getProduction(productionNumber);
        myStack.pop();
        auto rhs = production->getRHS();
        auto rhsIter = rhs.rbegin();
        while (rhsIter != rhs.rend())
        {
          auto rhsSymbol = *rhsIter;
          if (!(*rhsSymbol == *Lambda::getInstance()))
          {
            myStack.push(rhsSymbol);
          }
          ++rhsIter;
        }
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
    else
    {
      if (*expectedSymbol == *(token.getTerminal()))
      {
        predictValue << "Match";

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

    if (myPrintParse && ! myEWTracker.hasError())
    {
      std::cout << std::setw(ACTION_WIDTH) << predictValue.str() << " | "
                << std::setw(TOKENS_WIDTH) << remainingTokens.str() << " | "
                << std::setw(STACK_WIDTH) << stackContents.str()
                << std::endl;
    }
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
