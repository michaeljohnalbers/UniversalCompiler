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

#include "NonTerminalSymbol.h"
#include "Symbol.h"

class Production;

/**
 * Class to read in a grammar in BNF from a file and store the productions and
 * symbols. This isn't the smartest of parsers, don't get too clever.
 * File format:
 *  - The file must have one production per file
 *  - Each token must be separated by at least one whitespace character for
 *    the previous and next token.
 *  - Empty lines and lines which start with a '#' are ignored
 *  - Do NOT deviate from BNF. No incomplete lines or anything like that.
 *  - Does not accept '|' for multiple productions per LHS
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
   * Constructor. Reads in the file an populates this object
   *
   * @param theFileName
   *          name of the file containing the grammar
   */
  GrammarAnalyzer(const std::string &theFileName);

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
   * Erases all whitespace (space and tab) from the start of the string until
   * the first non-whitespace character
   *
   * @param theLine
   *          line from which to erease whitespace
   */
  void consumeWhitespace(std::string &theLine);

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

  /**
   * Returns a Symbol for the given non-terminal.
   *
   * @param theSymbol
   *          non-terminal symbol
   * @return Symbol
   */
  std::shared_ptr<Symbol> makeNonTerminal(
    const std::string &theSymbol) noexcept;

  /**
   * Creates a Symbol object from the given symbol string.
   *
   * @param theSymbol
   *          symbol string
   * @param new symbol
   */
  std::shared_ptr<Symbol> makeSymbol(const std::string &theSymbol);

  /**
   * Returns a Symbol for the given terminal.
   *
   * @param theSymbol
   *          terminal symbol
   * @return Symbol
   */
  std::shared_ptr<Symbol> makeTerminal(
    const std::string &theSymbol) noexcept;

  /**
   * Opens the given file with the given file object.
   *
   * @param theFileName
   *          name of the file to open
   * @param theFile
   *          file object used to open the file.
   */
  void openFile(const std::string &theFileName,
                std::ifstream &theFile);

  /**
   * Reads the grammar file.
   *
   * @param theFile
   *          open file object
   */
  void parseFile(std::ifstream &theFile);

  /**
   * Reads a single symbol from the start of the input line then erases that
   * symbol from the line
   *
   * @param theLine
   *          line containing symbols
   * @return symbol read
   */
  std::string readSymbol(std::string &theLine);

  /** Set of all non-terminal symbols in the productions. */
  Symbol::SymbolSet myNonTerminalSymbols;

  /** Set of all symbols in the productions . */
  Symbol::SymbolSet mySymbols;

  /** Set of all terminal symbols in the productions. */
  Symbol::SymbolSet myTerminalSymbols;

  /** All productions */
  std::vector<std::shared_ptr<Production>> myProductions;
};

#endif
