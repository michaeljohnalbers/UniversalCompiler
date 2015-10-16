#ifndef GRAMMAR_H
#define GRAMMAR_H

/**
 * @file Grammar.h
 * @brief Class which defines a grammer.
 *
 * @author Michael Albers
 */

#include <fstream>
#include <map>
#include <memory>
#include <ostream>
#include <string>

#include "ScannerTable.h"
#include "Symbol.h"

class ErrorWarningTracker;
class Production;


/**
 * This class defines an LL(1) grammar, along with some extra information. It
 * contains definitions for the symbols, all productions and the start
 * symbol.
 *
 * When a pointer (or set/vector of pointers) is returned, the pointers will
 * always point to the internally stored objects.
 *
 * The information for the grammar is taken from a file. This class assumes
 * no errors in the file (I didn't want to write a smart scanner/parser).
 *
 * File format:
 * Each section is delineated by a line of 5 hyphens.
 *
 * Section 1: Terminal symbol definitons.
 * Name/number pairs. One per line. Numbers are unsigned.
 * Reserved words have 3 entries per line (last entry is the actual reserved
 * word. Two terminals are built in:
 * 98 - No terminal, used for returning whitespace or comments. Essentially any
 *      string of text in the source file which isn't a terminal
 * 99 - EOF, end of file. There should be no need to use this in the grammar
 *      definition file
 *
 * Section 2: Scanner Table
 * Defines REs in a table to scan all terminals. First row is the column
 * definition. Special definitions:
 *   letter (A-Za-z)
 *   digit (0-9)
 *   whitespace (' ' and '\t')
 *   EOL (\n)
 *   Other (any character not already specified)
 * Each entry is 3 sub-fields, colon separated
 *    next_state:action_acronym:terminal_id
 * next_state only applies to the "Move*" actions, terminal_id only applies
 * to "Halt*" actions.
 *
 * Section 3: Productions
 * Standard form from class. Each symbol MUST have a space around it.
 *
 * Section 4: Start Symbol
 * Name of the start symbol, with the enclosing '<>'
 *
 * Example File (not actually valid):
 *
 * 1 BeginSym begin
 * 2 EndSym end
 * 3 Id
 * -----
 * letter  digit   whitespace +       -       EOL      Other
 * 1:MA:0  2:MA:0  3:MNA:0    0:HA:3  4:MA:0  3:MNA:0  E
 * -----
 * <program> -> begin <stmt list> end
 * <stmt list> -> <statement> <statement tail>
 * -----
 * <system goal>
 */
class Grammar
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor
   */
  Grammar() = delete;

  /**
   * Copy constructor
   */
  Grammar(const Grammar&) = default;

  /**
   * Move constructor
   */
  Grammar(Grammar&&) = default;

  /**
   * Constructor
   *
   * @param theFileName
   *          name of the file containing grammar information
   * @param theEWTracker
   *          error/warning tracker
   * @param theScannerTable
   *          scanner table to populate
   * @throws std::runtime_error
   *          on error reading the file, errors reported through EWTracker
   */
  Grammar(const std::string &theFileName,
          ErrorWarningTracker &theEWTracker,
          ScannerTable &theScannerTable);

  /**
   * Destructor
   */
  ~Grammar() = default;

  /**
   * Copy assignment operator
   */
  Grammar& operator=(const Grammar &) = default;

  /**
   * Move assignment operator
   */
  Grammar& operator=(Grammar &&) = default;

  /**
   * Stream insertion operator
   *
   * @param theOS
   *          stream to insert into
   * @param theGrammar
   *          object to insert into theOS
   * @return modified stream
   */
  friend std::ostream& operator<<(std::ostream &theOS,
                                  const Grammar &theGrammar) noexcept;

  /**
   * Returns the set of non-terminal symbols.
   *
   * @return the set of non-terminal symbols.
   */
  Symbol::SymbolSet getNonTerminalSymbols() const noexcept;

  /**
   * Returns the production for the given number.
   *
   * @param theProductionNumber
   *          production number (1-based)
   * @return production
   */
  std::shared_ptr<Production> getProduction(uint32_t theProductionNumber)
    const noexcept;

  /**
   * Returns the productions of this grammar.
   *
   * @return grammar productions
   */
  std::vector<std::shared_ptr<Production>> getProductions() const noexcept;

  /**
   * Returns the start symbol of this grammar.
   *
   * @return start symbol
   */
  std::shared_ptr<Symbol> getStartSymbol() const noexcept;

  /**
   * Returns the set of terminal symbols.
   *
   * @return the set of terminal symbols.
   */
  Symbol::SymbolSet getTerminalSymbols() const noexcept;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Section delimeter string */
  static const std::string SECTION_DELIM;

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
   * Reads the grammar file and populates this object from the contents
   * of said file.
   */
  void populateGrammar();

  /**
   * Reads a line from the grammar file. Removes comments and empty lines
   * as it reads.
   *
   * @param theInputLine
   *          string populated with the next line
   * @return always true
   */
  bool readLine(std::string &theInputLine) noexcept;

  /**
   * Reads the productions from the grammar file
   */
  void readProductions();

  /**
   * Reads in the scanner table from the grammar file.
   */
  void readScannerTable();

  /**
   * Reads the start symbol
   */
  void readStartSymbol();

  /**
   * Reads the terminal definitions from the grammar file
   */
  void readTerminals();

  /** Current file line number */
  uint32_t myLineNumber = 0;

  /*
   * Meta elements: input file, EW Tracker
   */
  /** Error/Warning tracker */
  ErrorWarningTracker &myEWTracker;

  /** Name of file containing the grammar. */
  std::string myFileName;

  /** File stream for reading grammar definition file. */
  std::ifstream myFile;

  /** Scanner table */
  ScannerTable &myScannerTable;

  /*
   * Grammar elements: Symbols (terminal & non-terminal), and productions.
   */
  /** Set of all non-terminal symbols in the productions. */
  Symbol::SymbolSet myNonTerminalSymbols;

  /** Grammar start symbol. */
  std::shared_ptr<Symbol> myStartSymbol;

  /** Set of all terminal symbols in the productions. */
  Symbol::SymbolSet myTerminalSymbols;

  /** All productions */
  std::vector<std::shared_ptr<Production>> myProductions;
};

#endif
