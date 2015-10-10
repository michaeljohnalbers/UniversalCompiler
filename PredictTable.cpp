/**
 * @file PredictTable.cpp
 * @brief Implementation of PredictTable class
 *
 * @author Michael Albers
 */

#include <iomanip>

#include "Grammar.h"
#include "PredictTable.h"
#include "Production.h"

//*******************************************************
// PredictTable::PredictTable
//*******************************************************
PredictTable::PredictTable(const Grammar &theGrammar) :
  myGrammar(theGrammar)
{
  populateTable();
}

//*******************************************************
// PredictTable::populateTable
//*******************************************************
void PredictTable::populateTable()
{
  auto productions = myGrammar.getProductions();
  for (auto production : productions)
  {
    auto lhsSymbol = production->getLHS();

    auto nonTerminalNameSize = lhsSymbol->getName().size();
    if (nonTerminalNameSize > myLargestNonTerminalNameSize)
    {
      myLargestNonTerminalNameSize = nonTerminalNameSize;
    }

    auto predictSet = production->getPredictSet();
    for (auto predictSymbol : predictSet)
    {
      auto columnNameSize = predictSymbol->getName().size();
      if (columnNameSize > myLargestColumnNameSize)
      {
        myLargestColumnNameSize = columnNameSize;
      }
      myTable[lhsSymbol][predictSymbol] = production->getNumber();
      myTerminals.insert(predictSymbol);
    }
  }
}

//*******************************************************
// PredictTable::getProductionNumber
//*******************************************************
uint32_t PredictTable::getProductionNumber(
  std::shared_ptr<Symbol> theNonTerminal,
  std::shared_ptr<Symbol> theTerminal) const noexcept
{
  uint32_t productionNumber = 0;
  try
  {
    productionNumber = myTable.at(theNonTerminal).at(theTerminal);
  }
  catch(...) {}
  return productionNumber;
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS,
                         const PredictTable &thePredictTable)
{
  static constexpr uint32_t MIN_WIDTH = 3;

  theOS << std::setw(thePredictTable.myLargestNonTerminalNameSize)
        << "Predict Table" << std::endl
        << std::setw(thePredictTable.myLargestNonTerminalNameSize)
        << "=============" << std::endl;
  /*
   * Print the column headers (terminal symbol names) 
   */ 
  theOS << std::setw(thePredictTable.myLargestNonTerminalNameSize) << "";
  for (auto terminal : thePredictTable.myTerminals)
  {
    theOS << " | " << std::setw(MIN_WIDTH) << terminal->getName();
  }
  theOS << std::endl;

  /*
   * Print each LHS predict set. 
   */ 
  for (auto row : thePredictTable.myTable)
  {
    auto nonTerminal = row.first;
    auto columns = row.second;

    // Print the non-terminal symbol
    theOS << std::setw(thePredictTable.myLargestNonTerminalNameSize)
          << nonTerminal->getName();

    // Print production numbers.
    for (auto terminal : thePredictTable.myTerminals)
    {
      uint32_t nameSize = terminal->getName().size();
      theOS << " | " << std::setw(std::max(MIN_WIDTH, nameSize));
      const auto productionNumberIter = columns.find(terminal);
      if (productionNumberIter != columns.end())
      {
        theOS << productionNumberIter->second;
      }
      else
      {
        theOS << " ";
      }
    }
    theOS << std::endl;
  }
  return theOS;
}
