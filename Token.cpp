/**
 * @file Token.cpp
 * @brief Implementation of Token class
 *
 * @author Michael Albers
 */

#include "Token.h"

//*******************************************************
// Token::append
//*******************************************************
void Token::append(char theCharacter) noexcept
{
  myToken.push_back(theCharacter);
}

//*******************************************************
// Token::clear
//*******************************************************
void Token::clear() noexcept
{
  myColumn = 0;
  myLine = 0;
  myTerminal = nullptr;
  myToken.clear();
}

//*******************************************************
// Token::getColumn
//*******************************************************
uint32_t Token::getColumn() const noexcept
{
  return myColumn;
}

//*******************************************************
// Token::getLine
//*******************************************************
uint32_t Token::getLine() const noexcept
{
  return myLine;
}

//*******************************************************
// Token::getTerminal
//*******************************************************
std::shared_ptr<Symbol> Token::getTerminal() const noexcept
{
  return myTerminal;
}

//*******************************************************
// Token::getToken
//*******************************************************
std::string Token::getToken() const noexcept
{
  return myToken;
}

//*******************************************************
// Token::setPosition
//*******************************************************
void Token::setPosition(uint32_t theLine, uint32_t theColumn) noexcept
{
  myLine = theLine;
  myColumn = theColumn;
}

//*******************************************************
// Token::getToken
//*******************************************************
void Token::setTerminal(std::shared_ptr<Symbol> theTerminal) noexcept
{
  myTerminal = theTerminal;
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS,
                         const Token &theToken) noexcept
{
  theOS << theToken.getToken() << " (" << theToken.getTerminal()->getName()
        << ") at " << theToken.getLine() << ":" << theToken.getColumn();
  return theOS;
}
