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
#include "ScannerTable.h"

//*******************************************************
// Scanner::Scanner
//*******************************************************
Scanner::Scanner(const std::string &theFile,
                 ScannerTable &theScannerTable,
                 ErrorWarningTracker &theEWTracker,
                 bool thePrintTokens) :
  myEWTracker(theEWTracker),
  myFile(theFile),
  myPrintTokens(thePrintTokens),
  myScannerTable(theScannerTable)
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
                   ScannerTable::TokenId &theTokenId)
{
  ScannerTable::State currentState;

  // Resets the variables in this function to (re)start the scan from a fresh
  // token.
  auto reset = [&]()
  {
    theToken.clear();
    theTokenId = ScannerTable::NO_TOKEN;
    currentState = ScannerTable::START_STATE;
  };

  reset();

  while (currentChar() >= 0) // -1 is returned on EOF
  {
    auto action = myScannerTable.getAction(currentState, currentChar());

    switch (action)
    {
      case ScannerTable::Action::Error:
      {
        theToken += currentChar();
        consumeChar();

        std::string error;
        error += "invalid token: '" + theToken + "'";
        myEWTracker.reportError(getLine(), getColumn(), error);

        reset(); // Error recovery
      }
      break;

      case ScannerTable::Action::MoveAppend:
        currentState = myScannerTable.getState(currentState,
                                               currentChar());
        theToken += currentChar();
        consumeChar();
        break;

      case ScannerTable::Action::MoveNoAppend:
        currentState = myScannerTable.getState(currentState,
                                               currentChar());
        consumeChar();
        break;

      case ScannerTable::Action::HaltAppend:
        theToken += currentChar();
        theTokenId = myScannerTable.lookupCode(currentState,
                                               currentChar());
        theTokenId = myScannerTable.checkExceptions(theTokenId,
                                                    theToken);
        consumeChar();
        if (ScannerTable::NO_TOKEN == theTokenId)
        {
          scan(theToken, theTokenId);
        }
        return;

      case ScannerTable::Action::HaltNoAppend:
        theTokenId = myScannerTable.lookupCode(currentState,
                                               currentChar());
        theTokenId = myScannerTable.checkExceptions(theTokenId,
                                                    theToken);
        consumeChar();
        if (ScannerTable::NO_TOKEN == theTokenId)
        {
          scan(theToken, theTokenId);
        }
        return;

      case ScannerTable::Action::HaltReuse:
        theTokenId = myScannerTable.lookupCode(currentState,
                                               currentChar());
        theTokenId = myScannerTable.checkExceptions(theTokenId,
                                                    theToken);
        if (ScannerTable::NO_TOKEN == theTokenId)
        {
          scan(theToken, theTokenId);
        }
        return;
    }
  }

  if (myInputStream.eof())
  {
    theToken = "";
    theTokenId = ScannerTable::EOF_SYMBOL;
  }

  // TODO: eventually put this into the parser?
  if (ScannerTable::NO_TOKEN == theTokenId)
  {
    if (myPrintTokens)
    {
      std::cout << "Token: " << myScannerTable.getTokenName(theTokenId)
                << " (\"" << theToken << "\")" << std::endl;
    }
  }
}
