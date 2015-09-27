/**
 * @file Production.cpp
 * @brief Implementation of GrammarAnalyzer class
 *
 * @author Michael Albers
 */

#include <cerrno>
#include <iostream>
#include <system_error>

#include "GrammarAnalyzer.h"
#include "NonTerminalSymbol.h"
#include "Production.h"
#include "TerminalSymbol.h"

//*******************************************************
// GrammarAnalyzer::GrammarAnalyzer
//*******************************************************
GrammarAnalyzer::GrammarAnalyzer(const std::string &theFileName)
{
  std::ifstream file;
  openFile(theFileName, file);
  parseFile(file);
}

//*******************************************************
// GrammarAnalyzer::consumeWhitespace
//*******************************************************
void GrammarAnalyzer::consumeWhitespace(std::string &theLine)
{
  auto position = theLine.begin();
  while (true)
  {
    const auto character = *position;
    if (' ' == character || '\t' == character)
    {
      ++position;
    }
    else
    {
      break;
    }
  }

  if (position != theLine.begin())
  {
    theLine.erase(theLine.begin(), position);
  }
}

//*******************************************************
// GrammarAnalyzer::makeSymbol
//*******************************************************
std::shared_ptr<Symbol> GrammarAnalyzer::makeSymbol(
  const std::string &theSymbol)
{
  std::shared_ptr<Symbol> symbol;
  if ('<' == theSymbol[0])
  {
    symbol.reset(new NonTerminalSymbol(theSymbol));
    myNonTerminalSymbols.insert(theSymbol);
  }
  else
  {
    symbol.reset(new TerminalSymbol(theSymbol));
    myTerminalSymbols.insert(theSymbol);
  }
  return symbol;
}

//*******************************************************
// GrammarAnalyzer::openFile
//*******************************************************
void GrammarAnalyzer::openFile(const std::string &theFileName,
                               std::ifstream &theFile)
{
  theFile.open(theFileName, std::ios::in);
  if (!theFile)
  {
    auto localErrno = errno;
    std::string error{"Failed to open grammar file '"};
    error += theFileName + "'";
    throw std::system_error(localErrno, std::system_category(), error);
  }
}

//*******************************************************
// GrammarAnalyzer::parseFile
//*******************************************************
void GrammarAnalyzer::parseFile(std::ifstream &theFile)
{
  uint32_t productionNumber = 0;
  Production *production = nullptr;

  while (! theFile.eof())
  {
    std::string inputLine;
    std::getline(theFile, inputLine);

    // Only process non-empty lines or non-comment lines
    if (inputLine.size() > 0 && '#' != inputLine[0])
    {
      ++productionNumber;

      consumeWhitespace(inputLine);
      std::string lhs(readSymbol(inputLine));
      myNonTerminalSymbols.insert(lhs);

      production = new Production(NonTerminalSymbol(lhs), productionNumber);
      myProductions.push_back(std::shared_ptr<Production>(production));

      consumeWhitespace(inputLine);
      std::string arrow(readSymbol(inputLine));
      while (inputLine.size() > 0)
      {
        consumeWhitespace(inputLine);

        std::string symbolString(readSymbol(inputLine));

        if (! symbolString.empty())
        {
          auto symbol = makeSymbol(symbolString);

          // TODO: create factory, if symbol doesn't start with < then
          //       need non-terminal, in which case the name should be
          //       looked up against known tokens and the Symbol can be given
          //       a terminal code (need Token class then)
          production->addRHSSymbol(symbol);
        }
      }
    }
  }
  mySymbols.insert(myNonTerminalSymbols.begin(), myNonTerminalSymbols.end());
  mySymbols.insert(myTerminalSymbols.begin(), myTerminalSymbols.end());
}

//*******************************************************
// GrammarAnalyzer::readSymbol
//*******************************************************
std::string GrammarAnalyzer::readSymbol(std::string &theLine)
{
  std::string symbol;
  auto currentChar = theLine.begin();
  if (*currentChar == '<')
  {
    // Read non-terminal
    while (true)
    {
      if (*currentChar != '>')
      {
        symbol += *currentChar;
        ++currentChar;
      }
      else
      {
        symbol += *currentChar;
        ++currentChar;
        break;
      }
    }
  }
  else
  {
    // Read non-non-terminal (also used to read '->', $, etc.)
    while (currentChar != theLine.end())
    {
      // Handle whitespace
      if (' ' == *currentChar || '\t' == *currentChar)
      {
        break;
      }
      else
      {
        symbol += *currentChar;
        ++currentChar;
      }
    }
  }

  if (currentChar != theLine.begin())
  {
    theLine.erase(theLine.begin(), currentChar);
  }

  return symbol;
}

//*******************************************************
// GrammarAnalyzer::readSymbol
//*******************************************************
std::ostream& operator<<(std::ostream &theOS,
                         const GrammarAnalyzer &theAnalyzer)
{
  theOS << "Productions" << std::endl
        << "-----------" << std::endl;
  for (auto production : theAnalyzer.myProductions)
  {
    theOS << *production << std::endl;
  }
  theOS << std::endl;

  theOS << "All Symbols" << std::endl
        << "-----------" << std::endl;
  for (auto &symbol : theAnalyzer.mySymbols)
  {
    theOS << symbol << std::endl;
  }
  theOS << std::endl;

  theOS << "Terminal Symbols" << std::endl
        << "----------------" << std::endl;
  for (auto &symbol : theAnalyzer.myTerminalSymbols)
  {
    theOS << symbol << std::endl;
  }
  theOS << std::endl;

  theOS << "Non-Terminal Symbols" << std::endl
        << "--------------------" << std::endl;
  for (auto &symbol : theAnalyzer.myNonTerminalSymbols)
  {
    theOS << symbol << std::endl;
  }
  theOS << std::endl;

  theOS << "Left Hand Sides" << std::endl
        << "---------------" << std::endl;
  for (auto production : theAnalyzer.myProductions)
  {
    theOS << production->getLHS() << std::endl;
  }
  theOS << std::endl;

  theOS << "Right Hand Sides" << std::endl
        << "----------------" << std::endl;
  for (auto production : theAnalyzer.myProductions)
  {
    theOS << production->getRHS() << std::endl;
  }
  theOS << std::endl;

  return theOS;
}
