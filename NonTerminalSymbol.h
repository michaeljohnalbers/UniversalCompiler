#ifndef NONTERMINALSYMBOL_H
#define NONTERMINALSYMBOL_H

/**
 * @file NonTerminalSymbol.h
 * @brief Class for a non-terminal symbol in a grammar.
 *
 * @author Michael Albers
 */

#include <string>

#include "Symbol.h"

/**
 * Class for a non-terminal symbol in a grammar.
 */
class NonTerminalSymbol : public Symbol
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  NonTerminalSymbol() = delete;

  /**
   * Copy constructor.
   */
  NonTerminalSymbol(const NonTerminalSymbol&) = default;

  /**
   * Move constructor.
   */
  NonTerminalSymbol(NonTerminalSymbol &&) = default;

  /**
   * Constructor.
   *
   * @param theName
   *          symbol name
   */
  NonTerminalSymbol(const std::string &theName);

  /**
   * Destructor
   */
  ~NonTerminalSymbol() = default;

  /**
   * Copy assignment operator.
   */
  NonTerminalSymbol& operator=(const NonTerminalSymbol&) = default;

  /**
   * Move assignment operator.
   */
  NonTerminalSymbol& operator=(NonTerminalSymbol&&) = default;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

};

#endif
