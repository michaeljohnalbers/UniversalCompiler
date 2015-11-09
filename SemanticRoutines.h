#ifndef SEMANTICROUTINES_H
#define SEMANTICROUTINES_H

/**
 * @file SemanticRoutines.h
 * @brief Defines the semantic routines for code generation.
 *
 * @author Michael Albers
 */

#include <cstdint>
#include <fstream>
#include <map>
#include <memory>
#include <vector>

#include "SemanticRecord.h"

class ErrorWarningTracker;
class SemanticStack;
class Symbol;
class SymbolTable;

/**
 * All semantic routines are housed within this class, which interfaces
 * closely with the semantic routines to retrieve and modify data during
 * parsing.
 */
class SemanticRoutines
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  SemanticRoutines() = delete;

  /**
   * Copy constructor.
   */
  SemanticRoutines(const SemanticRoutines&) = default;

  /**
   * Move constructor.
   */
  SemanticRoutines(SemanticRoutines &&) = default;

  /**
   * Constructor.
   *
   * @param theGeneratedCodeFileName
   *          file in which to write generated code
   * @param theSemanticStack
   *          semantic stack
   * @param theSymbolTable
   *          symbol table
   * @param theEWTracker
   *          error/warning tracker
   * @throws std::runtime_error
   *           on error opening generated code file
   */
  SemanticRoutines(const std::string &theGeneratedCodeFileName,
                   SemanticStack &theSemanticStack,
                   SymbolTable &theSymbolTable,
                   ErrorWarningTracker &theEWTracker);

  /**
   * Destructor
   */
  virtual ~SemanticRoutines() = default;

  /**
   * Copy assignment operator.
   */
  SemanticRoutines& operator=(const SemanticRoutines&) = default;

  /**
   * Move assignment operator.
   */
  SemanticRoutines& operator=(SemanticRoutines&&) = default;

  /**
   * Executes the semantic routine from the given action symbol.
   *
   * @param theActionSymbol
   *          action symbol from the grammar.
   */
  void executeSemanticRoutine(const std::shared_ptr<Symbol> theActionSymbol);

  /**
   * Returns all generated code.
   *
   * @return generated code
   */
  std::vector<std::string> getCode() const noexcept;

  /**
   * Returns all symbols in the symbol table.
   *
   * @return symbols sorted by scope level
   */
  std::vector<std::string> getSymbols() const noexcept;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /**
   * Semantic routine functions.
   */
  using SemanticRoutine = std::function<void(SemanticRoutines*,
                                             std::vector<std::string>&)>;

  /**
   * All action symbol semantic routines.
   *
   * @param Arguments to action symbol.
   */
#define ACTION_SYMBOL_ROUTINE(x) void x(std::vector<std::string>&) noexcept
  ACTION_SYMBOL_ROUTINE(assign);
  ACTION_SYMBOL_ROUTINE(copy);
  ACTION_SYMBOL_ROUTINE(createScope);
  ACTION_SYMBOL_ROUTINE(destroyScope);
  ACTION_SYMBOL_ROUTINE(genInfix);
  ACTION_SYMBOL_ROUTINE(finish);
  ACTION_SYMBOL_ROUTINE(processId);
  ACTION_SYMBOL_ROUTINE(processLiteral);
  ACTION_SYMBOL_ROUTINE(processOp);
  ACTION_SYMBOL_ROUTINE(readId);
  ACTION_SYMBOL_ROUTINE(start);
  ACTION_SYMBOL_ROUTINE(writeExpr);
#undef ACTION_SYMBOL_ROUTINE

  /**
   * Writes the instruction to the generated code file.
   *
   * @param theInstruction
   *          instruction to write
   */
  void generate(const std::string &theInstruction) noexcept;

  /**
   * Writes the instruction and argument to the generated code file.
   *
   * @param theInstruction
   *          instruction to write
   * @param theFirst
   *          first argument to the instruction.
   */
  void generate(const std::string &theInstruction,
                const std::string &theFirst) noexcept;

  /**
   * Writes the instruction and arguments to the generated code file.
   *
   * @param theInstruction
   *          instruction to write
   * @param theFirst
   *          first argument to the instruction.
   * @param theSecond
   *          second argument to the instruction.
   */
  void generate(const std::string &theInstruction,
                const std::string &theFirst,
                const std::string &theSecond) noexcept;

  /**
   * Writes the instruction and arguments to the generated code file.
   *
   * @param theInstruction
   *          instruction to write
   * @param theFirst
   *          first argument to the instruction.
   * @param theSecond
   *          second argument to the instruction.
   * @param theThird
   *          third argument to the instruction.
   */
  void generate(const std::string &theInstruction,
                const std::string &theFirst,
                const std::string &theSecond,
                const std::string &theThird) noexcept;

  /**
   * Returns the operand string for an instruction. Adds an 'Addr' call
   * for variables.
   *
   * @param theOperand
   *          operand
   * @return operand with possible 'Addr' call.
   */
  static std::string getOperand(std::string theOperand) noexcept;

  /**
   * Get a new temporary variable.
   *
   * @return temporary variable
   */
  SemanticRecord getTemp() noexcept;

  /**
   * Returns tuple prefix code ("(x)")
   *
   * @return tuple prefix code
   */
  std::string getTupleCode() noexcept;

  /** Error/Warning tracker */
  ErrorWarningTracker &myEWTracker;

  /** Each generated instruction, for printing code generation steps. */
  std::vector<std::string> myGeneratedCode;

  /** File for generated code. */
  std::ofstream myGeneratedCodeFile;

  /** Name of file for generated code. */
  const std::string &myGeneratedCodeFileName;

  /** Temporary variable id */
  uint32_t myNextTemp = 0;

  /** Map of action symbol name to actual semantic routine. */
  std::map<std::string, SemanticRoutine> mySemanticRoutines;

  /** Semantic stack. */
  SemanticStack &mySemanticStack;

  /** Symbol table. */
  SymbolTable &mySymbolTable;

  /** Tuple number */
  uint32_t myTupleNumber = 0;
};

#endif
