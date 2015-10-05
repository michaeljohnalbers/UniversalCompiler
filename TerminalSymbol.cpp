/**
 * @file TerminalSymbol.cpp
 * @brief Implementation of TerminalSymbol class
 *
 * @author Michael Albers
 */

#include "TerminalSymbol.h"

//*******************************************************
// TerminalSymbol::TerminalSymbol
//*******************************************************
TerminalSymbol::TerminalSymbol(const std::string &theName) :
  Symbol(theName)
{
}

//*******************************************************
// TerminalSymbol::getDerivesLambda
//*******************************************************
bool TerminalSymbol::getDerivesLambda() const noexcept
{
  return false;
}
