#ifndef DRIVERTABLE_H
#define DRIVERTABLE_H

/**
 * @file DriverTable.h
 * @brief Defines the class used to drive the scanning of an input file
 *
 * @author Michael Albers
 */

#include <cstdint>
#include <map>
#include <ostream>
#include <string>

/**
 * Table used to drive the scanning. Used to work through the regular
 * expressions defining the tokens and the actions to take for each state.
 */
class DriverTable
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

  /** Starting state */
  static constexpr State START_STATE = 0;

  /** Token codes */
  enum class TokenCode
  {
    NoToken, // Comment, space, error, etc.
    BeginSym,
    EndSym,
    ReadSym,
    WriteSym,
    Id,
    IntLiteral,
    LParen,
    RParen,
    SemiColon,
    Comma,
    AssignOp,
    PlusOp,
    MinusOp,
    EqualOp,
    EofSym,
  };

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
    /** Sets entry to error */
    Entry();
    /** Used for Move* action*/
    Entry(State theNextState, Action theAction);
    /** Used for Halt* action */
    Entry(Action theAction, TokenCode theTokenCode);
    /** Full parameterized constructor */
    Entry(State theNextState, Action theAction, TokenCode theTokenCode);
    Entry(const Entry &) = default;
    Entry(Entry &&) = default;
    ~Entry() = default;

    Entry& operator=(const Entry &) = default;
    Entry& operator=(Entry &&) = default;

    friend std::ostream& operator<<(std::ostream &theOS, const Entry &theEntry);

    State myNextState;
    Action myAction;
    TokenCode myTokenCode;
  };

  /**
   * Default constructor.
   */
  DriverTable();

  /**
   * Copy constructor
   */
  DriverTable(const DriverTable &) = default;

  /**
   * Move constructor
   */
  DriverTable(DriverTable &&) = default;

  /**
   * Destructor
   */
  ~DriverTable() = default;

  /**
   * Copy assignment operator
   */
  DriverTable& operator=(const DriverTable &) = default;

  /**
   * Move assignment operator
   */
  DriverTable& operator=(DriverTable &&) = default;

  /**
   * Returns a new token code if the given token is a reserved word.
   *
   * @param theTokenCode
   *          code of current token
   * @param theToken
   *          actual token
   * @return possibly updated token code
   */
  TokenCode checkExceptions(TokenCode theTokenCode, const std::string &theToken)
    const;

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
   * Returns a string representation of the given token
   *
   * @param theTokenCode
   *          token code for which to get string representation
   * @return string reprsentation of the token
   */
  static std::string getDescription(TokenCode theTokenCode) noexcept;

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
   * Returns the token code for the given state/character combination.
   *
   * @param theCurrentState
   *          current state
   * @param theCharacter
   *          current character
   * @return token code
   * @throws std::invalid_argument on invalid state
   */
  TokenCode lookupCode(State theCurrentState, char theCharacter) const;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  /** Maximum legal state number. */
  static constexpr uint32_t MAX_STATE = 6;

  /** Number of character classes needed. */
  static constexpr uint32_t NUMBER_CHARACTER_CLASSES = 15;

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

  /**
   * Populates myTable
   */
  void populateTable() noexcept;

  /**
   * Populates individual states in the table
   */
  void populateState0() noexcept;
  void populateState1() noexcept;
  void populateState2() noexcept;
  void populateState3() noexcept;
  void populateState4() noexcept;
  void populateState5() noexcept;
  void populateState6() noexcept;

  using TokenMap = std::map<TokenCode, std::string>;

  /** Token-to-string mapping*/
  static TokenMap ourTokenDescriptions;

  /** Driver table. */
  Entry myTable[MAX_STATE+1][NUMBER_CHARACTER_CLASSES];

};

#endif
