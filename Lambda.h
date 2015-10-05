#ifndef LAMBDA_H
#define LAMBDA_H

/**
 * @file Lambda.h
 * @brief Defines Lambda.
 *
 * @author Michael Albers
 */

#include "Symbol.h"

/**
 * Defines Lambda. Lambda isn't actually a symbol however, given the design
 * of the rest of the UniversalCompiler it does need to derive from Symbol.
 * To achieve some amount of integrity it isn't being defined as a terminal
 * or non-terminal
 */
class Lambda : public Symbol
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Copy constructor.
   */
  Lambda(const Lambda&) = delete;

  /**
   * Move constructor.
   */
  Lambda(Lambda &&) = delete;

  /**
   * Destructor
   */
  virtual ~Lambda() = default;

  /**
   * Copy assignment operator.
   */
  Lambda& operator=(const Lambda&) = delete;

  /**
   * Move assignment operator.
   */
  Lambda& operator=(Lambda&&) = delete;

  /**
   * @see Symbol.h 
   */
  virtual void addToFirstSet(std::shared_ptr<Symbol> theSymbol) noexcept
    override;

  /**
   * Returns true if this symbol derives lambda
   *
   * @return true if this symbol derives lambda
   */
  virtual bool getDerivesLambda() const noexcept override;

  /**
   * Returns the first set for this symbol
   *
   * @return the first set for this symbol
   */
  virtual const SymbolSet& getFirstSet() const noexcept override;

  /**
   * Returns the singleton lambda.
   *
   * @return the singleton lambda.
   */
  static std::shared_ptr<Symbol> getInstance() noexcept;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  /**
   * Default constructor.
   */
  Lambda();

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Singleton lambda. */
  static const std::shared_ptr<Symbol> ourLambda;

  /** Special first set for lambda. */
  SymbolSet myFirstSet;
};

#endif
