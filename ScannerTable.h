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
#include <ostream>
#include <string>
#include <vector>

/**
 * Table used to drive the scanning. Used to work through the regular
 * expressions defining the tokens and the actions to take for each state.
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
    /** Return token code, consume character, append to next token. */
    HaltAppend,
    /** Return token code, consume character, do not append. */
    HaltNoAppend,
    /** Return token code, consume character, save for reuse in next token. */
    HaltReuse,
  };

  /** State number */
  using State = uint32_t;

  /** Token Id */
  using TokenId = uint32_t;

  /**
   * Single entry in table.
   *
   * If myAction is one of the Move* values, then use myNextState. No token
   * has been found yet.
   *
   * If myAction is one of the Halt* values, then use myTokenCode. A full token
   * has been found.
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
          TokenId theTokenId);

    Entry(const Entry &) = default;
    Entry(Entry &&) = default;
    ~Entry() = default;

    Entry& operator=(const Entry &) = default;
    Entry& operator=(Entry &&) = default;

    friend std::ostream& operator<<(std::ostream &theOS, const Entry &theEntry);

    State myNextState;
    Action myAction;
    TokenId myTokenId;
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
   * Informs the table of a new reserved word.
   *
   * @param theTokenId
   *          ID of the token to add
   * @param theReservedWord
   *          reserved word
   */
  void addReservedWord(TokenId theTokenId, std::string theReservedWord)
    noexcept;

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
   * Informs the table of a new token definition.
   *
   * @param theTokenId
   *          ID of the token to add
   * @param theToken
   *          token name
   */
  void addToken(TokenId theTokenId, const std::string &theToken) noexcept;

  /**
   * Returns a new token code if the given token is a reserved word.
   *
   * @param theTokenId
   *          Id of current token
   * @param theToken
   *          actual token
   * @return possibly updated token code
   */
  TokenId checkExceptions(TokenId theTokenId, std::string theToken)
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
   * Returns the name of the given token Id
   *
   * @param theTokenId
   *          token id
   * @return token name
   */
  std::string getTokenName(TokenId theTokenId) const noexcept;

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
   * Returns the token Id for the given state/character combination.
   *
   * @param theCurrentState
   *          current state
   * @param theCharacter
   *          current character
   * @return token id
   * @throws std::invalid_argument on invalid state
   */
  TokenId lookupCode(State theCurrentState, char theCharacter) const;

  /** Starting state */
  static constexpr uint32_t START_STATE = 0;

  /** Built-in token id for whitespace */
  static constexpr TokenId NO_TOKEN = 98;

  /** End of file symbol. */
  static constexpr TokenId EOF_SYMBOL = 99;

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

  /** Reserved words. Token number, reserved word*/
  std::map<TokenId, std::string> myReservedWords;

  /** Scanner driver table */
  std::vector<std::vector<Entry>> myTable;

  /** Tokens from the grammar file. Token number, token name*/
  std::map<TokenId, std::string> myTokens;
};

#endif
