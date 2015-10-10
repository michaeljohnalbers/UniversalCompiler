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

//*******************************************************
// ScannerTable::addColumn
//*******************************************************
ScannerTable::ScannerTable()
{
  addToken(EOF_SYMBOL, "EofSym");
  addToken(NO_TOKEN, "NoToken");
}

//*******************************************************
// ScannerTable::addColumn
//*******************************************************
void ScannerTable::addColumn(const std::string &theCharacterClass) noexcept
{
  myColumnCharacterClasses.push_back(theCharacterClass);
}

//*******************************************************
// ScannerTable::addToken
//*******************************************************
void ScannerTable::addToken(TokenId theTokenId, const std::string &theToken)
  noexcept
{
  myTokens[theTokenId] = theToken;
}

//*******************************************************
// ScannerTable::addReservedWord
//*******************************************************
void ScannerTable::addReservedWord(TokenId theTokenId,
                                   std::string theReservedWord) noexcept
{
  std::transform(theReservedWord.begin(), theReservedWord.end(),
                 theReservedWord.begin(), ::tolower);
  myReservedWords[theTokenId] = theReservedWord;
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
ScannerTable::TokenId ScannerTable::checkExceptions(
  TokenId theTokenId,
  std::string theToken) const noexcept
{
  std::transform(theToken.begin(), theToken.end(), theToken.begin(), ::tolower);

  for (auto reservedWordPair : myReservedWords)
  {
    if (theToken == reservedWordPair.second)
    {
      theTokenId = reservedWordPair.first;
      break;
    }
  }

  return theTokenId;
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
// ScannerTable::getTokenName
//*******************************************************
std::string ScannerTable::getTokenName(TokenId theTokenId) const noexcept
{
  auto tokenIter = myTokens.find(theTokenId);
  if (tokenIter != myTokens.end())
  {
    return tokenIter->second;
  }
  return "UnknownToken";
}

//*******************************************************
// ScannerTable::lookupCode
//*******************************************************
ScannerTable::TokenId ScannerTable::lookupCode(State theCurrentState,
                                               char theCharacter) const
{
  validateState(theCurrentState);
  auto column = getColumn(theCharacter);

  return myTable[theCurrentState][column].myTokenId;
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
  myTokenId(0)
{
}

//*******************************************************
// ScannerTable::Entry::Entry
//*******************************************************
ScannerTable::Entry::Entry(State theNextState,
                           const std::string &theActionAcronym,
                           uint32_t theTokenId) :
  myNextState(theNextState),
  myTokenId(theTokenId)
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
        << ", Token Id: " << theEntry.myTokenId;
  return theOS;
}
