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

#include "Symbol.h"

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
  Production(const std::shared_ptr<Symbol> theLHS, uint32_t theNumber);

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
   * Adds the given symbol to the predict set of this production
   *
   * @param theSymbol
   *          symbol to add to the predict set
   */
  void addToPredictSet(std::shared_ptr<Symbol> theSymbol) noexcept;

  /**
   * Returns the LHS of this production.
   *
   * @return the LHS of this production
   */
  std::shared_ptr<Symbol> getLHS() const noexcept;

  /**
   * Returns the numeric identifier of this production.
   *
   * @return the numeric identifier of this production.
   */
  uint32_t getNumber() const noexcept;

  /**
   * Returns the predict set for this production.
   *
   * @return predict set
   */
  Symbol::SymbolSet getPredictSet() const noexcept;

  /**
   * Returns the RHS symbols.
   *
   * @return the RHS symbols.
   */
  Symbol::SymbolList getRHS() const noexcept;

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

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** LHS of production */
  std::shared_ptr<Symbol> myLHS;

  /** Numeric identifier of the production. */
  const uint32_t myNumber;

  /** Set of symbols which predicts this production. */
  Symbol::SymbolSet myPredictSet;

  /** Right hand side of production, in order. */
  Symbol::SymbolList myRHS;

};

#endif
