/**
 * @file Lambda.cpp
 * @brief Implementation of Lambda class
 *
 * @author Michael Albers
 */

#include "Lambda.h"

const std::shared_ptr<Symbol> Lambda::ourLambda(new Lambda());

//*******************************************************
// Lambda::Lambda
//*******************************************************
Lambda::Lambda() :
  Symbol("Lambda"),
  myFirstSet{ourLambda}
{
}

//*******************************************************
// Lambda::addToFirstSet
//*******************************************************
void Lambda::addToFirstSet(std::shared_ptr<Symbol> theSymbol) noexcept
{
  // Nothing else goes in the first set of Lambda.
}

//*******************************************************
// Lambda::getDerivesLambda
//*******************************************************
bool Lambda::getDerivesLambda() const noexcept
{
  return true;
}

//*******************************************************
// Lambda::getInstance
//*******************************************************
std::shared_ptr<Symbol> Lambda::getInstance() noexcept
{
  return ourLambda;
}

//*******************************************************
// Lambda::getFirstSet
//*******************************************************
const Symbol::SymbolSet& Lambda::getFirstSet() const noexcept
{
  return myFirstSet;
}
