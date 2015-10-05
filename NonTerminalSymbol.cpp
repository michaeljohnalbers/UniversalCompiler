/**
 * @file NonTerminalSymbol.cpp
 * @brief Implementation of NonTerminalSymbol class
 *
 * @author Michael Albers
 */

#include "NonTerminalSymbol.h"

//*******************************************************
// NonTerminalSymbol::NonTerminalSymbol
//*******************************************************
NonTerminalSymbol::NonTerminalSymbol(const std::string &theName) :
  Symbol(theName)
{
}

//*******************************************************
// NonTerminalSymbol::addToFollowSet
//*******************************************************
void NonTerminalSymbol::addToFollowSet(std::shared_ptr<Symbol> theSymbol)
  noexcept
{
  myFollowSet.insert(theSymbol);
}

//*******************************************************
// NonTerminalSymbol::getFollowSet
//*******************************************************
const Symbol::SymbolSet& NonTerminalSymbol::getFollowSet() const noexcept
{
  return myFollowSet;
}
