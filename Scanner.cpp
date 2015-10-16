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

  Token token;
  do
  {
    token = getToken();
    myTokens.push_back(token);
  } while (!(token.getTerminal() == myScannerTable.getEOF()));
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
    myColumn = 1;
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
// Scanner::getRemainingTokens
//*******************************************************
std::deque<Token> Scanner::getRemainingTokens() const noexcept
{
  return myTokens;
}

//*******************************************************
// Scanner::getToken
//*******************************************************
Token Scanner::getToken()
{
  Token token;

  ScannerTable::State currentState;

  // Resets the variables in this function to (re)start the scan from a fresh
  // token.
  auto reset = [&token, &currentState]()
  {
    token.clear();
    currentState = ScannerTable::START_STATE;
  };

  auto isNoTerminal = [](std::shared_ptr<Symbol> theTerminal)->bool
  {
    bool isNoTerminal = false;
    try
    {
      isNoTerminal =
        (dynamic_cast<TerminalSymbol*>(theTerminal.get())->getId() ==
         ScannerTable::NO_TERMINAL);
    }
    catch (std::bad_cast){/* Just in case */}

    return isNoTerminal;
  };

  reset();
  token.setPosition(myLine, myColumn);

  while (currentChar() >= 0) // -1 is returned on EOF
  {
    auto action = myScannerTable.getAction(currentState, currentChar());

    switch (action)
    {
      case ScannerTable::Action::Error:
      {
        token.append(currentChar());
        consumeChar();

        std::string error;
        error += "invalid token: '" + token.getToken() + "'";
        myEWTracker.reportError(getLine(), getColumn(), error);

        reset(); // Error recovery
      }
      break;

      case ScannerTable::Action::MoveAppend:
        currentState = myScannerTable.getState(currentState,
                                               currentChar());
        token.append(currentChar());
        consumeChar();
        break;

      case ScannerTable::Action::MoveNoAppend:
        currentState = myScannerTable.getState(currentState,
                                               currentChar());
        consumeChar();
        break;

      case ScannerTable::Action::HaltAppend:
      {
        token.append(currentChar());
        auto terminal = myScannerTable.lookupTerminal(currentState,
                                                      currentChar(),
                                                      token.getToken());
        token.setTerminal(terminal);
        consumeChar();
        if (isNoTerminal(terminal))
        {
          return getToken();
        }
      }
      return token;

      case ScannerTable::Action::HaltNoAppend:
      {
        auto terminal = myScannerTable.lookupTerminal(currentState,
                                                      currentChar(),
                                                      token.getToken());
        token.setTerminal(terminal);
        consumeChar();
        if (isNoTerminal(terminal))
        {
          return getToken();
        }
      }
      return token;

      case ScannerTable::Action::HaltReuse:
      {
        auto terminal = myScannerTable.lookupTerminal(currentState,
                                                      currentChar(),
                                                      token.getToken());
        token.setTerminal(terminal);
        if (isNoTerminal(terminal))
        {
          return getToken();
        }
      }
      return token;
    }
  }

  if (myInputStream.eof())
  {
    token.clear();
    token.append('$'); // Just to match examples in lecture 15 PDF.
    token.setTerminal(myScannerTable.getEOF());
    token.setPosition(myLine, myColumn);
  }

  return token;
}

//*******************************************************
// Scanner::scan
//*******************************************************
Token Scanner::scan()
{
  Token token{myTokens.front()};
  // Allow so multiple calls to scan after EOF keep returning EOF.
  if (myTokens.size() > 1)
  {
    myTokens.pop_front();
  }

  if (myPrintTokens)
  {
    std::cout << "Token: " << token << std::endl;
  }
  return token;
}
