#ifndef TOKEN_H
#define TOKEN_H

/**
 * @file Token.h
 * @brief Class which defines a token.
 *
 * @author Michael Albers
 */

#include <cstdint>
#include <memory>
#include <string>

#include "Symbol.h"

/**
 * Class which defines a token. A token consists of the token which has
 * been scanned and the terminal symbol which is the grammatical representation
 * of this token.
 */
class Token
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  Token() = default;

  /**
   * Copy constructor.
   */
  Token(const Token&) = default;

  /**
   * Move constructor.
   */
  Token(Token &&) = default;

  /**
   * Destructor
   */
  ~Token() = default;

  /**
   * Copy assignment operator.
   */
  Token& operator=(const Token&) = default;

  /**
   * Move assignment operator.
   */
  Token& operator=(Token&&) = default;

  /**
   * Stream insertion operator
   *
   * @param theOS
   *          stream to insert into
   * @param theToken
   *          object to insert into theOS
   * @return modified stream
   */
  friend std::ostream& operator<<(std::ostream &theOS,
                                  const Token &theToken) noexcept;

  /**
   * Appends the given character to the token.
   */
  void append(char theCharacter) noexcept;

  /**
   * Clears the terminal (nulls it) and token (empty string).
   */
  void clear() noexcept;

  /**
   * Returns the column on which this token appears.
   *
   * @return column number
   */
  uint32_t getColumn() const noexcept;

  /**
   * Returns the line on which this token appears.
   *
   * @return line number
   */
  uint32_t getLine() const noexcept;

  /**
   * Returns the terminal symbol.
   *
   * @return terminal symbol
   */
  std::shared_ptr<Symbol> getTerminal() const noexcept;

  /**
   * Returns the token.
   *
   * @return token
   */
  std::string getToken() const noexcept;

  /**
   * Sets the position of this token within the source file.
   *
   * @param theLine
   *          file line number
   * @param theColumn
   *          column in the line
   */
  void setPosition(uint32_t theLine, uint32_t theColumn) noexcept;

  /**
   * Sets the terminal symbol for this token.
   *
   * @param theTerminal
   *          terminal symbol
   */
  void setTerminal(std::shared_ptr<Symbol> theTerminal) noexcept;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Column on which the token appears. */
  uint32_t myColumn = 0;

  /** Line on which the token appears. */
  uint32_t myLine = 0;

  /** Terminal symbol for this token. */
  std::shared_ptr<Symbol> myTerminal;

  /** Scanned string */
  std::string myToken;
};

#endif
