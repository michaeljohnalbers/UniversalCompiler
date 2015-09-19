#ifndef ERRORWARNINGTRACKER_H
#define ERRORWARNINGTRACKER_H

/**
 * @file ErrorWarningTracker.h
 * @brief Defines the class used to print and track compile errors and warnings.
 *
 * @author Michael Albers
 */

#include <string>

/**
 * This class handles compiler errors and warnings. It should be used to report
 * all errors and warnings encountered during compilation.
 */
class ErrorWarningTracker
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  ErrorWarningTracker() = delete;

  /**
   * Copy constructor
   */
  ErrorWarningTracker(const ErrorWarningTracker &) = default;

  /**
   * Move constructor
   */
  ErrorWarningTracker(ErrorWarningTracker &&) = default;

  /**
   * Constructor.
   */
  ErrorWarningTracker(const std::string &theFile);

  /**
   * Destructor
   */
  ~ErrorWarningTracker() = default;

  /**
   * Copy assignment operator
   */
  ErrorWarningTracker& operator=(const ErrorWarningTracker &) = default;

  /**
   * Move assignment operator
   */
  ErrorWarningTracker& operator=(ErrorWarningTracker &&) = default;

  /**
   * Returns if the file has an error.
   *
   * @return true if an error was reported, false otherwise
   */
  bool hasError() const noexcept;

  /**
   * Reports a syntax error.
   *
   * @param theError
   *          error to report
   */
  void reportError(const std::string &theError) noexcept;

  /**
   * Reports a syntax error at the given location.
   *
   * @param theLine
   *          line in the file where the error was detected
   * @param theColumn
   *          column in the file where the error was detected
   * @param theError
   *          error to report
   */
  void reportError(uint32_t theLine, uint32_t theColumn,
                   const std::string &theError) noexcept;

  /**
   * Reports a warning.
   *
   * @param theWarning
   *          warngin to report
   */
  void reportWarning(const std::string &theWarning) noexcept;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** File being compiled. */
  const std::string myFile;

  /** Does the program have an error? */
  bool myHasError = false;
};

#endif
