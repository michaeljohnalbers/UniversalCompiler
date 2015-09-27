/**
 * @file Symbol.cpp
 * @brief Implementation of Symbol class
 *
 * @author Michael Albers
 */

#include "Symbol.h"

//*******************************************************
// Symbol::Symbol
//*******************************************************
Symbol::Symbol(const std::string &theName) :
  myName(theName)
{
}

//*******************************************************
// Symbol::~Symbol
//*******************************************************
Symbol::~Symbol()
{
}

//*******************************************************
// Symbol::getName
//*******************************************************
std::string Symbol::getName() const noexcept
{
  return myName;
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS, const Symbol &theSymbol)
{
  theSymbol.print(theOS);
  return theOS;
}

//*******************************************************
// Symbol::print
//*******************************************************
void Symbol::print(std::ostream &theOS) const noexcept
{
  theOS << getName();
}
