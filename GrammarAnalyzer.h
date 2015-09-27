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

class Symbol;
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

  void consumeWhitespace(std::string &theLine);

  std::shared_ptr<Symbol> makeSymbol(const std::string &theSymbol);

  void openFile(const std::string &theFileName,
                std::ifstream &theFile);

  void parseFile(std::ifstream &theFile);

  std::string readSymbol(std::string &theLine);

  std::set<std::string> myNonTerminalSymbols;

  std::set<std::string> mySymbols;

  std::set<std::string> myTerminalSymbols;

  std::vector<std::shared_ptr<Production>> myProductions;
};

#endif
