/**
 * @file TerminalSymbol.cpp
 * @brief Implementation of TerminalSymbol class
 *
 * @author Michael Albers
 */

#include <iomanip>

#include "TerminalSymbol.h"

bool TerminalSymbol::ourLongPrint = false;

//*******************************************************
// TerminalSymbol::TerminalSymbol
//*******************************************************
TerminalSymbol::TerminalSymbol(
  const std::string &theName,
  Id theId,
  const std::string &theResevedWord) :
  Symbol(theName),
  myId(theId),
  myReservedWord(theResevedWord)
{
}

//*******************************************************
// TerminalSymbol::getDerivesLambda
//*******************************************************
bool TerminalSymbol::getDerivesLambda() const noexcept
{
  return false;
}

//*******************************************************
// TerminalSymbol::getId
//*******************************************************
TerminalSymbol::Id TerminalSymbol::getId() const noexcept
{
  return myId;
}

//*******************************************************
// TerminalSymbol::getReservedWord
//*******************************************************
std::string TerminalSymbol::getReservedWord() const noexcept
{
  return myReservedWord;
}

//*******************************************************
// TerminalSymbol::print
//*******************************************************
void TerminalSymbol::print(std::ostream &theOS) const noexcept
{
  if (ourLongPrint)
  {
    theOS << std::setw(3) << getId() << " " << getName();
    std::string reservedWord{getReservedWord()};
    if (! reservedWord.empty())
    {
      theOS << " (" << reservedWord << ")";
    }
  }
  else
  {
    theOS << getName();
  }
}
