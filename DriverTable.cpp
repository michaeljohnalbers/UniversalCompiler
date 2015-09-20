/**
 * @file DriverTable.cpp
 * @brief Implementation of DriverTable class
 *
 * @author Michael Albers
 */

#include <algorithm>
#include <cctype>
#include <climits>
#include <stdexcept>
#include <string>

#include "DriverTable.h"

// Static Variables
DriverTable::TokenMap DriverTable::ourTokenDescriptions{
  {TokenCode::NoToken, "NoToken"},  
  {TokenCode::BeginSym, "BeginSym"},
  {TokenCode::EndSym, "EndSym"},
  {TokenCode::ReadSym, "ReadSym"},
  {TokenCode::WriteSym, "WriteSym"},
  {TokenCode::Id, "Id"},
  {TokenCode::IntLiteral, "IntLiteral"},
  {TokenCode::LParen, "LParen"},
  {TokenCode::RParen, "RParen"},
  {TokenCode::SemiColon, "SemiColon"},
  {TokenCode::Comma, "Comma"},
  {TokenCode::AssignOp, "AssignOp"},
  {TokenCode::PlusOp, "PlusOp"},
  {TokenCode::MinusOp, "MinusOp"},
  {TokenCode::EqualOp, "EqualOp"},
  {TokenCode::EofSym, "EofSym"},
};

//*******************************************************
// DriverTable::getAction
//*******************************************************
DriverTable::DriverTable()
{
  populateTable();
}

//*******************************************************
// DriverTable::checkExceptions
//*******************************************************
DriverTable::TokenCode DriverTable::checkExceptions(
  TokenCode theTokenCode,
  const std::string &theToken) const
{
  std::string token {theToken};
  std::transform(token.begin(), token.end(), token.begin(), ::tolower);

  if (TokenCode::Id == theTokenCode)
  {
    if ("begin" == token)
    {
      theTokenCode = TokenCode::BeginSym;
    }
    else if ("end" == token)
    {
      theTokenCode = TokenCode::EndSym;
    }
    else if ("read" == token)
    {
      theTokenCode = TokenCode::ReadSym;
    }
    else if ("write" == token)
    {
      theTokenCode = TokenCode::WriteSym;
    }
  }

  return theTokenCode;
}

//*******************************************************
// DriverTable::getAction
//*******************************************************
DriverTable::Action DriverTable::getAction(State theCurrentState,
                                           char theCharacter) const
{
  validateState(theCurrentState);
  auto column = getColumn(theCharacter);

  return myTable[theCurrentState][column].myAction;
}

//*******************************************************
// DriverTable::getColumn
//*******************************************************
uint32_t DriverTable::getColumn(char theCharacter) const noexcept
{
  // This could be optimized some. Space, letters and numbers will occur far
  // more frequently than any other character. That isn't done here for the
  // sake of readability.
  uint32_t column = NUMBER_CHARACTER_CLASSES-1;
  if (std::isalpha(theCharacter)) {column = 0;}
  else if (std::isdigit(theCharacter)) {column = 1;}
  else if (' ' == theCharacter) {column = 2;}
  else if ('+' == theCharacter) {column = 3;}
  else if ('-' == theCharacter) {column = 4;}
  else if ('=' == theCharacter) {column = 5;}
  else if (':' == theCharacter) {column = 6;}
  else if (',' == theCharacter) {column = 7;}
  else if (';' == theCharacter) {column = 8;}
  else if ('(' == theCharacter) {column = 9;}
  else if (')' == theCharacter) {column = 10;}
  else if ('_' == theCharacter) {column = 11;}
  else if ('\t' == theCharacter) {column = 12;}
  else if ('\n' == theCharacter) {column = 13;}
  // else {Use value in column initialization}

  return column;
}

//*******************************************************
// DriverTable::getDescription
//*******************************************************
std::string DriverTable::getDescription(TokenCode theTokenCode)
  noexcept
{
  return ourTokenDescriptions[theTokenCode];
}

//*******************************************************
// DriverTable::getState
//*******************************************************
DriverTable::State DriverTable::getState(State theCurrentState,
                                         char theCharacter) const
{
  validateState(theCurrentState);
  auto column = getColumn(theCharacter);

  return myTable[theCurrentState][column].myNextState;
}

//*******************************************************
// DriverTable::lookupCode
//*******************************************************
DriverTable::TokenCode DriverTable::lookupCode(State theCurrentState,
                                               char theCharacter) const
{
  validateState(theCurrentState);
  auto column = getColumn(theCharacter);

  return myTable[theCurrentState][column].myTokenCode;
}

//*******************************************************
// DriverTable::validateState
//*******************************************************
void DriverTable::validateState(State theState) const
{
  if (theState > MAX_STATE)
  {
    std::string error{"Illegal state, "};
    error += std::to_string(theState) + ", must be 0 <= x <= " +
      std::to_string(MAX_STATE);
    throw std::invalid_argument(error);
  }
}

//*******************************************************
// DriverTable::populateTable
//*******************************************************
void DriverTable::populateTable() noexcept
{
  populateState0();
  populateState1();
  populateState2();
  populateState3();
  populateState4();
  populateState5();
  populateState6();
}

//*******************************************************
// DriverTable::populateState0
//*******************************************************
void DriverTable::populateState0() noexcept
{
  // Initial state
  int32_t charSet = -1;
  myTable[0][++charSet] = Entry(1, Action::MoveAppend);   // Letter
  myTable[0][++charSet] = Entry(2, Action::MoveAppend);   // Digit
  myTable[0][++charSet] = Entry(3, Action::MoveNoAppend); // Blank
  myTable[0][++charSet] = Entry(Action::HaltAppend,       // +
                                TokenCode::PlusOp);
  myTable[0][++charSet] = Entry(4, Action::MoveAppend);   // -
  myTable[0][++charSet] = Entry();                        // =
  myTable[0][++charSet] = Entry(6, Action::MoveAppend);   // :
  myTable[0][++charSet] = Entry(Action::HaltAppend,       // ,
                                TokenCode::Comma);
  myTable[0][++charSet] = Entry(Action::HaltAppend,       // ;
                                TokenCode::SemiColon);
  myTable[0][++charSet] = Entry(Action::HaltAppend,       // (
                                TokenCode::LParen);
  myTable[0][++charSet] = Entry(Action::HaltAppend,       // )
                                TokenCode::RParen);
  myTable[0][++charSet] = Entry();                        // _
  myTable[0][++charSet] = Entry(3, Action::MoveNoAppend); // \t
  myTable[0][++charSet] = Entry(3, Action::MoveNoAppend); // \n
  myTable[0][++charSet] = Entry();                        // other
}

//*******************************************************
// DriverTable::populateState1
//*******************************************************
void DriverTable::populateState1() noexcept
{
  // Identifier (will also find reserved words, see checkExceptions)
  int32_t charSet = -1;
  myTable[1][++charSet] = Entry(1, Action::MoveAppend); // Letter
  myTable[1][++charSet] = Entry(1, Action::MoveAppend); // Digit
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // Blank
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // +
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // -
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // =
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // :
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // ,
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // ;
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // (
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // )
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(1, Action::MoveAppend); // _
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // \t
                                TokenCode::Id);
  myTable[1][++charSet] = Entry(Action::HaltReuse,      // \n
                                TokenCode::Id);
  myTable[1][++charSet] = Entry();                      // other
}

//*******************************************************
// DriverTable::populateState2
//*******************************************************
void DriverTable::populateState2() noexcept
{
  // Integer Literal
  int32_t charSet = -1;
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // Letter
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(2, Action::MoveAppend); // Digit
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // Blank
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // +
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // -
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // =
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // :
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // ,
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // ;
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // (
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // )
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // _
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // \t
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry(Action::HaltReuse,      // \n
                                TokenCode::IntLiteral);
  myTable[2][++charSet] = Entry();                      // other
}

//*******************************************************
// DriverTable::populateState3
//*******************************************************
void DriverTable::populateState3() noexcept
{
  // Empty space
  int32_t charSet = -1;
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // Letter
                                TokenCode::NoToken); 
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // Digit
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(3, Action::MoveNoAppend); // Blank
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // +
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // -
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // =
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // :
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // ,
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // ;
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // (
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // )
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(Action::HaltReuse,        // _
                                TokenCode::NoToken);
  myTable[3][++charSet] = Entry(3, Action::MoveNoAppend); // \t
  myTable[3][++charSet] = Entry(3, Action::MoveNoAppend); // \n
  myTable[3][++charSet] = Entry();                        // other
}

//*******************************************************
// DriverTable::populateState4
//*******************************************************
void DriverTable::populateState4() noexcept
{
  // Minus or Comment
  int32_t charSet = -1;
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // Letter
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // Digit
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // Blank
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // +
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry(5, Action::MoveNoAppend); // -
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // =
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // :
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // ,
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // ;
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // (
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // )
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry();                        // _
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // \t
                                TokenCode::MinusOp); 
  myTable[4][++charSet] = Entry(Action::HaltReuse,        // \n
                                TokenCode::MinusOp);
  myTable[4][++charSet] = Entry();                        // other
}

//*******************************************************
// DriverTable::populateState5
//*******************************************************
void DriverTable::populateState5() noexcept
{
  // Comment
  int32_t charSet = -1;
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // Letter
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // Digit
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // Blank
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // +
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // -
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // =
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // :
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // ,
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // ;
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // (
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // )
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // _
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // \t
  myTable[5][++charSet] = Entry(Action::HaltNoAppend,     // \n
                                TokenCode::NoToken);
  myTable[5][++charSet] = Entry(5, Action::MoveNoAppend); // other
}

//*******************************************************
// DriverTable::populateState6
//*******************************************************
void DriverTable::populateState6() noexcept
{
  // Assignment Op
  int32_t charSet = -1;
  myTable[6][++charSet] = Entry();                    // Letter
  myTable[6][++charSet] = Entry();                    // Digit
  myTable[6][++charSet] = Entry();                    // Blank
  myTable[6][++charSet] = Entry();                    // +
  myTable[6][++charSet] = Entry();                    // -
  myTable[6][++charSet] = Entry(Action::HaltAppend,   // =
                                TokenCode::AssignOp);
  myTable[6][++charSet] = Entry();                    // :
  myTable[6][++charSet] = Entry();                    // ,
  myTable[6][++charSet] = Entry();                    // ;
  myTable[6][++charSet] = Entry();                    // (
  myTable[6][++charSet] = Entry();                    // )
  myTable[6][++charSet] = Entry();                    // _
  myTable[6][++charSet] = Entry();                    // \t
  myTable[6][++charSet] = Entry();                    // \n
  myTable[6][++charSet] = Entry();                    // other
}

//*******************************************************
// DriverTable::Entry::Entry
//*******************************************************
DriverTable::Entry::Entry() :
  Entry(UINT_MAX, Action::Error)
{
}

//*******************************************************
// DriverTable::Entry::Entry
//*******************************************************
DriverTable::Entry::Entry(State theNextState, Action theAction) :
  Entry(theNextState, theAction, TokenCode::NoToken)
{
}

//*******************************************************
// DriverTable::Entry::Entry
//*******************************************************
DriverTable::Entry::Entry(Action theAction, TokenCode theTokenCode) :
  Entry(UINT_MAX, theAction, theTokenCode)
{
}

//*******************************************************
// DriverTable::Entry::Entry
//*******************************************************
DriverTable::Entry::Entry(State theNextState, Action theAction,
                          TokenCode theTokenCode) :
  myNextState(theNextState),
  myAction(theAction),
  myTokenCode(theTokenCode)
{
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS,
                         const DriverTable::Entry &theEntry)
{
  theOS << "Next State: " << theEntry.myNextState
        << ", Action: " << static_cast<int>(theEntry.myAction)
        << ", Token Code: " << static_cast<int>(theEntry.myTokenCode);
  return theOS;
}
