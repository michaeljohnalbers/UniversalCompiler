#ifndef EOPSYMBOL_H
#define EOPSYMBOL_H

/**
 * @file EOPSymbol.h
 * @brief Class for a EOP (End of Production) pseudo-symbol.
 *
 * @author Michael Albers
 */

#include <cstdint>

#include "Symbol.h"

/**
 * Class for a EOP (End of Production) pseudo-symbol. The EOP symbol is,
 * in fact, not a symbol at all. Rather it's just a tool used to save some
 * state from the Semantic Stack. The only reason it's a 'Symbol' is so
 * that it can be pushed onto the parse stack.
 */
class EOPSymbol : public Symbol
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  EOPSymbol() = default;

  /**
   * Copy constructor.
   */
  EOPSymbol(const EOPSymbol&) = default;

  /**
   * Move constructor.
   */
  EOPSymbol(EOPSymbol &&) = default;

  /**
   * Constructor.
   *
   * @param theCurrentIndex
   *          current index to save
   * @param theLeftIndex
   *          left index to save
   * @param theRightIndex
   *          right index to save
   * @param theTopIndex
   *          top index to save
   */
  EOPSymbol(uint32_t theCurrentIndex, uint32_t theLeftIndex,
            uint32_t theRightIndex, uint32_t theTopIndex);

  /**
   * Destructor
   */
  virtual ~EOPSymbol() = default;

  /**
   * Always returns false
   *
   * @return false
   */
  virtual bool getDerivesLambda() const noexcept override;

  /**
   * Gets the saved index values.
   *
   * @param theCurrentIndex
   *          current index to set
   * @param theLeftIndex
   *          left index to set
   * @param theRightIndex
   *          right index to set
   * @param theTopIndex
   *          top index to set
   */
  void getValues(uint32_t &theCurrentIndex, uint32_t &theLeftIndex,
                 uint32_t &theRightIndex, uint32_t &theTopIndex) const noexcept;

  /**
   * Copy assignment operator.
   */
  EOPSymbol& operator=(const EOPSymbol&) = default;

  /**
   * Move assignment operator.
   */
  EOPSymbol& operator=(EOPSymbol&&) = default;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  /**
   * Add data to theOS. Workaround for not being able to have a virtual <<
   * operator.
   *
   * @param theOS
   *          stream to insert into
   */
  virtual void print(std::ostream &theOS) const noexcept override;

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Top of the stack. */
  uint32_t myCurrentIndex = 0;

  /** Index of semantic record for LHS of production. */
  uint32_t myLeftIndex = 0;

  /** Index of first semantic record from production RHS. */
  uint32_t myRightIndex = 0;

  /** Next index in the stack to use. */
  uint32_t myTopIndex = 0;
};

#endif
