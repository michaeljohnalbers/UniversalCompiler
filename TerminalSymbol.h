#ifndef TERMINALSYMBOL_H
#define TERMINALSYMBOL_H

/**
 * @file TerminalSymbol.h
 * @brief Class for a terminal symbol in a grammar.
 *
 * @author Michael Albers
 */

#include <cstdint>
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

  /** Terminal ID*/
  using Id = uint32_t;

  /**
   * Print name/ID/reserved word? Ugly, terrible hack, but not worth doing it
   * correctly.
   */
  static bool ourLongPrint;

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
   * @param theId
   *          terminal Id number
   * @param theResevedWord
   *          reseved word, use "" for non-reserved word terminals
   */
  TerminalSymbol(const std::string &theName,
                 Id theId,
                 const std::string &theResevedWord);

  /**
   * Destructor
   */
  ~TerminalSymbol() = default;

  /**
   * Always returns false
   *
   * @return false
   */
  virtual bool getDerivesLambda() const noexcept override;

  /**
   * Returns the ID of this terminal.
   */
  Id getId() const noexcept;

  /**
   * Returns the reserved word for this terminal. For non-reserved word
   * terminals the empty string is returned.
   *
   * @return reserved word or ""
   */
  std::string getReservedWord() const noexcept;

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

  /**
   * Inserts symbol information into the given stream.
   *
   * @param theOS
   *          modified stream
   */
  virtual void print(std::ostream &theOS) const noexcept override;

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Terminal Id. */
  const Id myId;

  /** Reserved word for those terminals which are reseved words */
  const std::string myReservedWord;
};

#endif
