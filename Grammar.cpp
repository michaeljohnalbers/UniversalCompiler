/**
 * @file Grammar.cpp
 * @brief Implementation of Grammar class
 *
 * @author Michael Albers
 */

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "ErrorWarningTracker.h"
#include "Grammar.h"
#include "Lambda.h"
#include "NonTerminalSymbol.h"
#include "Production.h"
#include "TerminalSymbol.h"

const std::string Grammar::SECTION_DELIM{"-----"};

//*******************************************************
// Grammar::Grammar
//*******************************************************
Grammar::Grammar(const std::string &theFileName,
                 ErrorWarningTracker &theEWTracker,
                 ScannerTable &theScannerTable) :
  myEWTracker(theEWTracker),
  myFileName(theFileName),
  myScannerTable(theScannerTable)
{
  myFile.open(myFileName, std::ios::in);
  auto localErrno = errno;
  if (! myFile.is_open())
  {
    std::ostringstream error;
    error << "Failed to open grammar definition file '"
          << myFileName << "': " << std::strerror(localErrno);
    myEWTracker.reportError(error.str());
    throw std::runtime_error{error.str()};
  }

  populateGrammar();
}

//*******************************************************
// Grammar::getNonTerminalSymbols
//*******************************************************
Symbol::SymbolSet Grammar::getNonTerminalSymbols() const noexcept
{
  return myNonTerminalSymbols;
}

//*******************************************************
// Grammar::getProduction
//*******************************************************
std::shared_ptr<Production> Grammar::getProduction(uint32_t theProductionNumber)
  const noexcept
{
  return myProductions[theProductionNumber-1];
}

//*******************************************************
// Grammar::getProductions
//*******************************************************
std::vector<std::shared_ptr<Production>> Grammar::getProductions()
const noexcept
{
  return myProductions;
}

//*******************************************************
// Grammar::getStartSymbol
//*******************************************************
std::shared_ptr<Symbol> Grammar::getStartSymbol() const noexcept
{
  return myStartSymbol;
}

//*******************************************************
// Grammar::getTerminalSymbols
//*******************************************************
Symbol::SymbolSet Grammar::getTerminalSymbols() const noexcept
{
  return myTerminalSymbols;
}

//*******************************************************
// Grammar::makeNonTerminal
//*******************************************************
std::shared_ptr<Symbol> Grammar::makeNonTerminal(
  const std::string &theSymbol) noexcept
{
  std::shared_ptr<Symbol> nonTerminal(new NonTerminalSymbol(theSymbol));

  Symbol::SymbolSet::iterator nonTerminalIter =
    myNonTerminalSymbols.find(nonTerminal);
  if (nonTerminalIter == myNonTerminalSymbols.end())
  {
    myNonTerminalSymbols.insert(nonTerminal);
    return nonTerminal;
  }

  return *nonTerminalIter;
}

//*******************************************************
// Grammar::makeSymbol
//*******************************************************
std::shared_ptr<Symbol> Grammar::makeSymbol(
  const std::string &theSymbol)
{
  if ('<' == theSymbol[0])
  {
    return makeNonTerminal(theSymbol);
  }
  else
  {
    if ("$" == theSymbol)
    {
      return myScannerTable.getEOF();
    }
    else
    {
      for (auto terminal : myTerminalSymbols)
      {
        if (terminal->getName() == theSymbol)
        {
          return terminal;
        }
      }
    }

    std::ostringstream error;
    error << "Terminal symbol, \"" << theSymbol << "\" on line "
          << myLineNumber << " is not a valid symbol. "
          << "Check it against terminals defined at "
          << "the top of the grammar definition file.";
    throw std::out_of_range{error.str()};
  }
}

//*******************************************************
// Grammar::populateGrammar
//*******************************************************
void Grammar::populateGrammar()
{
  readTerminals();
  readScannerTable();
  readProductions();
  readStartSymbol();
}

//*******************************************************
// Grammar::readLine
//*******************************************************
bool Grammar::readLine(std::string &theInputLine) noexcept
{
  do
  {
    ++myLineNumber;
    std::getline(myFile, theInputLine);
  } while (theInputLine.size() == 0 || theInputLine[0] == '#');
  return true;
}

//*******************************************************
// Grammar::readScannerTable
//*******************************************************
void Grammar::readProductions()
{
  std::string inputLine;

  auto readSymbol = [&](std::stringstream &theSplitter)->std::string
  {
    while (theSplitter.peek() == ' ' || theSplitter.peek() == '\t')
    {
      theSplitter.get();
    }

    std::string symbolName;
    if (theSplitter.peek() == '<')
    {
      bool keepReading = true;
      while (keepReading)
      {
        char next = theSplitter.get();
        symbolName += next;
        keepReading = (next != '>');
      }
      // This either reads a whitespace after the terminal or gets us to EOF
      theSplitter.get();
    }
    else
    {
      theSplitter >> symbolName;
    }

    return symbolName;
  };

  uint32_t productionNumber = 1;
  while(readLine(inputLine) && inputLine != SECTION_DELIM)
  {
    std::stringstream splitter{inputLine};

    std::string symbolName{readSymbol(splitter)};
 
    std::shared_ptr<Symbol> lhsSymbol(makeNonTerminal(symbolName));

    std::shared_ptr<Production> production{
      new Production(lhsSymbol, productionNumber)};
    // getProduction assumes this is populated to line up with the value of
    // productionNumber
    myProductions.push_back(production);

    // Read/discard the ->
    std::string arrow;
    splitter >> arrow;

    bool hasRHS = false;
    while (! splitter.eof())
    {
      hasRHS = true;
      symbolName = readSymbol(splitter);
      auto symbol = makeSymbol(symbolName);
      production->addRHSSymbol(symbol);
    }

    if (! hasRHS)
    {
      production->addRHSSymbol(Lambda::getInstance());
    }

    ++productionNumber;
  }
}

//*******************************************************
// Grammar::readScannerTable
//*******************************************************
void Grammar::readScannerTable()
{
  std::string inputLine;

  // Read column definitions
  // Specials: letter(A-Za-Z), digit[0-9], whitespace (not EOL), EOL, other
  readLine(inputLine);
  std::stringstream splitter{inputLine};
  uint32_t numberColumns = 0;
  while (! splitter.eof())
  {
    ++numberColumns;
    std::string columnCharacterClass;
    splitter >> columnCharacterClass;
    myScannerTable.addColumn(columnCharacterClass);
  }

  uint32_t stateNumber = 0;
  while(readLine(inputLine) && inputLine != SECTION_DELIM)
  {
    std::stringstream splitter{inputLine};
    for (auto column = 0u; column < numberColumns; ++column)
    {
      std::string rule;
      splitter >> rule;
      if ("E" == rule)
      {
        ScannerTable::Entry entry{0, "E", 0};
        myScannerTable.addTableEntry(stateNumber, column, entry);
      }
      else
      {
        char ruleArray[rule.size()+1];
        std::strcpy(ruleArray, rule.c_str());

        auto terminal = std::strtok(ruleArray, ":");
        ScannerTable::State nextStep = std::strtoul(terminal, 0, 10);

        terminal = std::strtok(NULL, ":");
        std::string action{terminal};

        terminal = std::strtok(NULL, ":");
        TerminalSymbol::Id terminalId = std::strtoul(terminal, 0, 10);

        ScannerTable::Entry entry{nextStep, action, terminalId};
        myScannerTable.addTableEntry(stateNumber, column, entry);
      }
    }

    ++stateNumber;
  }
}

//*******************************************************
// Grammar::readStartSymbol
//*******************************************************
void Grammar::readStartSymbol()
{
  std::string startSymbol;
  readLine(startSymbol);
  for (auto symbol : myNonTerminalSymbols)
  {
    if (symbol->getName() == startSymbol)
    {
      myStartSymbol = symbol;
      break;
    }
  }

  if (myStartSymbol.get() == nullptr)
  {
    throw std::runtime_error{"Start symbol '" + startSymbol +
        "' is not a defined symbol."};
  }
}

//*******************************************************
// Grammar::readTerminals
//*******************************************************
void Grammar::readTerminals()
{
  std::string inputLine;

  while(readLine(inputLine) && inputLine != SECTION_DELIM)
  {
    std::stringstream splitter {inputLine};
    TerminalSymbol::Id terminalId;
    std::string terminalName;
    std::string reservedWord;

    splitter >> terminalId >> terminalName;
    if (!splitter.eof())
    {
      splitter >> reservedWord;
    }

    auto terminal = std::make_shared<TerminalSymbol>(
      terminalName, terminalId, reservedWord);

    myTerminalSymbols.insert(terminal);
    myScannerTable.addTerminal(terminal);
  }
}

//*******************************************************
// operator<<
//*******************************************************
std::ostream& operator<<(std::ostream &theOS,
                         const Grammar &theGrammar) noexcept
{
  theOS << "Grammar Definition" << std::endl
        << "==================" << std::endl
        << " Source File: " << theGrammar.myFileName << std::endl
        << std::endl;

  theOS << "Productions" << std::endl
        << "-----------" << std::endl;
  for (auto production : theGrammar.myProductions)
  {
    theOS << *production << std::endl;
  }
  theOS << std::endl;

  theOS << "Start Symbol: " << *theGrammar.myStartSymbol << std::endl
        << std::endl;

  TerminalSymbol::ourLongPrint = true;
  theOS << "Terminal Symbols" << std::endl
        << "----------------" << std::endl;
  for (auto symbol : theGrammar.myTerminalSymbols)
  {
    theOS << *symbol << std::endl;
  }
  theOS << std::endl;
  TerminalSymbol::ourLongPrint = false;

  theOS << "Non-Terminal Symbols" << std::endl
        << "--------------------" << std::endl;
  for (auto symbol : theGrammar.myNonTerminalSymbols)
  {
    theOS << *symbol << std::endl;
  }

  return theOS;
}
