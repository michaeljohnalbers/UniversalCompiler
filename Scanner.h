#ifndef SCANNER_H
#define SCANNER_H

/**
 * @file Scanner.h
 * @brief Defines the class used to scan input for tokens
 *
 * @author Michael Albers
 */

#include <cstdint>
#include <fstream>
#include <string>

#include "DriverTable.h"

class ErrorWarningTracker;

/**
 * Scans an input file returning all of the tokens contained therein.
 */
class Scanner
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  Scanner() = delete;

  /**
   * Copy constructor
   */
  Scanner(const Scanner &) = default;

  /**
   * Move constructor
   */
  Scanner(Scanner &&) = default;

  /**
   * constructor
   *
   * @param theFile
   *          file to scan/tokenize
   * @throw std::runtime_error
   *          on error opening input file
   */
  Scanner(const std::string &theFile,
          ErrorWarningTracker &theEWTracker);

  /**
   * Destructor
   */
  ~Scanner() = default;

  /**
   * Copy assignment operator
   */
  Scanner& operator=(const Scanner &) = default;

  /**
   * Move assignment operator
   */
  Scanner& operator=(Scanner &&) = default;

  /**
   * Returns the current column of the scan (this will be the column
   * immediately after the last token pulled from the 'scan' function).
   *
   * @return current column number
   */
  uint32_t getColumn() const noexcept;

  /**
   * Returns the current line of the scan.
   *
   * @return current line number
   */
  uint32_t getLine() const noexcept;

  /**
   * Returns the next token in the file.
   *
   * @param theToken
   *          OUT parameter, next token
   * @param theTokenId
   *          OUT parameter, ID of next token.
   */
  void scan(std::string &theToken, DriverTable::TokenCode &theTokenId);

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  /**
   * Consumes the current character.
   */
  void consumeChar();

  /**
   * Returns current character being processed. Does not extract
   * it from the input.
   *
   * @return current character
   */
  char currentChar();

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Current column being read. */
  uint32_t myColumn = 0;

  /** Table driving the scan. */
  DriverTable myDriverTable;

  /** Error/Warning tracker */
  ErrorWarningTracker &myEWTracker;

  /** Input file name */
  std::string myFile;

  /** File input */
  std::ifstream myInputStream;

  /** Current line number. */
  uint32_t myLine = 1;
};

#endif
