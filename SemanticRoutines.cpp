/**
 * @file SemanticRoutines.cpp
 * @brief Implementation of SemanticRoutines class
 *
 * @author Michael Albers
 */

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstring>
#include <exception>
#include <iomanip>
#include <sstream>
#include <vector>

#include "ActionSymbol.h"
#include "ErrorWarningTracker.h"
#include "SemanticRoutines.h"
#include "SemanticStack.h"

//*******************************************************
// SemanticRoutines::SemanticRoutines
//*******************************************************
SemanticRoutines::SemanticRoutines(const std::string &theGeneratedCodeFileName,
                                   SemanticStack &theSemanticStack,
                                   ErrorWarningTracker &theEWTracker) :
  myEWTracker(theEWTracker),
  myGeneratedCodeFileName(theGeneratedCodeFileName),
  mySemanticStack(theSemanticStack)
{

  myGeneratedCodeFile.open(myGeneratedCodeFileName);
  auto localErrno = errno;
  if (! myGeneratedCodeFile)
  {
    std::ostringstream error;  
    error << "Failed to open generated code file '"
          << myGeneratedCodeFileName << "': " << std::strerror(localErrno);
    myEWTracker.reportError(error.str());
    throw std::runtime_error{error.str()};
  }

#define ADD_ROUTINE(x,y) mySemanticRoutines[#x] = &SemanticRoutines::y
  ADD_ROUTINE(assign, assign);
  ADD_ROUTINE(copy, copy);
  ADD_ROUTINE(geninfix, genInfix);
  ADD_ROUTINE(finish, finish);
  ADD_ROUTINE(processid, processId);
  ADD_ROUTINE(processliteral, processLiteral);
  ADD_ROUTINE(processop, processOp);
  ADD_ROUTINE(readid, readId);
  ADD_ROUTINE(start, start);
  ADD_ROUTINE(writeexpr, writeExpr);
#undef ADD_ROUTINE
}

//*******************************************************
// SemanticRoutines::executeSemanticRoutine
//*******************************************************
void SemanticRoutines::executeSemanticRoutine(
  const std::shared_ptr<Symbol> theActionSymbol)
{
  const ActionSymbol *actionSymbol = dynamic_cast<const ActionSymbol*>(
    theActionSymbol.get());

  std::string actionSymbolText{actionSymbol->getName()};
  actionSymbolText.erase(actionSymbolText.begin()); // Remove #

  auto lParenPosition = actionSymbolText.find('(');
  std::string semanticRoutine{actionSymbolText.substr(0, lParenPosition)};

  std::vector<std::string> arguments;

  if (lParenPosition != std::string::npos)
  {
    auto rParenPosition = actionSymbolText.find(')');
    std::string argumentString{actionSymbolText.substr(
        lParenPosition+1, rParenPosition-lParenPosition-1)};

    std::string::size_type tokenStart = 0;
    auto position = argumentString.find(',', tokenStart);
    while (position != std::string::npos)
    {
      arguments.push_back(argumentString.substr(tokenStart,
                                                position-tokenStart));
      tokenStart = position + 1;
      position = argumentString.find(',', tokenStart);
    }
    arguments.push_back(argumentString.substr(tokenStart,
                                              position-tokenStart));
  }

  std::transform(semanticRoutine.begin(), semanticRoutine.end(),
                 semanticRoutine.begin(), ::tolower);

  auto routine = mySemanticRoutines[semanticRoutine];
  routine(this, arguments);
}

//*******************************************************
// SemanticRoutines::getCode
//*******************************************************
std::vector<std::string> SemanticRoutines::getCode() const noexcept
{
  return myGeneratedCode;
}

//*******************************************************
// SemanticRoutines::getOperand
//*******************************************************
std::string SemanticRoutines::getOperand(std::string theOperand) noexcept
{
  if (! std::isdigit(theOperand[0]))
  {
    theOperand = "Addr(" + theOperand + ")";
  }
  return theOperand;
}


/*
 * Semantic routines. (Segregated for readability.)
 */


//*******************************************************
// SemanticRoutines::assign
//*******************************************************
void SemanticRoutines::assign(std::vector<std::string> &theArguments) noexcept
{
  auto &targetRecord = mySemanticStack.getRecordFromArgument(theArguments[0]);
  auto &sourceRecord = mySemanticStack.getRecordFromArgument(theArguments[1]);
  generate("ASSIGN", getOperand(sourceRecord.extract()),
           getOperand(targetRecord.extract()));
}

//*******************************************************
// SemanticRoutines::checkId
//*******************************************************
void SemanticRoutines::checkId(SemanticRecord &theIdentifier) noexcept
{
  if (false == lookUp(theIdentifier))
  {
    enter(theIdentifier);
    generate("DECLARE", theIdentifier.extract(), "Integer");
  }
}

//*******************************************************
// SemanticRoutines::copy
//*******************************************************
void SemanticRoutines::copy(std::vector<std::string> &theArguments) noexcept
{
  auto &sourceRecord = mySemanticStack.getRecordFromArgument(theArguments[0]);
  auto &targetRecord = mySemanticStack.getRecordFromArgument(theArguments[1]);
  targetRecord = sourceRecord;
}

//*******************************************************
// SemanticRoutines::enter
//*******************************************************
void SemanticRoutines::enter(SemanticRecord &theIdentifier) noexcept
{
  mySymbolTable.push_back(theIdentifier);
}

//*******************************************************
// SemanticRoutines::finish
//*******************************************************
void SemanticRoutines::finish(std::vector<std::string> &theArguments) noexcept
{
  generate("HALT");
}

//*******************************************************
// SemanticRoutines::generate
//*******************************************************
void SemanticRoutines::generate(const std::string &theInstruction) noexcept
{
  if (! myEWTracker.hasError())
  {
    std::ostringstream code;
    code << getTupleCode() << " (" << theInstruction << ")";
    myGeneratedCode.push_back(code.str());
    myGeneratedCodeFile << code.str() << std::endl;;
  }
}

//*******************************************************
// SemanticRoutines::generate
//*******************************************************
void SemanticRoutines::generate(const std::string &theInstruction,
                                const std::string &theFirst) noexcept
{
  if (! myEWTracker.hasError())
  {
    std::ostringstream code;
    code << getTupleCode() << " (" << theInstruction << ", " << theFirst << ")";
    myGeneratedCode.push_back(code.str());
    myGeneratedCodeFile << code.str() << std::endl;;
  }
}

//*******************************************************
// SemanticRoutines::generate
//*******************************************************
void SemanticRoutines::generate(const std::string &theInstruction,
                                const std::string &theFirst,
                                const std::string &theSecond) noexcept
{
  if (! myEWTracker.hasError())
  {
    std::ostringstream code;
    code << getTupleCode() << " (" << theInstruction << ", " << theFirst
         << ", " << theSecond << ")";
    myGeneratedCode.push_back(code.str());
    myGeneratedCodeFile << code.str() << std::endl;;
  }
}

//*******************************************************
// SemanticRoutines::generate
//*******************************************************
void SemanticRoutines::generate(const std::string &theInstruction,
                                const std::string &theFirst,
                                const std::string &theSecond,
                                const std::string &theThird) noexcept
{
  if (! myEWTracker.hasError())
  {
    std::ostringstream code;
    code << getTupleCode() << " (" << theInstruction << ", " << theFirst
         << ", " << theSecond << ", " << theThird << ")";
    myGeneratedCode.push_back(code.str());
    myGeneratedCodeFile << code.str() << std::endl;;
  }
}

//*******************************************************
// SemanticRoutines::genInfix
//*******************************************************
void SemanticRoutines::genInfix(std::vector<std::string> &theArguments) noexcept
{
  auto &expr1 = mySemanticStack.getRecordFromArgument(theArguments[0]);
  auto &op = mySemanticStack.getRecordFromArgument(theArguments[1]);
  auto &expr2 = mySemanticStack.getRecordFromArgument(theArguments[2]);
  auto &result = mySemanticStack.getRecordFromArgument(theArguments[3]);

  SemanticRecord temporary{getTemp()};
  generate(op.extract(), getOperand(expr1.extract()), getOperand(expr2.extract()),
           temporary.extract());
  result = temporary;
}

//*******************************************************
// SemanticRoutines::getTemp
//*******************************************************
SemanticRecord SemanticRoutines::getTemp() noexcept
{
  std::string tempName{"Temp&" + std::to_string(++myNextTemp)};
  return SemanticRecord(ExpressionRecord(ExpressionRecord::Kind::Temporary,
                                         tempName));
}

//*******************************************************
// SemanticRoutines::getTupleCode
//*******************************************************
std::string SemanticRoutines::getTupleCode() noexcept
{
  std::ostringstream prefix;
  prefix << "(" << std::setw(2) << ++myTupleNumber << ")";
  return prefix.str();
}

//*******************************************************
// SemanticRoutines::lookUp
//*******************************************************
bool SemanticRoutines::lookUp(SemanticRecord &theIdentifier) const noexcept
{
  auto location =
    std::find(mySymbolTable.begin(), mySymbolTable.end(), theIdentifier);
  bool found = (location != mySymbolTable.end());
  return found;
}

//*******************************************************
// SemanticRoutines::processId
//*******************************************************
void SemanticRoutines::processId(std::vector<std::string> &theArguments)
  noexcept
{
  auto &identifier = mySemanticStack.getRecordAtCurrentIndexMinusOne();

  SemanticRecord newIdentifier(ExpressionRecord(ExpressionRecord::Kind::Id,
                                                identifier.extract()));

  checkId(newIdentifier);

  auto &out = mySemanticStack.getRecordFromArgument(theArguments[0]);

  out = newIdentifier;
}

//*******************************************************
// SemanticRoutines::processLiteral
//*******************************************************
void SemanticRoutines::processLiteral(std::vector<std::string> &theArguments)
  noexcept
{
  auto &literal = mySemanticStack.getRecordAtCurrentIndexMinusOne();

  SemanticRecord newLiteral(ExpressionRecord(ExpressionRecord::Kind::Literal,
                                             literal.extract()));
  auto &out = mySemanticStack.getRecordFromArgument(theArguments[0]);
  out = newLiteral;
}

//*******************************************************
// SemanticRoutines::processOp
//*******************************************************
void SemanticRoutines::processOp(std::vector<std::string> &theArguments)
  noexcept
{
  auto &operatorData = mySemanticStack.getRecordAtCurrentIndexMinusOne();

  std::string operatorString{operatorData.extract()};

  OperatorRecord tempRecord(operatorString);
  SemanticRecord operatorRecord(tempRecord);

  auto &out = mySemanticStack.getRecordFromArgument(theArguments[0]);

  out = operatorRecord;
}

//*******************************************************
// SemanticRoutines::readId
//*******************************************************
void SemanticRoutines::readId(std::vector<std::string> &theArguments) noexcept
{
  auto &variable = mySemanticStack.getRecordFromArgument(theArguments[0]);
  generate("READI", variable.extract());
}

//*******************************************************
// SemanticRoutines::start
//*******************************************************
void SemanticRoutines::start(std::vector<std::string> &theArguments) noexcept
{
  myNextTemp = 0;
}

//*******************************************************
// SemanticRoutines::writeExpr
//*******************************************************
void SemanticRoutines::writeExpr(std::vector<std::string> &theArguments)
  noexcept
{
  auto &expression = mySemanticStack.getRecordFromArgument(theArguments[0]);
  generate("WRITEI", expression.extract());
}
