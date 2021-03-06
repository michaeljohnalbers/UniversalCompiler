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
#include <deque>

#include "ScannerTable.h"
#include "Token.h"

class ErrorWarningTracker;
class ScannerTable;

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
   * @param theScannerTable
   *          table which drives the scan
   * @param theEWTracker
   *          error/warning tracker
   * @param thePrintTokens
   *          if true, tokens will be printed as they are scanned
   * @throw std::runtime_error
   *          on error opening input file
   */
  Scanner(const std::string &theFile,
          ScannerTable &theScannerTable,
          ErrorWarningTracker &theEWTracker,
          bool thePrintTokens);

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
   * Returns the tokens which have not yet been scanned.
   *
   * @return list of tokens
   */
  std::deque<Token> getRemainingTokens() const noexcept;

  /**
   * Consumes and returns the next token in the source file.
   *
   * @return scanned token
   */
  Token scan();

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

  /**
   * Get the next token from the file.
   *
   * @return next token
   */
  Token getToken();

  /** Current column being read. */
  uint32_t myColumn = 1;

  /** Error/Warning tracker */
  ErrorWarningTracker &myEWTracker;

  /** Input file name */
  std::string myFile;

  /** File input */
  std::ifstream myInputStream;

  /** Current line number. */
  uint32_t myLine = 1;

  /** Should tokens be printed as they are scanned? */
  const bool myPrintTokens;

  /** Scanner driver table */
  ScannerTable &myScannerTable;

  /** All tokens from the file. */
  std::deque<Token> myTokens;
};

#endif
