/**
 * @file SymbolTable.cpp
 * @brief Implementation of SymbolTable class
 *
 * @author Michael Albers
 */

#include <exception>
#include <cstring>
#include <cctype>

#include "SymbolTable.h"

//*******************************************************
// SymbolTable::~SymbolTable
//*******************************************************
SymbolTable::SymbolTable()
{
  myHashTable.fill(nullptr);
}

//*******************************************************
// SymbolTable::~SymbolTable
//*******************************************************
SymbolTable::~SymbolTable()
{
  for (HashNode *tableEntry : myHashTable)
  {
    if (tableEntry != nullptr)
    {
      delete tableEntry;
    }
  }
}

//*******************************************************
// SymbolTable::add
//*******************************************************
bool SymbolTable::add(const std::string &theIdentifier,
                      SymbolAttributes &theAttributes) noexcept
{
  auto hashValue = hash(theIdentifier);
  bool found = find(theIdentifier, theAttributes, hashValue, myScopeLevel);

  // This doesn't work if the identifier is over SEGMENT_SIZE_BYTES characters,
  // but that constant is set high enough that for all practical purposes it
  // won't happen.
  if (! found)
  {
    uint32_t index = 0;

    if (nullptr == myHashTable[hashValue])
    {
      myHashTable[hashValue] = new HashNode();
    }
    else
    {
      index = myHashTable[hashValue]->myIndex +
        myHashTable[hashValue]->myLength;;

      HashNode *node = new HashNode();
      node->myNextNode = myHashTable[hashValue];
      myHashTable[hashValue] = node;
    }

    HashNode *newNode = myHashTable[hashValue];

    auto &stringSpace = myStringSpaces[hashValue];
    uint32_t segmentNumber, offset;

    getSegmentNumberAndOffset(index, segmentNumber, offset);

    if (offset + theIdentifier.size() > SEGMENT_SIZE_BYTES)
    {
      index += SEGMENT_SIZE_BYTES - offset;
      getSegmentNumberAndOffset(index, segmentNumber, offset);
    }

    // segmentNumber will be 0-based, while size is 1-based
    if (segmentNumber >= stringSpace.size() || stringSpace.size() == 0)
    {
      stringSpace.resize(segmentNumber+1);
    }

    auto &segment = stringSpace.at(segmentNumber);
    std::strncpy(&segment[offset], theIdentifier.c_str(), theIdentifier.size());

    newNode->myLength = theIdentifier.size();
    newNode->myIndex = index;
    newNode->myScopeLevel = myScopeLevel;
    theAttributes = newNode->myAttributes;
  }

  return found;
}

//*******************************************************
// SymbolTable::createNewScope
//*******************************************************
void SymbolTable::createNewScope() noexcept
{
  myScopeLevel++;
}

//*******************************************************
// SymbolTable::destroyCurrentScope
//*******************************************************
void SymbolTable::destroyCurrentScope()
{
  if (MIN_SCOPE_LEVEL == myScopeLevel)
  {
    throw std::underflow_error{"Cannot reduce scope any further, "
        "already at minimum level."};
  }

  myScopeLevel--;

  for (uint32_t ii = 0; ii < TABLE_SIZE; ++ii)
  {
    if (myHashTable[ii] != nullptr &&
        myHashTable[ii]->myScopeLevel > myScopeLevel)
    {
      HashNode *scopeStart = myHashTable[ii];
      HashNode *scopeEnd = scopeStart;
      while (scopeEnd->myNextNode != nullptr &&
             scopeEnd->myNextNode->myScopeLevel > myScopeLevel)
      {
        scopeEnd = scopeEnd->myNextNode;
      }

      myHashTable[ii] = scopeEnd->myNextNode;
      scopeEnd->myNextNode = nullptr;
      delete scopeStart; // See ~HashNode, chain deletes until scopeEnd
    }
  }
}

//*******************************************************
// SymbolTable::find
//*******************************************************
bool SymbolTable::find(const std::string &theIdentifier,
                       SymbolAttributes &theAttributes)
  const noexcept
{
  auto hashValue = hash(theIdentifier);
  bool found = find(theIdentifier, theAttributes, hashValue, MIN_SCOPE_LEVEL);
  return found;
}

//*******************************************************
// SymbolTable::find
//*******************************************************
bool SymbolTable::find(const std::string &theIdentifier,
                       SymbolAttributes &theAttributes,
                       uint32_t theHashValue,
                       uint32_t theScopeLevel) const noexcept
{
  bool found = false;

  HashNode *node = myHashTable[theHashValue];
  while (node != nullptr)
  {
    if (node->myScopeLevel >= theScopeLevel &&
        node->myLength == theIdentifier.size())
    {
      const char *symbol = getString(theHashValue, node->myIndex);
      if (::strncasecmp(symbol, theIdentifier.c_str(),
                        node->myLength) == 0)
      {
        theAttributes = node->myAttributes;
        found = true;
        break;
      }
    }
    node = node->myNextNode;
  }
  return found;
}

//*******************************************************
// SymbolTable::getAllSymbols
//*******************************************************
std::vector<std::string> SymbolTable::getAllSymbols() const noexcept
{
  std::vector<std::string> symbols;
  for (uint32_t hashValue = 0; hashValue < TABLE_SIZE; ++hashValue)
  {
    HashNode *node = myHashTable[hashValue];
    while (node != nullptr)
    {
      const char* symbolPtr = getString(hashValue, node->myIndex);
      std::string symbol(symbolPtr, node->myLength);
      symbol = "(" + std::to_string(node->myScopeLevel) + ") " + symbol;
      symbols.push_back(symbol);
      node = node->myNextNode;
    }
  }
  return symbols;
}

//*******************************************************
// SymbolTable::getSegmentNumberAndOffset
//*******************************************************
void SymbolTable::getSegmentNumberAndOffset(uint32_t theIndex,
                                            uint32_t &theSegmentNumber,
                                            uint32_t &theOffset)
{
  theSegmentNumber = theIndex / SEGMENT_SIZE_BYTES;
  theOffset = theIndex % SEGMENT_SIZE_BYTES;
}

//*******************************************************
// SymbolTable::getString
//*******************************************************
const char* SymbolTable::getString(uint32_t theHashValue, uint32_t theIndex)
  const noexcept
{
  uint32_t segmentNumber, offset;
  getSegmentNumberAndOffset(theIndex, segmentNumber, offset);

  auto &stringSpace = myStringSpaces[theHashValue];
  const StringSegment &segment = stringSpace.at(segmentNumber);
  const char *string = &segment[offset];

  return string;
}

//*******************************************************
// SymbolTable::hash
//*******************************************************
uint32_t SymbolTable::hash(const std::string &theIdentifier) noexcept
{
  uint32_t hashValue = 0;

  for (auto ii = 0u; ii < theIdentifier.size(); ++ii)
  {
    hashValue += std::tolower(theIdentifier[ii]);
  }

  hashValue %= TABLE_SIZE;

  return hashValue;
}
