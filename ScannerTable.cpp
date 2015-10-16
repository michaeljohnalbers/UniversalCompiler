/**
 * @file ScannerTable.cpp
 * @brief Implementation of ScannerTable class
 *
 * @author Michael Albers
 */

#include <algorithm>
#include <cctype>
#include <climits>
#include <stdexcept>
#include <string>

#include "ScannerTable.h"
#include "TerminalSymbol.h"


constexpr TerminalSymbol::Id ScannerTable::NO_TERMINAL;
constexpr TerminalSymbol::Id ScannerTable::EOF_SYMBOL;

//*******************************************************
// ScannerTable::ScannerTable
//*******************************************************
ScannerTable::ScannerTable()
{
  addTerminal(std::make_shared<TerminalSymbol>("$", EOF_SYMBOL, ""));
  addTerminal(std::make_shared<TerminalSymbol>("NoTerminal", NO_TERMINAL, ""));
}

//*******************************************************
// ScannerTable::addColumn
//*******************************************************
void ScannerTable::addColumn(const std::string &theCharacterClass) noexcept
{
  myColumnCharacterClasses.push_back(theCharacterClass);
}

//*******************************************************
// ScannerTable::addTerminal
//*******************************************************
void ScannerTable::addTerminal(std::shared_ptr<Symbol> theTerminal) noexcept
{
  myTerminals.insert(theTerminal);

  TerminalSymbol *terminal = dynamic_cast<TerminalSymbol*>(theTerminal.get());
  myTerminalIdMap[terminal->getId()] = theTerminal;

  std::string reservedWord {terminal->getReservedWord()};
  if (! reservedWord.empty())
  {
    std::transform(reservedWord.begin(), reservedWord.end(),
                   reservedWord.begin(), ::tolower);
    myReservedWords[reservedWord] = terminal->getId();
  }
}

//*******************************************************
// ScannerTable::addTableEntry
//*******************************************************
void ScannerTable::addTableEntry(State theState, uint32_t theColumn,
                                 const Entry &theEntry) noexcept
{
  if (myTable.size() <= theState)
  {
    myTable.resize(theState+1);
    myTable[theState].resize(myColumnCharacterClasses.size());
  }

  myTable[theState][theColumn] = theEntry;
}

//*******************************************************
// ScannerTable::checkExceptions
//*******************************************************
TerminalSymbol::Id ScannerTable::checkExceptions(
  TerminalSymbol::Id theTerminalId,
  std::string theTokenString) const noexcept
{
  std::transform(theTokenString.begin(), theTokenString.end(),
                 theTokenString.begin(), ::tolower);

  try
  {
    theTerminalId = myReservedWords.at(theTokenString);
  }
  catch(...){}

  return theTerminalId;
}

//*******************************************************
// ScannerTable::getAction
//*******************************************************
ScannerTable::Action ScannerTable::getAction(State theCurrentState,
                                             char theCharacter) const
{
  validateState(theCurrentState);
  auto column = getColumn(theCharacter);

  return myTable[theCurrentState][column].myAction;
}

//*******************************************************
// ScannerTable::getColumn
//*******************************************************
uint32_t ScannerTable::getColumn(char theCharacter) const noexcept
{
  uint32_t column = 0;
  for (const auto characterClass : myColumnCharacterClasses)
  {
    if ((characterClass == "letter" && std::isalpha(theCharacter)) ||
        (characterClass == "digit" && std::isdigit(theCharacter)) ||
        (characterClass == "whitespace" &&
         (' ' == theCharacter || '\t' == theCharacter)) ||
        (characterClass == "EOL" && '\n' == theCharacter) ||
        (theCharacter == characterClass[0]) ||
        (characterClass == "Other")) // Must be last in the condition
    {
      break;
    }
    ++column;
  }

  return column;
}

//*******************************************************
// ScannerTable::getEOF
//*******************************************************
std::shared_ptr<Symbol> ScannerTable::getEOF() const noexcept
{
  return myTerminalIdMap.at(EOF_SYMBOL);
}

//*******************************************************
// ScannerTable::getState
//*******************************************************
ScannerTable::State ScannerTable::getState(State theCurrentState,
                                           char theCharacter) const
{
  validateState(theCurrentState);
  auto column = getColumn(theCharacter);

  return myTable[theCurrentState][column].myNextState;
}

//*******************************************************
// ScannerTable::lookupTerminal
//*******************************************************
std::shared_ptr<Symbol> ScannerTable::lookupTerminal(
  State theCurrentState,
  char theCharacter,
  std::string theTokenString) const
{
  validateState(theCurrentState);
  auto column = getColumn(theCharacter);

  std::shared_ptr<Symbol> terminal;

  auto terminalId = myTable[theCurrentState][column].myTerminalId;
  try
  {
    terminal = myTerminalIdMap.at(terminalId);
    std::transform(theTokenString.begin(), theTokenString.end(),
                   theTokenString.begin(), ::tolower);
    auto reservedWord = myReservedWords.find(theTokenString);
    if (reservedWord != myReservedWords.end())
    {
      terminal = myTerminalIdMap.at(reservedWord->second);
    }
  }
  catch (std::out_of_range &exception)
  {
  }

  return terminal;
}

//*******************************************************
// ScannerTable::validateState
//*******************************************************
void ScannerTable::validateState(State theState) const
{
  if (theState >= myTable.size())
  {
    std::string error{"Illegal state, "};
    error += std::to_string(theState) + ", must be 0 <= x < " +
      std::to_string(myTable.size());
    throw std::invalid_argument(error);
  }
}

//*******************************************************
// ScannerTable::Entry::Entry
//*******************************************************
ScannerTable::Entry::Entry() :
  myNextState(0),
  myAction(Action::Error),
  myTerminalId(0)
{
}

//*******************************************************
// ScannerTable::Entry::Entry
//*******************************************************
ScannerTable::Entry::Entry(State theNextState,
                           const std::string &theActionAcronym,
                           TerminalSymbol::Id theTerminalId) :
  myNextState(theNextState),
  myTerminalId(theTerminalId)
{
  if ("E" == theActionAcronym)
  {
    myAction = Action::Error;
  }
  else if ("MA" == theActionAcronym)
  {
    myAction = Action::MoveAppend;
  }
  else if ("MNA" == theActionAcronym)
  {
    myAction = Action::MoveNoAppend;
  }
  else if ("HA" == theActionAcronym)
  {
    myAction = Action::HaltAppend;
  }
  else if ("HNA" == theActionAcronym)
  {
    myAction = Action::HaltNoAppend;
  }
  else if ("HR" == theActionAcronym)
  {
    myAction = Action::HaltReuse;
  }
  else
  {
    throw std::runtime_error("Invalid action acronym: " + theActionAcronym);
  }
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS,
                         const ScannerTable::Entry &theEntry)
{
  theOS << "Next State: " << theEntry.myNextState
        << ", Action: " << static_cast<int>(theEntry.myAction)
        << ", Terminal Id: " << theEntry.myTerminalId;
  return theOS;
}
