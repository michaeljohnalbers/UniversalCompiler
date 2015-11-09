#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

/**
 * @file SymbolTable.h
 * @brief Implementation of symbol table.
 *
 * @author Michael Albers
 */

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/**
 * Symbol table.
 */
class SymbolTable
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Attributes of an item in the symbol table. Very basic as MicroLanguage
   * doesn't really have any symbol attributes.
   */
  class SymbolAttributes
  {
    public:
    std::string myType{"Integer"};
  };

  /**
   * Default constructor.
   */
  SymbolTable();

  /**
   * Copy constructor.
   */
  SymbolTable(const SymbolTable&) = default;

  /**
   * Move constructor.
   */
  SymbolTable(SymbolTable&&) = default;

  /**
   * Destructor.
   */
  ~SymbolTable();

  /**
   * Copy assignment operator.
   */
  SymbolTable& operator=(const SymbolTable&) = default;

  /**
   * Move assignment operator
   */
  SymbolTable& operator=(SymbolTable&&) = default;

  /**
   * Adds the given identifier to the symbol table at the current scope.
   * A duplicate of an identifier already in the table at the current scope
   * will not be added.
   *
   * @param theIdentifier
   *          identifier to add
   * @param theAttributes
   *          OUT parameter - attributes of the identifier. If the identifier
   *          is already in the table, this will be populated with that
   *          symbol's attributes. If the identifier is to be added, this will
   *          be populated with the new symbol's attributes.
   * @return true if symbol was already in the table
   */
  bool add(const std::string &theIdentifier,
           SymbolAttributes &theAttributes) noexcept;

  /**
   * Adds a new scope to the symbol table.
   */
  void createNewScope() noexcept;

  /**
   * Destroys the current scope.
   *
   * @throws std::underflow_error
   *           on trying to destroy lowest (global) scope
   */
  void destroyCurrentScope();

  /**
   * Finds the given identifier in the symbol table.
   *
   * @param theIdentifier
   *          identifier to find
   * @param theAttributes
   *          OUT parameter - attributes of the identifier (only populated if
   *          the identifier is found)
   * @return true if the symbol was found
   */
  bool find(const std::string &theIdentifier,
            SymbolAttributes &theAttributes) const noexcept;

  /**
   * Returns all symbols currently defined in the table.
   *
   * @return unordered list of all symbols
   */
  std::vector<std::string> getAllSymbols() const noexcept;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /**
   * Node in linked list for handling hash collisions.
   */
  class HashNode
  {
    public:
    ~HashNode()
    {
      if (myNextNode != nullptr)
      {
        delete myNextNode;
      }
    }

    uint32_t myIndex = -1;
    uint32_t myLength = -1;
    uint32_t myScopeLevel = 0;
    SymbolAttributes myAttributes;
    HashNode *myNextNode = nullptr;
  };

  /** Semi-arbitrary table size (prime number is only constraint). */
  static constexpr uint32_t TABLE_SIZE = 17;

  /**
   * Hashing function.
   *
   * @param theIdentifier
   *          identifier to hash
   * @return index into hash table.
   */
  static uint32_t hash(const std::string &theIdentifier) noexcept;

  /**
   * Finds the identifier as the given hash value.
   *
   * @param theIdentifier
   *          identifier to find
   * @param theAttributes
   *          attributes of the identifier (populated if found, otherwise
   *          modified)
   * @param theHashValue
   *          pre-computed hash value of theIdentifier
   * @param theScopeLevel
   *          only find symbols >= this level (use MIN_SCOPE_LEVEL to search
   *          entire table)
   */
  bool find(const std::string &theIdentifier,
            SymbolAttributes &theAttributes,
            uint32_t theHashValue,
            uint32_t theScopeLevel) const noexcept;

  /**
   * Given the index computes the string space segment number and offset.
   *
   * @param theIndex
   *          index into string space
   * @param theSegmentNumber
   *          OUT parameter - segment number
   * @param theOffset
   *          OUT parameter - offset into the segment
   */
  static void getSegmentNumberAndOffset(uint32_t theIndex,
                                        uint32_t &theSegmentNumber,
                                        uint32_t &theOffset);

  /**
   * Given the hash value and index of a string returns a pointer to it.
   *
   * @param theHashValue
   *          hash value of string to retrieve
   * @param theIndex
   *          index of string to retrieve
   */
  const char* getString(uint32_t theHashValue, uint32_t theIndex)
    const noexcept;

  /** Hash table (parallel array to myStringSpaces). */
  mutable std::array<HashNode*, TABLE_SIZE> myHashTable;

  /** Minimum scoping level. */
  static constexpr uint32_t MIN_SCOPE_LEVEL = 0;

  /** Current scope level of the table. */
  uint32_t myScopeLevel = MIN_SCOPE_LEVEL;

  /** String segment size */
  static constexpr uint32_t SEGMENT_SIZE_BYTES = 500;

  /** String space (parallel array to myHashTable). */
  using StringSegment = std::array<char, SEGMENT_SIZE_BYTES>;
  std::array<std::vector<StringSegment>, TABLE_SIZE> myStringSpaces;
};

#endif
