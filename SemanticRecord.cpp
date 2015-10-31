/**
 * @file SemanticRecord.cpp
 * @brief Implementation of SemanticRecord class
 *
 * @author Michael Albers
 */

#include "SemanticRecord.h"

//*******************************************************
// SemanticRecord::SemanticRecord
//*******************************************************
SemanticRecord::SemanticRecord() :
  SemanticRecord(ExpressionRecord(ExpressionRecord::Kind::Id, "__Placeholder__"))
{
}

//*******************************************************
// SemanticRecord::SemanticRecord
//*******************************************************
SemanticRecord::SemanticRecord(const ErrorRecord &theError) :
  myError(theError),
  myType(Type::Error)
{
}

//*******************************************************
// SemanticRecord::SemanticRecord
//*******************************************************
SemanticRecord::SemanticRecord(const ExpressionRecord &theExpression) :
  myExpression(theExpression),
  myType(Type::Expression)
{
}

//*******************************************************
// SemanticRecord::SemanticRecord
//*******************************************************
SemanticRecord::SemanticRecord(const OperatorRecord &theOperator) :
  myOperator(theOperator),
  myType(Type::Operator)
{
}

//*******************************************************
// SemanticRecord::SemanticRecord
//*******************************************************
SemanticRecord::SemanticRecord(const PlaceholderRecord &thePlaceholder) :
  myPlaceholder(thePlaceholder),
  myType(Type::Placeholder)
{
}

//*******************************************************
// SemanticRecord::operator==
//*******************************************************
bool SemanticRecord::operator==(const SemanticRecord &theRHS)
  const noexcept
{
  bool equal = false;
  if (getType() == theRHS.getType())
  {
    const auto *lhs = getRecord();
    const auto *rhs = theRHS.getRecord();
    equal = (*lhs == *rhs);
  }
  return equal;
}

//*******************************************************
// SemanticRecord::extract
//*******************************************************
std::string SemanticRecord::extract() const noexcept
{
  return getRecord()->extract();
}

//*******************************************************
// SemanticRecord::getRecord
//*******************************************************
const Record* SemanticRecord::getRecord() const noexcept
{
  const Record *record = &myOperator;
  switch (myType)
  {
    case Type::Error:
      record = &myError;
      break;

    case Type::Expression:
      record = &myExpression;
      break;

    case Type::Placeholder:
      record = &myPlaceholder;

    case Type::Operator:
    default:
      break;
  }

  return record;
}

//*******************************************************
// SemanticRecord::getType
//*******************************************************
SemanticRecord::Type SemanticRecord::getType() const noexcept
{
  return myType;
}
