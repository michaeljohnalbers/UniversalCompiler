#ifndef NONTERMINALSYMBOL_H
#define NONTERMINALSYMBOL_H

/**
 * @file NonTerminalSymbol.h
 * @brief Class for a non-terminal symbol in a grammar.
 *
 * @author Michael Albers
 */

#include <memory>
#include <set>
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

  /**
   * Adds the given symbol to the follow set.
   *
   * @param theSymbol
   *          symbol to add
   */
  void addToFollowSet(std::shared_ptr<Symbol> theSymbol) noexcept;

  /**
   * Returns the follow set for this symbol
   *
   * @return the follow set for this symbol
   */
  const SymbolSet& getFollowSet() const noexcept;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Follow set of this symbol. */
  SymbolSet myFollowSet;
};

#endif
