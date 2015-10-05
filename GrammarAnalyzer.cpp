/**
 * @file GrammarAnalyzer.cpp
 * @brief Implementation of GrammarAnalyzer class
 *
 * @author Michael Albers
 */

#include <cerrno>
#include <iostream>
#include <system_error>

#include "GrammarAnalyzer.h"
#include "Lambda.h"
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
  calculateDerivesLambda();
  fillFirstSets();
  fillFollowSets();
  generatePredictSets();
}

//*******************************************************
// GrammarAnalyzer::calculateDerivesLambda
//*******************************************************
void GrammarAnalyzer::calculateDerivesLambda() noexcept
{
  bool anyChanges = false;
  do
  {
    anyChanges = false;
    for (auto production : myProductions)
    {
      bool rhsDerivesLambda = true;
      auto rhs = production->getRHS();
      for (auto rhsSymbol : rhs)
      {
        rhsDerivesLambda &= rhsSymbol->getDerivesLambda();
      }

      auto lhs = production->getLHS();
      if (rhsDerivesLambda && ! lhs->getDerivesLambda())
      {
        anyChanges = true;
        lhs->setDerivesLambda(true);
      }
    }
  }
  while (anyChanges);
}

//*******************************************************
// GrammarAnalyzer::computeFirst
//*******************************************************
Symbol::SymbolSet GrammarAnalyzer::computeFirst(
  const Symbol::SymbolList &theSymbols) const noexcept
{
  Symbol::SymbolSet firstSet;

  auto lambda = Lambda::getInstance();

  auto numberSymbols = theSymbols.size();
  if (theSymbols.size() == 0)
  {
    firstSet.insert(lambda);
  }
  else
  {
    Symbol::SymbolSet symbolFirstSet;
    Symbol::SymbolList::size_type ii = 0;
    do
    {
      symbolFirstSet = theSymbols[ii]->getFirstSet();
      firstSet.insert(symbolFirstSet.begin(), symbolFirstSet.end());
      firstSet.erase(lambda);
      ++ii;
    }
    while (ii < numberSymbols && containsLambda(symbolFirstSet));

    if (ii == numberSymbols && containsLambda(symbolFirstSet))
    {
      firstSet.insert(lambda);
    }
  }
  return firstSet;
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
// GrammarAnalyzer::containsLambda
//*******************************************************
bool GrammarAnalyzer::containsLambda(const Symbol::SymbolSet &theSymbols)
  noexcept
{
  return theSymbols.find(Lambda::getInstance()) != theSymbols.end();
}

//*******************************************************
// GrammarAnalyzer::fillFirstSets
//*******************************************************
void GrammarAnalyzer::fillFirstSets() noexcept
{
  // This loop assumes the first sets haven't been changed from the empty
  // set yet. (Code provided in class has an else case to set the first
  // set to the empty list.)
  for (auto nonTerminal : myNonTerminalSymbols)
  {
    if (nonTerminal->getDerivesLambda())
    {
      nonTerminal->addToFirstSet(Lambda::getInstance());
    }
  }

  for (auto terminal : myTerminalSymbols)
  {
    terminal->addToFirstSet(terminal);
  }

  for (auto production : myProductions)
  {
    auto rhs = production->getRHS();
    if (typeid(*rhs[0]) == typeid(TerminalSymbol))
    {
      production->getLHS()->addToFirstSet(rhs[0]);
    }
  }

  bool anyChanges = true;
  while (anyChanges)
  {
    anyChanges = false;
    for (auto production : myProductions)
    {
      auto previousSize = production->getLHS()->getFirstSet().size();
      Symbol::SymbolSet rhsFirstSet{computeFirst(production->getRHS())};

      for (auto symbol : rhsFirstSet)
      {
        production->getLHS()->addToFirstSet(symbol);
      }

      auto afterSize = production->getLHS()->getFirstSet().size();

      if (! anyChanges)
      {
        anyChanges = (previousSize != afterSize);
      }
    }
  }
}

//*******************************************************
// GrammarAnalyzer::fillFollowSets
//*******************************************************
void GrammarAnalyzer::fillFollowSets() noexcept
{
  // Quite the hack, but really no other way to do it.
  for (auto symbol : myNonTerminalSymbols)
  {
    if (symbol->getName() == "<system goal>")
    {
      auto s = dynamic_cast<NonTerminalSymbol*>(symbol.get());
      s->addToFollowSet(Lambda::getInstance());
      break;
    }
  }

  bool anyChanges = true;
  while (anyChanges)
  {
    anyChanges = false;
    for (auto production : myProductions)
    {
      auto rhs = production->getRHS();
      for (uint32_t rhsIndex = 0; rhsIndex < rhs.size(); ++rhsIndex)
      {
        std::shared_ptr<Symbol> rhsSymbol = rhs[rhsIndex];
        if (typeid(*rhsSymbol) == typeid(NonTerminalSymbol))
        {
          NonTerminalSymbol *nonTerminal =
            dynamic_cast<NonTerminalSymbol*>(rhsSymbol.get());

          auto previousSize = nonTerminal->getFollowSet().size();

          Symbol::SymbolList remainingRhs;
          for (auto jj = rhsIndex + 1; jj < rhs.size(); ++jj)
          {
            remainingRhs.push_back(rhs[jj]);
          }

          Symbol::SymbolSet firstSetOfRemaining{computeFirst(remainingRhs)};

          auto lambdaIter = firstSetOfRemaining.find(Lambda::getInstance());
          bool hasLambda = lambdaIter != firstSetOfRemaining.end();

          if (hasLambda)
          {
            firstSetOfRemaining.erase(lambdaIter);
          }

          for (auto s : firstSetOfRemaining)
          {
            nonTerminal->addToFollowSet(s);
          }

          if (hasLambda)
          {
            NonTerminalSymbol *lhs =
              dynamic_cast<NonTerminalSymbol*>(production->getLHS().get());
            for (auto symbol : lhs->getFollowSet())
            {
              nonTerminal->addToFollowSet(symbol);
            }
          }

          auto afterSize = nonTerminal->getFollowSet().size();

          if (! anyChanges)
          {
            anyChanges = (previousSize != afterSize);
          }
        }
      }
    }  
  }
}

//*******************************************************
// GrammarAnalyzer::generatePredictSets
//*******************************************************
void GrammarAnalyzer::generatePredictSets() noexcept
{
  for (auto production : myProductions)
  {
    Symbol::SymbolSet predictSet{computeFirst(production->getRHS())};
    if (containsLambda(predictSet))
    {
      auto lhsPtr = production->getLHS();
      NonTerminalSymbol *lhs = dynamic_cast<NonTerminalSymbol*>(lhsPtr.get());
      Symbol::SymbolSet followSet{lhs->getFollowSet()};
      predictSet.insert(followSet.begin(), followSet.end());
    }
    predictSet.erase(Lambda::getInstance());
    for (auto symbol : predictSet)
    {
      production->addToPredictSet(symbol);
    }
  }
}

//*******************************************************
// GrammarAnalyzer::makeNonTerminal
//*******************************************************
std::shared_ptr<Symbol> GrammarAnalyzer::makeNonTerminal(
  const std::string &theSymbol) noexcept
{
  std::shared_ptr<Symbol> nonTerminal(new NonTerminalSymbol(theSymbol));

  Symbol::SymbolSet::iterator nonTerminalIter =
    myNonTerminalSymbols.find(nonTerminal);
  if (nonTerminalIter == myNonTerminalSymbols.end())
  {
    myNonTerminalSymbols.insert(nonTerminal);
    return nonTerminal;
  }

  return *nonTerminalIter;
}

//*******************************************************
// GrammarAnalyzer::makeSymbol
//*******************************************************
std::shared_ptr<Symbol> GrammarAnalyzer::makeSymbol(
  const std::string &theSymbol)
{
  if ('<' == theSymbol[0])
  {
    return makeNonTerminal(theSymbol);
  }

  return makeTerminal(theSymbol);
}

//*******************************************************
// GrammarAnalyzer::makeTerminal
//*******************************************************
std::shared_ptr<Symbol> GrammarAnalyzer::makeTerminal(
  const std::string &theSymbol) noexcept
{
  std::shared_ptr<Symbol> terminal(new TerminalSymbol(theSymbol));

  Symbol::SymbolSet::iterator terminalIter =
    myTerminalSymbols.find(terminal);
  if (terminalIter == myTerminalSymbols.end())
  {
    myTerminalSymbols.insert(terminal);
    return terminal;
  }

  return *terminalIter;
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
      std::string lhsString(readSymbol(inputLine));
      std::shared_ptr<Symbol> lhs(makeNonTerminal(lhsString));

      production = new Production(lhs, productionNumber);
      myProductions.push_back(std::shared_ptr<Production>(production));

      consumeWhitespace(inputLine);
      std::string arrow(readSymbol(inputLine));

      bool hasRHSSymbols = false;
      while (inputLine.size() > 0)
      {
        consumeWhitespace(inputLine);

        std::string symbolString(readSymbol(inputLine));

        if (! symbolString.empty())
        {
          auto symbol = makeSymbol(symbolString);
          production->addRHSSymbol(symbol);
          hasRHSSymbols = true;
        }
      }

      if (! hasRHSSymbols)
      {
        production->addRHSSymbol(Lambda::getInstance());
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
  for (auto symbol : theAnalyzer.mySymbols)
  {
    theOS << *symbol << std::endl;
  }
  theOS << std::endl;

  theOS << "Terminal Symbols" << std::endl
        << "----------------" << std::endl;
  for (auto symbol : theAnalyzer.myTerminalSymbols)
  {
    theOS << *symbol << std::endl;
  }
  theOS << std::endl;

  theOS << "Non-Terminal Symbols" << std::endl
        << "--------------------" << std::endl;
  for (auto symbol : theAnalyzer.myNonTerminalSymbols)
  {
    theOS << *symbol << std::endl;
  }
  theOS << std::endl;

  theOS << "Left Hand Sides" << std::endl
        << "---------------" << std::endl;
  for (auto production : theAnalyzer.myProductions)
  {
    theOS << *(production->getLHS()) << std::endl;
  }
  theOS << std::endl;

  theOS << "Right Hand Sides" << std::endl
        << "----------------" << std::endl;
  for (auto production : theAnalyzer.myProductions)
  {
    theOS << production->getRHS() << std::endl;
  }
  theOS << std::endl;

  theOS << "First Sets" << std::endl
        << "----------" << std::endl;
  for (auto symbol : theAnalyzer.mySymbols)
  {
    theOS << symbol->getName() << " = " << symbol->getFirstSet() << std::endl;
  }
  theOS << std::endl;

  theOS << "Follow Sets" << std::endl
        << "----------" << std::endl;
  for (auto symbol : theAnalyzer.myNonTerminalSymbols)
  {
    NonTerminalSymbol *nonTerminal = dynamic_cast<NonTerminalSymbol*>(
      symbol.get());
    theOS << nonTerminal->getName() << " = " << nonTerminal->getFollowSet()
          << std::endl;
  }
  theOS << std::endl;

  theOS << "Predict Sets" << std::endl
        << "------------" << std::endl;
  for (auto production : theAnalyzer.myProductions)
  {
    theOS << *production << " = " << production->getPredictSet() << std::endl;
  }
  theOS << std::endl;

  return theOS;
}
