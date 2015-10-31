#ifndef SEMANTICSTACK_H
#define SEMANTICSTACK_H

/**
 * @file SemanticStack.h
 * @brief Manages the semantic stack.
 *
 * @author Michael Albers
 */

#include <cstdint>
#include <memory>
#include <vector>

#include "SemanticRecord.h"

class Symbol;

/**
 * A semantic stack for the compiler. The semantic stack contains semantic
 * information about the tokens which have been encountered during the
 * parse. This is nowhere near a true stack as more than just the top
 * element is accessed and manipulated.
 */
class SemanticStack
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  SemanticStack();

  /**
   * Copy constructor.
   */
  SemanticStack(const SemanticStack&) = default;

  /**
   * Move constructor.
   */
  SemanticStack(SemanticStack &&) = default;

  /**
   * Destructor
   */
  virtual ~SemanticStack() = default;

  /**
   * Copy assignment operator.
   */
  SemanticStack& operator=(const SemanticStack&) = default;

  /**
   * Move assignment operator.
   */
  SemanticStack& operator=(SemanticStack&&) = default;

  /**
   * Expand the stack by the given number of elements.
   *
   * @param theNumberElementsToAdd
   *          number of elements to add to the stack.
   */
  void expand(uint32_t theNumberElementsToAdd) noexcept;

  /**
   * Returns an EOPSymbol with the current state of the semantic stack.
   *
   * @return EOPSymbol with stack state.
   */
  std::shared_ptr<Symbol> getEOPSymbol() const noexcept;

  /**
   * Returns the record at currentIndex - 1. Specialty function provided for
   * semantic routines.
   *
   * @return record at currentIndex - 1
   */
  SemanticRecord& getRecordAtCurrentIndexMinusOne() noexcept;

  /**
   * Returns the semantic record for the given action symbol argument (i.e.,
   * "$$" or "$3")
   *
   * @param theArgument
   *          action sybmol argument
   * @return semantic record
   */
  SemanticRecord& getRecordFromArgument(std::string theArgument) noexcept;

  /**
   * Returns the semantic stack for printing out code generation steps.
   *
   * @return semantic stack
   */
  std::vector<SemanticRecord> getStack() const noexcept;

  /**
   * Initializes the stack. (Essentially just code for
   * the initial push from the class slides.)
   */
  void initialize() noexcept;

  /**
   * Replaces the element at current index with the given element.
   *
   * @param theNewRecord
   *          new record for replacement
   */
  void replaceAtCurrentIndex(const SemanticRecord &theNewRecord) noexcept;

  /**
   * Restores state from the given EOP symbol.
   *
   * @param theEOPSymbol
   *          state restoration object
   */
  void restore(std::shared_ptr<Symbol> theEOPSymbol) noexcept;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /**
   * Given an action symbol argument ($$, $1, etc.) this function returns
   * the semantic stack index.
   *
   * @param theArguments
   *          action symbol argument
   * @return semantic stack index
   */
  uint32_t getIndex(std::string theArgument) const noexcept;

  /** Top of the stack. */
  uint32_t myCurrentIndex = 0;

  /** Index of semantic record for LHS of production. */
  uint32_t myLeftIndex = 0;

  /** Index of first semantic record from production RHS. */
  uint32_t myRightIndex = 0;

  /** Next index in the stack to use. */
  uint32_t myTopIndex = 0;

  /**
   * Fake stack. For stack-like operations the end of the vector
   * can be considered the top of the stack.
   */
  std::vector<SemanticRecord> mySemanticStack;
};

#endif
