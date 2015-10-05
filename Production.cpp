/**
 * @file Production.cpp
 * @brief Implementation of Production class
 *
 * @author Michael Albers
 */

#include <iomanip>

#include "Production.h"
#include "Symbol.h"

//*******************************************************
// Production::Production
//*******************************************************
Production::Production(std::shared_ptr<Symbol> theLHS,
                       uint32_t theNumber) :
  myLHS(theLHS),
  myNumber(theNumber)
{
}

//*******************************************************
// Production::addRHSSymbol
//*******************************************************
void Production::addRHSSymbol(std::shared_ptr<Symbol> theRHSSymbol)
  noexcept
{
  myRHS.push_back(theRHSSymbol);
}

//*******************************************************
// Production::addToPredictSet
//*******************************************************
void Production::addToPredictSet(std::shared_ptr<Symbol> theSymbol) noexcept
{
  myPredictSet.insert(theSymbol);
}

//*******************************************************
// Production::getLHS
//*******************************************************
std::shared_ptr<Symbol> Production::getLHS() const noexcept
{
  return myLHS;
}

//*******************************************************
// Production::getNumber
//*******************************************************
uint32_t Production::getNumber() const noexcept
{
  return myNumber;
}

//*******************************************************
// Production::getPredictSet
//*******************************************************
Symbol::SymbolSet Production::getPredictSet() const noexcept
{
  return myPredictSet;
}

//*******************************************************
// Production::getRHS
//*******************************************************
Symbol::SymbolList Production::getRHS() const noexcept
{
  return myRHS;
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS,
                         const Production &theProduction)
{
  theOS << std::setw(3) << theProduction.myNumber << " "
        << *(theProduction.myLHS) << " -> " << theProduction.myRHS;
  return theOS;
}
