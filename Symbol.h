#ifndef SYMBOL_H
#define SYMBOL_H

/**
 * @file Symbol.h
 * @brief Base class for all symbols types in a grammar.
 *
 * @author Michael Albers
 */

#include <ostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

class Symbol;
class SymbolList;

class SymbolCompare
{
  public:

  /**
   * Comparison operator for use with shared_ptr.
   *
   * @param theLHS
   *          lhs of lhs == rhs
   * @param theRHS
   *          rhs of lhs == rhs
   * @param true if two Symbols are equal
   */
  bool operator()(const std::shared_ptr<Symbol> theLHS,
                  const std::shared_ptr<Symbol> theRHS) const noexcept;
};

/**
 * Base type for all symbols in a grammar.
 */
class Symbol
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  using SymbolList = std::vector<std::shared_ptr<Symbol>>;
  using SymbolSet = std::set<std::shared_ptr<Symbol>, SymbolCompare>;

  /**
   * Stream insertion operator.
   *
   * @param theOS
   *          stream to insert into
   * @param theList
   *          List of symobls
   * @return modified stream
   */
  friend std::ostream& operator<<(std::ostream &theOS,
                                  const Symbol::SymbolList &theList);

  /**
   * Stream insertion operator.
   *
   * @param theOS
   *          stream to insert into
   * @param theSet
   *          Set of symobls
   * @return modified stream
   */
  friend std::ostream& operator<<(std::ostream &theOS,
                                  const Symbol::SymbolSet &theSet);
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
   * Less-than operator for use with shared_ptr.
   *
   * @param theRHS
   *          rhs of this < rhs
   * @param true if this < rhs
   */
  virtual bool operator<(const Symbol &theRHS) const noexcept;

  /**
   * Adds the given symbol to the first set.
   *
   * @param theSymbol
   *          symbol to add
   */
  virtual void addToFirstSet(std::shared_ptr<Symbol> theSymbol) noexcept;

  /**
   * Returns true if this symbol derives lambda
   *
   * @return true if this symbol derives lambda
   */
  virtual bool getDerivesLambda() const noexcept;

  /**
   * Returns the first set for this symbol
   *
   * @return the first set for this symbol
   */
  virtual const SymbolSet& getFirstSet() const noexcept;

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

  /**
   * Sets if this symbols derives lambda
   *
   * @param theDerivesLambda
   *          true if this symbols derives lambda
   */
  virtual void setDerivesLambda(bool theDerivesLambda) noexcept;

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

  /** Does this symbol eventually derive lambda? */
  bool myDerivesLambda = false;

  /** First set for this symbol. */
  SymbolSet myFirstSet;

  /** Symbol name */
  const std::string myName;

};

#endif
