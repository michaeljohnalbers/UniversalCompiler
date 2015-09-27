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
Production::Production(const NonTerminalSymbol &theLHS,
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
// Production::getLHS
//*******************************************************
NonTerminalSymbol Production::getLHS() const noexcept
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
// Production::getRHS
//*******************************************************
std::vector<std::shared_ptr<Symbol>> Production::getRHS() const noexcept
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
        << theProduction.myLHS << " -> " << theProduction.myRHS;
  return theOS;
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS,
                         const std::vector<std::shared_ptr<Symbol>> &theRHS)
{
  if (theRHS.empty())
  {
    theOS << "lambda";
  }
  else
  {
    for (auto ii = 0u; ii < theRHS.size(); ++ii)
    {
      theOS << *theRHS[ii];
      if (ii < theRHS.size() - 1)
      {
        theOS << " ";
      }
    }
  }
  return theOS;
}
