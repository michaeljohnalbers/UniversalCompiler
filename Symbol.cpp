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
// Symbol::addToFirstSet
//*******************************************************
void Symbol::addToFirstSet(std::shared_ptr<Symbol> theSymbol) noexcept
{
  myFirstSet.insert(theSymbol);
}

//*******************************************************
// Symbol::getDerivesLambda
//*******************************************************
bool Symbol::getDerivesLambda() const noexcept
{
  return myDerivesLambda;
}

//*******************************************************
// Symbol::getFirstSet
//*******************************************************
const Symbol::SymbolSet& Symbol::getFirstSet() const noexcept
{
  return myFirstSet;
}

//*******************************************************
// Symbol::getName
//*******************************************************
std::string Symbol::getName() const noexcept
{
  return myName;
}

//*******************************************************
// Symbol::operator<
//*******************************************************
bool Symbol::operator<(const Symbol &theRHS) const noexcept
{
  return getName() < theRHS.getName();
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
// Symbol::operator==
//*******************************************************
bool Symbol::operator==(const Symbol &theRHS) const noexcept
{
  return getName() == theRHS.getName();
}

//*******************************************************
// Symbol::print
//*******************************************************
void Symbol::print(std::ostream &theOS) const noexcept
{
  theOS << getName();
}

//*******************************************************
// Symbol::setDerivesLambda
//*******************************************************
void Symbol::setDerivesLambda(bool theDerivesLambda) noexcept
{
  myDerivesLambda = theDerivesLambda;
}

//*******************************************************
// SymbolCompare::operator()
//*******************************************************
bool SymbolCompare::operator()(const std::shared_ptr<Symbol> theLHS,
                               const std::shared_ptr<Symbol> theRHS)
  const noexcept
{
  if (theLHS.get() && theRHS.get())
  {
    return *theLHS < *theRHS;
  }
  else
  {
    return false;
  }
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS, const Symbol::SymbolList &theList)
{
  for (auto ii = 0u; ii < theList.size(); ++ii)
  {
    theOS << *theList[ii];
    if (ii < theList.size() - 1)
    {
      theOS << " ";
    }
  }

  return theOS;
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS, const Symbol::SymbolSet &theSet)
{
  theOS << "{";
  auto iter = theSet.begin();
  for (auto ii = 0u; iter != theSet.end(); ++iter, ++ii)
  {
    if (ii > 0)
    {
      theOS << ", ";
    }
    theOS << **iter;
  }
  theOS << "}";

  return theOS;
}
