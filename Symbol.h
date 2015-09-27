#ifndef SYMBOL_H
#define SYMBOL_H

/**
 * @file Symbol.h
 * @brief Base class for all symbols types in a grammar.
 *
 * @author Michael Albers
 */

#include <ostream>
#include <string>

/**
 * Base type for all symbols in a grammar.
 */
class Symbol
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  Symbol() = delete;

  /**
   * Copy constructor.
   */
  Symbol(const Symbol&) = default;

  /**
   * Move constructor.
   */
  Symbol(Symbol &&) = default;

  /**
   * Constructor.
   *
   * @param theName
   *          symbol name
   */
  Symbol(const std::string &theName);

  /**
   * Destructor
   * Making this pure virtual to make class abstract (there is no other pure
   * virtual method needed for this class, so I found this trick at
   * http://stackoverflow.com/questions/4640985/making-a-class-abstract-without-any-pure-virtual-methods)
   */
  virtual ~Symbol() = 0;

  /**
   * Copy assignment operator.
   */
  Symbol& operator=(const Symbol&) = default;

  /**
   * Move assignment operator.
   */
  Symbol& operator=(Symbol&&) = default;

  /**
   * Returns the symbol's name.
   *
   * @return symbol's name
   */
  virtual std::string getName() const noexcept;

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
                                  const Symbol &theSymbol);

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
  virtual void print(std::ostream &theOS) const noexcept;

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Symbol name */
  const std::string myName;

};

#endif
