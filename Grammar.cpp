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

  return makeTerminal(theSymbol);
}

//*******************************************************
// Grammar::makeTerminal
//*******************************************************
std::shared_ptr<Symbol> Grammar::makeTerminal(
  const std::string &theSymbol) noexcept
{
  std::shared_ptr<Symbol> terminal(new TerminalSymbol(theSymbol));

  Symbol::SymbolSet::iterator terminalIter =
    myTerminalSymbols.find(terminal);
  if (terminalIter == myTerminalSymbols.end())
  {
    myTerminalSymbols.insert(terminal);
    return terminal;
  }

  return *terminalIter;
}

//*******************************************************
// Grammar::populateGrammar
//*******************************************************
void Grammar::populateGrammar()
{
  readTokens();
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

        auto token = std::strtok(ruleArray, ":");
        ScannerTable::State nextStep = std::strtoul(token, 0, 10);

        token = std::strtok(NULL, ":");
        std::string action{token};

        token = std::strtok(NULL, ":");
        ScannerTable::TokenId tokenId = std::strtoul(token, 0, 10);

        ScannerTable::Entry entry{nextStep, action, tokenId};
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
// Grammar::readTokens
//*******************************************************
void Grammar::readTokens()
{
  std::string inputLine;

  while(readLine(inputLine) && inputLine != SECTION_DELIM)
  {
    std::stringstream splitter {inputLine};
    ScannerTable::TokenId tokenId;
    std::string tokenName;
    splitter >> tokenId >> tokenName;
    myTokens.emplace(std::make_pair(tokenId, tokenName));
    myScannerTable.addToken(tokenId, tokenName);
    if (!splitter.eof())
    {
      std::string reservedWord;
      splitter >> reservedWord;
      myReservedWords.emplace(std::make_pair(tokenId, reservedWord));
      myScannerTable.addReservedWord(tokenId, reservedWord);
    }
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
        << std::endl
        << "Tokens" << std::endl
        << "------" << std::endl;
  for (auto tokenPair : theGrammar.myTokens)
  {
    auto tokenId = tokenPair.first;
    theOS << std::setw(3) << tokenId << " " << tokenPair.second;
    auto reservedWord = theGrammar.myReservedWords.find(tokenId);
    if (reservedWord != theGrammar.myReservedWords.end())
    {
      theOS << " (" << reservedWord->second << ")";
    }
    theOS << std::endl;
  }
  theOS << std::endl;

  theOS << "Productions" << std::endl
        << "-----------" << std::endl;
  for (auto production : theGrammar.myProductions)
  {
    theOS << *production << std::endl;
  }
  theOS << std::endl;

  theOS << "Start Symbol: " << *theGrammar.myStartSymbol << std::endl
        << std::endl;

  theOS << "Terminal Symbols" << std::endl
        << "----------------" << std::endl;
  for (auto symbol : theGrammar.myTerminalSymbols)
  {
    theOS << *symbol << std::endl;
  }
  theOS << std::endl;

  theOS << "Non-Terminal Symbols" << std::endl
        << "--------------------" << std::endl;
  for (auto symbol : theGrammar.myNonTerminalSymbols)
  {
    theOS << *symbol << std::endl;
  }

  return theOS;
}
