/**
 * @file ActionSymbol.cpp
 * @brief Implementation of ActionSymbol class
 *
 * @author Michael Albers
 */

#include "ActionSymbol.h"

//*******************************************************
// ActionSymbol::ActionSymbol
//*******************************************************
ActionSymbol::ActionSymbol(const std::string &theName) :
  Symbol(theName)
{
}

//*******************************************************
// ActionSymbol::getDerivesLambda
//*******************************************************
bool ActionSymbol::getDerivesLambda() const noexcept
{
  return false;
}
