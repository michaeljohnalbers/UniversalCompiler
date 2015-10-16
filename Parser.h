#ifndef PARSER_H
#define PARSER_H

/**
 * @file Parser.h
 * @brief Defines the class used to parse a given token stream.
 *
 * @author Michael Albers
 */

#include <memory>
#include <stack>

class Grammar;
class ErrorWarningTracker;
class PredictTable;
class Scanner;
class Symbol;
class Token;

/**
 * Class which defines a universal LL(1) parser.
 */
class Parser
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  Parser() = delete;

  /**
   * Copy constructor
   */
  Parser(const Parser &) = default;

  /**
   * Move constructor
   */
  Parser(Parser &&) = default;

  /**
   * constructor
   *
   * @param theScanner
   *          scanner object
   * @param theGrammar
   *          language grammar
   * @param thePredictTable
   *          LL(1) predict table
   * @param theEWTracker
   *          error/warning tracker
   * @param thePrintParse
   *          print parse steps
   */
  Parser(Scanner &theScanner,
         const Grammar &theGrammar,
         const PredictTable &thePredictTable,
         ErrorWarningTracker &theEWTracker,
         bool thePrintParse);

  /**
   * Destructor
   */
  ~Parser() = default;

  /**
   * Copy assignment operator
   */
  Parser& operator=(const Parser &) = default;

  /**
   * Move assignment operator
   */
  Parser& operator=(Parser &&) = default;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /**
   * Parses the tokens from the scanner according to the grammar.
   */
  void parse();

  /**
   * Add the stack contents to the given stream.
   *
   * @param theOS
   *          stream to add stack data to
   * @param theStack
   *          stack to print
   */
  static void printStack(std::ostream &theOS,
                         std::stack<std::shared_ptr<Symbol>> theStack);

  /**
   * Prints all tokens remaining in the scanner.
   *
   * @param theOS
   *          stream to print tokens to
   * @param theLookAheadToken
   *          Look-ahead token
   */
  void printTokens(std::ostream &theOS, const Token &theLookAheadToken);

  /** Error/Warning tracker */
  ErrorWarningTracker &myEWTracker;

  /** Language grammar */
  const Grammar &myGrammar;

  /** Production predict table */
  const PredictTable &myPredictTable;

  /** Print parse steps */
  const bool myPrintParse;

  /** Token scanner */
  Scanner &myScanner;

  /** Stack of expected symbols during parsing */
  std::stack<std::shared_ptr<Symbol>> myStack;
};

#endif
