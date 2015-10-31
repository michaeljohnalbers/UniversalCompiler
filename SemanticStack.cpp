/**
 * @file SemanticStack.cpp
 * @brief Implementation of SemanticStack class
 *
 * @author Michael Albers
 */

#include <algorithm>
#include <cctype>
#include <vector>

#include "EOPSymbol.h"
#include "SemanticStack.h"

//*******************************************************
// SemanticStack::SemanticStack
//*******************************************************
SemanticStack::SemanticStack()
{
}

//*******************************************************
// SemanticStack::expand
//*******************************************************
void SemanticStack::expand(uint32_t theNumberElementsToAdd) noexcept
{
  mySemanticStack.resize(mySemanticStack.size() + theNumberElementsToAdd);
  myLeftIndex = myCurrentIndex;
  myRightIndex = myTopIndex;
  myCurrentIndex = myRightIndex;
  myTopIndex += theNumberElementsToAdd;
}

//*******************************************************
// SemanticStack::getEOPSymbol
//*******************************************************
std::shared_ptr<Symbol> SemanticStack::getEOPSymbol() const noexcept
{
  return std::shared_ptr<Symbol>(new EOPSymbol(myCurrentIndex,
                                               myLeftIndex,
                                               myRightIndex,
                                               myTopIndex));
}

//*******************************************************
// SemanticStack::getIndex
//*******************************************************
uint32_t SemanticStack::getIndex(std::string theArgument) const noexcept
{
  uint32_t index = 0;
  theArgument.erase(theArgument.begin());
  if ('$' == theArgument[0])
  {
    // $$
    index = myLeftIndex;
  }
  else
  {
    // $1, $2, etc. $1 == right index, hence the -1
    index = myRightIndex + std::stoul(theArgument) - 1;
  }
  return index;
}

//*******************************************************
// SemanticStack::getRecordAtCurrentIndexMinusOne
//*******************************************************
SemanticRecord& SemanticStack::getRecordAtCurrentIndexMinusOne() noexcept
{
  return mySemanticStack.at(myCurrentIndex-1);
}

//*******************************************************
// SemanticStack::getRecordFromArgument
//*******************************************************
SemanticRecord& SemanticStack::getRecordFromArgument(std::string theArgument)
  noexcept
{
  auto stackIndex = getIndex(theArgument);
  return mySemanticStack.at(stackIndex);
}

//*******************************************************
// SemanticStack::getStack
//*******************************************************
std::vector<SemanticRecord> SemanticStack::getStack() const noexcept
{
  return mySemanticStack;
}

//*******************************************************
// SemanticStack::initialize
//*******************************************************
void SemanticStack::initialize() noexcept
{
  mySemanticStack.clear();
  // This does two things. First, it creates a dummy element at the bottom of
  // the stack. The pseudo-code from class appears to have a one-based stack
  // this dummy element makes this zero-based stack one-based. The second
  // element is equivalent to pushing the start symbol onto the stack.
  mySemanticStack.resize(2);
  myLeftIndex = 0;
  myRightIndex = 0;
  myCurrentIndex = 1;
  myTopIndex = 2;
}

//*******************************************************
// SemanticStack::replaceAtCurrentIndex
//*******************************************************
void SemanticStack::replaceAtCurrentIndex(const SemanticRecord &theNewRecord)
  noexcept
{
  mySemanticStack[myCurrentIndex] = theNewRecord;
  ++myCurrentIndex;
}

//*******************************************************
// SemanticStack::restore
//*******************************************************
void SemanticStack::restore(std::shared_ptr<Symbol> theEOPSymbol)
  noexcept
{
  dynamic_cast<EOPSymbol*>(theEOPSymbol.get())->getValues(myCurrentIndex,
                                                          myLeftIndex,
                                                          myRightIndex,
                                                          myTopIndex);
  // Need to add one as stack is 1-based. (So if myTopIndex == 12,
  // mySemanticStack[12] must be the first free item. And mySemanticStack[12]
  // is actually the 13th element.).
  mySemanticStack.resize(myTopIndex+1);
  ++myCurrentIndex;
}
