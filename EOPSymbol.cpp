/**
 * @file EOPSymbol.cpp
 * @brief Implementation of EOPSymobl class
 *
 * @author Michael Albers
 */

#include <sstream>

#include "EOPSymbol.h"

//*******************************************************
// EOPSymbol::EOPSymbol
//*******************************************************
EOPSymbol::EOPSymbol(uint32_t theCurrentIndex, uint32_t theLeftIndex,
                     uint32_t theRightIndex, uint32_t theTopIndex) :
  Symbol("EOPSymbol"),
  myCurrentIndex(theCurrentIndex),
  myLeftIndex(theLeftIndex),
  myRightIndex(theRightIndex),
  myTopIndex(theTopIndex)
{
}

//*******************************************************
// EOPSymbol::getDerivesLambda
//*******************************************************
bool EOPSymbol::getDerivesLambda() const noexcept
{
  return false;
}

//*******************************************************
// EOPSymbol::getValues
//*******************************************************
void EOPSymbol::getValues(uint32_t &theCurrentIndex, uint32_t &theLeftIndex,
                          uint32_t &theRightIndex, uint32_t &theTopIndex)
  const noexcept
{
  theCurrentIndex = myCurrentIndex;
  theLeftIndex = myLeftIndex;
  theRightIndex = myRightIndex;
  theTopIndex = myTopIndex;
}

//*******************************************************
// EOPSymbol::print
//*******************************************************
void EOPSymbol::print(std::ostream &theOS) const noexcept
{
  // If this were inserted directly into theOS, the setw in Parser.cpp would
  // get applied to the name (EOPSymbol) instead of the whole string
  // (EOPSymbol(...)).
  std::ostringstream symbol;
  symbol << getName() << "(" << myLeftIndex << "," << myRightIndex << ","
         << myCurrentIndex << "," << myTopIndex << ")";
  theOS << symbol.str();
}
