#ifndef TERMINALSYMBOL_H
#define TERMINALSYMBOL_H

/**
 * @file TerminalSymbol.h
 * @brief Class for a terminal symbol in a grammar.
 *
 * @author Michael Albers
 */

#include <string>

#include "Symbol.h"

/**
 * Class for a terminal symbol in a grammer.
 */
class TerminalSymbol : public Symbol
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  TerminalSymbol() = delete;

  /**
   * Copy constructor.
   */
  TerminalSymbol(const TerminalSymbol&) = default;

  /**
   * Move constructor.
   */
  TerminalSymbol(TerminalSymbol &&) = default;

  /**
   * Constructor.
   *
   * @param theName
   *          symbol name
   */
  TerminalSymbol(const std::string &theName);

  /**
   * Destructor
   */
  ~TerminalSymbol() = default;

  /**
   * Copy assignment operator.
   */
  TerminalSymbol& operator=(const TerminalSymbol&) = default;

  /**
   * Move assignment operator.
   */
  TerminalSymbol& operator=(TerminalSymbol&&) = default;

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
