#ifndef SCANNERTABLE_H
#define SCANNERTABLE_H

/**
 * @file ScannerTable.h
 * @brief Defines the class used to drive the scanning of an input file
 *
 * @author Michael Albers
 */

#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "Symbol.h"
#include "TerminalSymbol.h"

/**
 * Table used to drive the scanning. Used to work through the regular
 * expressions defining the terminals and the actions to take for each state.
 */
class ScannerTable
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Actions for the scanner to take.
   */
  enum class Action
  {
    /** Invalid move */
    Error,
    /** Move to next state, append current character to current token. */
    MoveAppend,
    /** Move to next state, do not append current character, consume it. */
    MoveNoAppend,
    /** Return terminal code, consume character, append to next token. */
    HaltAppend,
    /** Return terminal code, consume character, do not append. */
    HaltNoAppend,
    /** Return terminal code, consume character, save for use in next token. */
    HaltReuse,
  };

  /** State number */
  using State = uint32_t;

  /**
   * Single entry in table.
   *
   * If myAction is one of the Move* values, then use myNextState. No token
   * has been found yet.
   *
   * If myAction is one of the Halt* values, then use myTerminalCode. A full
   * token has been found.
   */
  class Entry
  {
    public:

    /**
     * Default constructor, sets entry to error
     */
    Entry();

    /**
     * Constructor, converts action acronym to an Action.
     */
    Entry(State theNextState, const std::string &theActionAcronym,
          TerminalSymbol::Id theTerminalId);

    Entry(const Entry &) = default;
    Entry(Entry &&) = default;
    ~Entry() = default;

    Entry& operator=(const Entry &) = default;
    Entry& operator=(Entry &&) = default;

    friend std::ostream& operator<<(std::ostream &theOS, const Entry &theEntry);

    State myNextState;
    Action myAction;
    TerminalSymbol::Id myTerminalId;
  };

  /**
   * Default constructor.
   */
  ScannerTable();

  /**
   * Copy constructor
   */
  ScannerTable(const ScannerTable &) = default;

  /**
   * Move constructor
   */
  ScannerTable(ScannerTable &&) = default;

  /**
   * Destructor
   */
  ~ScannerTable() = default;

  /**
   * Copy assignment operator
   */
  ScannerTable& operator=(const ScannerTable &) = default;

  /**
   * Move assignment operator
   */
  ScannerTable& operator=(ScannerTable &&) = default;

  /**
   * Adds a new column to the end of the current column set. Each column
   * defines a character class.
   *
   * @param theCharacterClass
   *          character class
   */
  void addColumn(const std::string &theCharacterClass) noexcept;

  /**
   * Add the given entry to the driver table at the given location. If the
   * state doesn't exist yet, the table will be resized to create it. The column
   * number must be within the current number of columns (see addColumn).
   *
   * @param theState
   *          state value (i.e., table row number), zero-based
   * @param theColumn
   *          column number, zero-based
   * @param theEntry
   *          driver table entry
   */
  void addTableEntry(State theState, uint32_t theColumn, const Entry &theEntry)
    noexcept;

  /**
   * Informs the table of a new terminal definition.
   *
   * @param theTerminal
   *          terminal data
   */
  void addTerminal(std::shared_ptr<Symbol> theTerminal) noexcept;

  /**
   * Returns a new terminal code if the given terminal is a reserved word.
   *
   * @param theTerminalId
   *          Id of current terminal
   * @param theTokenString
   *          actual token string from source
   * @return possibly updated terminal code
   */
  TerminalSymbol::Id checkExceptions(TerminalSymbol::Id theTerminalId,
                                     std::string theTokenString)
    const noexcept;

  /**
   * Returns the action for the given current state/character inputs.
   *
   * @param theCurrentState
   *          current state
   * @param theCharacter
   *          current character
   * @return action to take based on inputs
   * @throws std::invalid_argument on invalid state
   */
  Action getAction(State theCurrentState, char theCharacter) const;

  /**
   * Returns the special EOF symbol.
   *
   * @return EOF symbol
   */
  std::shared_ptr<Symbol> getEOF() const noexcept;

  /**
   * Returns the next state given the current state/character inputs.
   *
   * @param theCurrentState
   *          current state
   * @param theCharacter
   *          current character
   * @return state value
   * @throws std::invalid_argument on invalid state
   */
  State getState(State theCurrentState, char theCharacter) const;

  /**
   * Returns the terminal for the given state/character/token combination.
   *
   * @param theCurrentState
   *          current state
   * @param theCharacter
   *          current character
   * @param theTokenString
   *          token from source file, used for reserved word checking
   * @return terminal id
   * @throws std::invalid_argument on invalid state
   */
  std::shared_ptr<Symbol> lookupTerminal(State theCurrentState,
                                         char theCharacter,
                                         std::string theTokenString) const;

  /** Starting state */
  static constexpr uint32_t START_STATE = 0;

  /** Built-in terminal id for whitespace */
  static constexpr TerminalSymbol::Id NO_TERMINAL = 98;

  /** End of file symbol. */
  static constexpr TerminalSymbol::Id EOF_SYMBOL = 99;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  /**
   * Returns the table column number based on the given character.
   *
   * @param theCharacter
   *          character which determines the column number
   * @return column number suitable for direct array/vector indexing
   */
  uint32_t getColumn(char theCharacter) const noexcept;

  /**
   * Validates the given state.
   *
   * @param theState
   *          state to validate
   * @throws std::invalid_argument
   *           on invalid state
   */
  void validateState(State theState) const;

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Table columns character classes*/
  std::vector<std::string> myColumnCharacterClasses;

  /** Reserved words. Reserved word, terminal info*/
  std::map<std::string, TerminalSymbol::Id> myReservedWords;

  /** Scanner driver table */
  std::vector<std::vector<Entry>> myTable;

  /** Terminals from the grammar file. */
  Symbol::SymbolSet myTerminals;

  /** Map of terminal Ids to its terminal. */
  std::map<TerminalSymbol::Id, std::shared_ptr<Symbol>> myTerminalIdMap;
};

#endif
