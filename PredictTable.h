#ifndef PREDICTTABLE_H
#define PREDICTTABLE_H

/**
 * @file PredictTable.h
 * @brief Table for predicting which production to use.
 *
 * @author Michael Albers
 */

#include <cstdint>
#include <map>
#include <memory>
#include <ostream>

#include "Symbol.h"

class Grammar;

/**
 * The class encapuslates a predict table. The table is used to determine
 * the next production to apply given a terminal and non-terminal symbol.
 */
class PredictTable
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor
   */
  PredictTable() = delete;

  /**
   * Copy constructor
   */
  PredictTable(const PredictTable&) = default;

  /**
   * Move constructor
   */
  PredictTable(PredictTable&&) = default;

  /**
   * Constructor. Populates the predict table from the Grammar object.
   *
   * @param theGrammar
   *          Grammar object with predict sets filled in.
   */
  PredictTable(const Grammar &theGrammar);

  /**
   * Destructor
   */
  ~PredictTable() = default;

  /**
   * Copy assignment operator
   */
  PredictTable& operator=(const PredictTable&) = default;

  /**
   * Move assignment operator
   */
  PredictTable& operator=(PredictTable&&) = default;

  /**
   * Returns the production number to use for the given non-terminal and
   * terminal.
   *
   * @param theNonTerminal
   *          non-terminal for lookup
   * @param theTerminal
   *          terminal for lookup
   * @return production number or 0 on invalid symbol combination
   */
  uint32_t getProductionNumber(std::shared_ptr<Symbol> theNonTerminal,
                               std::shared_ptr<Symbol> theTerminalId)
    const noexcept;

  /**
   * Stream insertion operator.
   *
   * @param theOS
   *          stream to insert into
   * @param thePredictTable
   *          predict table to insert into theOS
   * @return modified stream
   */
  friend std::ostream& operator<<(std::ostream &theOS,
                                  const PredictTable &thePredictTable);

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /**
   * Populates the predict table.
   */
  void populateTable();

  /** Grammar data */
  const Grammar &myGrammar;

  /** Size of the longest column name. */
  uint32_t myLargestColumnNameSize = 0;

  /** Size of the longest non-terminal name. */
  uint32_t myLargestNonTerminalNameSize = 0;

  /**
   * Predict table. Mimics a 2-D array.
   * myTable[non-terminal][terminal] = production number
   */
  std::map<std::shared_ptr<Symbol>,
           std::map<std::shared_ptr<Symbol>, uint32_t>> myTable;

  /** All terminals used in the predict table. */
  Symbol::SymbolSet myTerminals;
};

#endif
