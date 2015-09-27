#ifndef PRODUCTION_H
#define PRODUCTION_H

/**
 * @file Production.h
 * @brief Class for a production within the grammar
 *
 * @author Michael Albers
 */

#include <cstdint>
#include <memory>
#include <ostream>
#include <vector>

#include "NonTerminalSymbol.h"

class Symbol;

std::ostream& operator<<(
    std::ostream &theOS,
    const std::vector<std::shared_ptr<Symbol>> &theRHS);

/**
 * Class for a production within the grammer.
 */
class Production
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor
   */
  Production() = delete;

  /**
   * Copy constructor
   */
  Production(const Production&) = default;

  /**
   * Move constructor
   */
  Production(Production&&) = default;

  /**
   * Constructor
   *
   * @param theLHS
   *          LHS symbol of the production
   * @param theNumber
   *          numeric identifier of the production
   */
  Production(const NonTerminalSymbol &theLHS, uint32_t theNumber);

  /**
   * Destructor
   */
  ~Production() = default;

  /**
   * Adds the given symbol to the list of RHS symbols.
   *
   * @param theRHSSymbol
   *          symbol (terminal or non-terminal) to add
   */
  void addRHSSymbol(std::shared_ptr<Symbol> theRHSSymbol) noexcept;

  /**
   * Returns the LHS of this production.
   *
   * @return the LHS of this production
   */
  NonTerminalSymbol getLHS() const noexcept;

  /**
   * Returns the numeric identifier of this production.
   *
   * @return the numeric identifier of this production.
   */
  uint32_t getNumber() const noexcept;

  /**
   * Returns the RHS symbols.
   *
   * @return the RHS symbols.
   */
  std::vector<std::shared_ptr<Symbol>> getRHS() const noexcept;

  /**
   * Copy assignment operator
   */
  Production& operator=(const Production&) = default;

  /**
   * Move assignment operator
   */
  Production& operator=(Production&&) = default;

  /**
   * Stream insertion operator.
   *
   * @param theOS
   *          stream to insert into
   * @param theProduction
   *          production to insert into theOS
   * @return modified stream
   */
  friend std::ostream& operator<<(std::ostream &theOS,
                                  const Production &theProduction);

  /**
   * Stream insertion operator.
   *
   * @param theOS
   *          stream to insert into
   * @param theRhs
   *          RHS of a production to insert into theOS
   * @return modified stream
   */
  friend std::ostream& operator<<(
    std::ostream &theOS,
    const std::vector<std::shared_ptr<Symbol>> &theRHS);

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** LHS of production */
  NonTerminalSymbol myLHS;

  /** Numeric identifier of the production. */
  const uint32_t myNumber;

  /** Right hand side of production, in order. */
  std::vector<std::shared_ptr<Symbol>> myRHS;

};

#endif
