/**
 * @file ErrorWarningTracker.cpp
 * @brief Implementation of ErrorWarningTracker class
 *
 * @author Michael Albers
 */

#include <iostream>

#include "ErrorWarningTracker.h"

//*******************************************************
// ErrorWarningTracker::ErrorWarningTracker
//*******************************************************
ErrorWarningTracker::ErrorWarningTracker(const std::string &theFile) :
  myFile(theFile)
{
}

//*******************************************************
// ErrorWarningTracker::hasError
//*******************************************************
bool ErrorWarningTracker::hasError() const noexcept
{
  return myHasError;
}

//*******************************************************
// ErrorWarningTracker::reportError
//*******************************************************
void ErrorWarningTracker::reportError(const std::string &theError)
  noexcept
{
  myHasError = true;

  std::cerr << myFile << ": error: " << theError << std::endl;
}

//*******************************************************
// ErrorWarningTracker::reportError
//*******************************************************
void ErrorWarningTracker::reportError(uint32_t theLine, uint32_t theColumn,
                                      const std::string &theError) noexcept
{
  myHasError = true;

  std::cerr << myFile << ":" << theLine << ":" << theColumn << ": error: "
            << theError << std::endl;
}

//*******************************************************
// ErrorWarningTracker::reportWarning
//*******************************************************
void ErrorWarningTracker::reportWarning(const std::string &theWarning)
  noexcept
{
  std::cerr << myFile << ": warning: " << theWarning << std::endl;
}
