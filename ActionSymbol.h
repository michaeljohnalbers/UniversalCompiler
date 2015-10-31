#ifndef ACTIONSYMBOL_H
#define ACTIONSYMBOL_H

/**
 * @file ActionSymbol.h
 * @brief Class for a action symbol in a grammar.
 *
 * @author Michael Albers
 */

#include "Symbol.h"

/**
 * Class for a action symbol in a grammer. An action symbol represents a
 * call to a semantic function. Not a symbol in the traditional grammar
 * sense.
 */
class ActionSymbol : public Symbol
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  ActionSymbol() = delete;

  /**
   * Copy constructor.
   */
  ActionSymbol(const ActionSymbol&) = default;

  /**
   * Move constructor.
   */
  ActionSymbol(ActionSymbol &&) = default;

  /**
   * Constructor.
   *
   * @param theName
   *          symbol name
   */
  ActionSymbol(const std::string &theName);

  /**
   * Destructor
   */
  ~ActionSymbol() = default;

  /**
   * Always returns false
   *
   * @return false
   */
  virtual bool getDerivesLambda() const noexcept override;

  /**
   * Copy assignment operator.
   */
  ActionSymbol& operator=(const ActionSymbol&) = default;

  /**
   * Move assignment operator.
   */
  ActionSymbol& operator=(ActionSymbol&&) = default;

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
