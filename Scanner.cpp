/**
 * @file Scanner.cpp
 * @brief Implementation of Scanner class
 *
 * @author Michael Albers
 */

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "ErrorWarningTracker.h"
#include "Scanner.h"

//*******************************************************
// Scanner::Scanner
//*******************************************************
Scanner::Scanner(const std::string &theFile,
                 ErrorWarningTracker &theEWTracker) :
  myEWTracker(theEWTracker),
  myFile(theFile)
{
  myInputStream.open(myFile, std::ios::in);
  if (! myInputStream)
  {
    auto localErrno = errno;
    throw std::runtime_error("Failed to open '" + myFile + "': " +
                             std::strerror(localErrno));
  }
}

//*******************************************************
// Scanner::consumeChar
//*******************************************************
void Scanner::consumeChar()
{
  auto character = myInputStream.get();
  ++myColumn;
  if ('\n' == character)
  {
    myColumn = 0;
    ++myLine;
  }
}

//*******************************************************
// Scanner::currentChar
//*******************************************************
char Scanner::currentChar()
{
  return myInputStream.peek();
}

//*******************************************************
// Scanner::getColumn
//*******************************************************
uint32_t Scanner::getColumn() const noexcept
{
  return myColumn;
}

//*******************************************************
// Scanner::getLine
//*******************************************************
uint32_t Scanner::getLine() const noexcept
{
  return myLine;
}

//*******************************************************
// Scanner::scan
//*******************************************************
void Scanner::scan(std::string &theToken,
                   DriverTable::TokenCode &theTokenId)
{
  DriverTable::State currentState;

  // Resets the variables in this function to (re)start the scan from a fresh
  // token.
  auto reset = [&]()
  {
    theToken.clear();
    theTokenId = DriverTable::TokenCode::NoToken;
    currentState = DriverTable::START_STATE;
  };

  reset();

  while (currentChar() >= 0) // -1 is returned on EOF
  {
    auto action = myDriverTable.getAction(currentState, currentChar());

    switch (action)
    {
      case DriverTable::Action::Error:
      {
        theToken += currentChar();
        consumeChar();

        std::string error;
        error += "invalid token: '" + theToken + "'";
        myEWTracker.reportError(getLine(), getColumn(), error);

        reset(); // Error recovery
      }
      break;

      case DriverTable::Action::MoveAppend:
        currentState = myDriverTable.getState(currentState,
                                              currentChar());
        theToken += currentChar();
        consumeChar();
        break;

      case DriverTable::Action::MoveNoAppend:
        currentState = myDriverTable.getState(currentState,
                                              currentChar());
        consumeChar();
        break;

      case DriverTable::Action::HaltAppend:
        theToken += currentChar();
        theTokenId = myDriverTable.lookupCode(currentState,
                                              currentChar());
        theTokenId = myDriverTable.checkExceptions(theTokenId,
                                                   theToken);
        consumeChar();
        if (DriverTable::TokenCode::NoToken == theTokenId)
        {
          scan(theToken, theTokenId);
        }
        return;

      case DriverTable::Action::HaltNoAppend:
        theTokenId = myDriverTable.lookupCode(currentState,
                                              currentChar());
        theTokenId = myDriverTable.checkExceptions(theTokenId,
                                                   theToken);
        consumeChar();
        if (DriverTable::TokenCode::NoToken == theTokenId)
        {
          scan(theToken, theTokenId);
        }
        return;

      case DriverTable::Action::HaltReuse:
        theTokenId = myDriverTable.lookupCode(currentState,
                                              currentChar());
        theTokenId = myDriverTable.checkExceptions(theTokenId,
                                                   theToken);
        if (DriverTable::TokenCode::NoToken == theTokenId)
        {
          scan(theToken, theTokenId);
        }
        return;
    }
  }

  if (myInputStream.eof())
  {
    theToken = "";
    theTokenId = DriverTable::TokenCode::EofSym;
  }
}
