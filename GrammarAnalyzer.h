#ifndef GRAMMARANALYZER_H
#define GRAMMARANALYZER_H

/**
 * @file GrammarAnalyzer.h
 * @brief Reads in a grammar (BNF) defined in a file and stores the data.
 *
 * @author Michael Albers
 */

#include <fstream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Grammar.h"
#include "NonTerminalSymbol.h"
#include "Symbol.h"

class Production;

/**
 * This class accepts a Grammar and populates the first/follow/predict sets.
 */
class GrammarAnalyzer
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor
   */
  GrammarAnalyzer() = delete;

  /**
   * Copy constructor
   */
  GrammarAnalyzer(const GrammarAnalyzer&) = default;

  /**
   * Move constructor
   */
  GrammarAnalyzer(GrammarAnalyzer&&) = default;

  /**
   * Constructor. Populates the first/follow/predict sets of the provided
   * grammar.
   *
   * @param theGrammar
   *          grammar object to populate
   */
  GrammarAnalyzer(Grammar &theGrammar);

  /**
   * Destructor
   */
  ~GrammarAnalyzer() = default;

  /**
   * Copy assignment operator
   */
  GrammarAnalyzer& operator=(const GrammarAnalyzer&) = default;

  /**
   * Move assignment operator
   */
  GrammarAnalyzer& operator=(GrammarAnalyzer&&) = default;

  /**
   * Insertion operator
   *
   * @param theOS
   *          stream to insert into
   * @param theAnalyzer
   *          data to insert
   * @return modified stream
   */
  friend std::ostream& operator<<(std::ostream &theOS,
                                  const GrammarAnalyzer &theAnalyzer);

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /**
   * Determines which of the non-terminal symbols derives lambda
   */
  void calculateDerivesLambda() noexcept;

  /**
   * Returns the first set of the provided set of ordered symbols.
   *
   * @param list of symbols
   * @return first set.
   */
  Symbol::SymbolSet computeFirst(const Symbol::SymbolList &theSymbols)
    const noexcept;

  /**
   * Checks if lambda is in the given set of symbols.
   *
   * @param theSymbols
   *          set of symbols to check
   * @return true if lambda is in the set, false otherwise.
   */
  static bool containsLambda(const Symbol::SymbolSet &theSymbols) noexcept;

  /**
   * Fills the first sets for all symbols.
   */
  void fillFirstSets() noexcept;

  /**
   * Fills the follow sets for all non-terminal symbols.
   */
  void fillFollowSets() noexcept;

  /**
   * Generates the predict sets for each production.
   */
  void generatePredictSets() noexcept;

  /** Grammar definition. */
  Grammar &myGrammar;

  /** Set of all non-terminal symbols in the productions. */
  Symbol::SymbolSet myNonTerminalSymbols;

  /** All productions */
  std::vector<std::shared_ptr<Production>> myProductions;

  /** Set of all symbols in the productions . */
  Symbol::SymbolSet mySymbols;

  /** Set of all terminal symbols in the productions. */
  Symbol::SymbolSet myTerminalSymbols;
};

#endif
