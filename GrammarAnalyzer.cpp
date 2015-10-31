/**
 * @file GrammarAnalyzer.cpp
 * @brief Implementation of GrammarAnalyzer class
 *
 * @author Michael Albers
 */

#include <cerrno>
#include <iostream>

#include "GrammarAnalyzer.h"
#include "Lambda.h"
#include "NonTerminalSymbol.h"
#include "Production.h"
#include "TerminalSymbol.h"

//*******************************************************
// GrammarAnalyzer::GrammarAnalyzer
//*******************************************************
GrammarAnalyzer::GrammarAnalyzer(Grammar &theGrammar) :
  myGrammar(theGrammar),
  myNonTerminalSymbols(myGrammar.getNonTerminalSymbols()),
  myProductions(myGrammar.getProductions()),
  myTerminalSymbols(myGrammar.getTerminalSymbols())
{
  mySymbols.insert(myNonTerminalSymbols.begin(), myNonTerminalSymbols.end());
  mySymbols.insert(myTerminalSymbols.begin(), myTerminalSymbols.end());

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
    bool symbolSetContainsLambda = true;
    for (; ii < numberSymbols && symbolSetContainsLambda; ++ii)
    {
      if (! isGrammarSymbol(theSymbols[ii]))
      {
        continue;
      }

      symbolFirstSet = theSymbols[ii]->getFirstSet();
      firstSet.insert(symbolFirstSet.begin(), symbolFirstSet.end());
      firstSet.erase(lambda);
      symbolSetContainsLambda = containsLambda(symbolFirstSet);
    }

    if (ii == numberSymbols && symbolSetContainsLambda)
    {
      firstSet.insert(lambda);
    }
  }
  return firstSet;
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
    uint32_t index = 0;
    for (; index < rhs.size() && false == isGrammarSymbol(rhs[index]); ++index);
    if (typeid(*rhs[index]) == typeid(TerminalSymbol))
    {
      production->getLHS()->addToFirstSet(rhs[index]);
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
  auto s = dynamic_cast<NonTerminalSymbol*>(myGrammar.getStartSymbol().get());
  s->addToFollowSet(Lambda::getInstance());

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
            if (isGrammarSymbol(rhs[jj]))
            {
              remainingRhs.push_back(rhs[jj]);
            }
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
// GrammarAnalyzer::isGrammarSymbol
//*******************************************************
bool GrammarAnalyzer::isGrammarSymbol(std::shared_ptr<Symbol> theSymbol)
  noexcept
{
  auto *pointer = theSymbol.get();
  return (typeid(*pointer) == typeid(TerminalSymbol) ||
          typeid(*pointer) == typeid(NonTerminalSymbol) ||
          typeid(*pointer) == typeid(Lambda));
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS,
                         const GrammarAnalyzer &theAnalyzer)
{
  theOS << "Grammar Analysis" << std::endl
        << "================" << std::endl << std::endl;

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
