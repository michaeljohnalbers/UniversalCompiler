#ifndef RECORDCLASSES_H
#define RECORDCLASSES_H

/**
 * @file RecordClasse.h
 * @brief Defines classes to represent all semantic records.
 *
 * @author Michael Albers
 */

#include <string>
#include <typeinfo>

#include "Token.h"

class Record
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  virtual ~Record() = default;

  /**
   * Equality operator.
   *
   * @param theRHS
   *          RHS of *this == theRHS
   * @return true if the given record is equal to this record
   */
  virtual bool operator==(const Record &theRHS) const noexcept
  {
    bool equal = false;
    if (typeid(*this) == typeid(theRHS))
    {
      equal = (extract() == theRHS.extract());
    }
    return equal;
  }

  /**
   * Generate string for code generation.
   */
  virtual std::string extract() const noexcept = 0;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:
};

/**
 * Stores an operator.
 */
class OperatorRecord : public Record
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /** Supported operators. */
  enum class Operator
  {
    Plus,
    Minus,
  };

  /**
   * Default constructor.
   */
  OperatorRecord() = default;

  /**
   * Copy constructor.
   */
  OperatorRecord(const OperatorRecord&) = default;

  /**
   * Move constructor.
   */
  OperatorRecord(OperatorRecord&&) = default;

  /**
   * Constructor.
   *
   * @param theOperator
   *          operator
   */
  OperatorRecord(Operator theOperator) :
    myOperator(theOperator)
  {}

  /**
   * Constructor.
   *
   * @param theOperator
   *          operator
   */
  OperatorRecord(const std::string &theOperator)
  {
    if ("+" == theOperator)
    {
      myOperator = Operator::Plus;
    }
    else if ("-" == theOperator)
    {
      myOperator = Operator::Minus;
    }
  }

  /**
   * Destructor.
   */
  ~OperatorRecord() = default;

  /**
   * Copy assignment operator.
   */
  OperatorRecord& operator=(const OperatorRecord&) = default;

  /**
   * Move assignment operator.
   */
  OperatorRecord& operator=(OperatorRecord&&) = default;

  /**
   * @see Record::extract
   */
  std::string extract() const noexcept override
  {
    switch (myOperator)
    {
      case Operator::Plus:
        return "ADDI";
        break;

      case Operator::Minus:
      default:
        return "SUBI";
        break;
    }
  }

  /**
   * Returns operator.
   *
   * @return operator
   */
  Operator getOperator() const noexcept
  {
    return myOperator;
  }

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Operator stored. */
  Operator myOperator = Operator::Plus;
};


/**
 * Stores an expression.
 */
class ExpressionRecord : public Record
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /** Supported expressions. */
  enum class Kind
  {
    Id,
    Literal,
    Temporary,
  };

  /**
   * Default constructor.
   */
  ExpressionRecord() = default;

  /**
   * Copy constructor.
   */
  ExpressionRecord(const ExpressionRecord&) = default;

  /**
   * Move constructor.
   */
  ExpressionRecord(ExpressionRecord&&) = default;

  /**
   * Constructor.
   *
   * @param theExpression
   *          expression
   */
  ExpressionRecord(Kind theExpressionKind, const std::string &theValue) :
    myKind(theExpressionKind),
    myValue(theValue)
  {}

  /**
   * Destructor.
   */
  ~ExpressionRecord() = default;

  /**
   * Copy assignment expression.
   */
  ExpressionRecord& operator=(const ExpressionRecord&) = default;

  /**
   * Move assignment expression.
   */
  ExpressionRecord& operator=(ExpressionRecord&&) = default;

  /**
   * @see Record::extract
   */
  std::string extract() const noexcept override
  {
    return myValue;
  }

  /**
   * Returns the expression kind.
   *
   * @return expression kind
   */
  Kind getKind() const noexcept
  {
    return myKind;
  }

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Expression kind. */
  Kind myKind = Kind::Id;

  /** Value suitable for any kind. */
  std::string myValue;
};

/**
 * Stores an error.
 */
class ErrorRecord : public Record
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  ErrorRecord() = default;

  /**
   * Copy constructor.
   */
  ErrorRecord(const ErrorRecord&) = default;

  /**
   * Move constructor.
   */
  ErrorRecord(ErrorRecord&&) = default;

  /**
   * Destructor.
   */
  ~ErrorRecord() = default;

  /**
   * Copy assignment error.
   */
  ErrorRecord& operator=(const ErrorRecord&) = default;

  /**
   * Move assignment error.
   */
  ErrorRecord& operator=(ErrorRecord&&) = default;

  /**
   * @see Record::extract
   */
  std::string extract() const noexcept override
  {
    return "";
  }

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

};

/**
 * Placeholder item. Used to hold data from parse until it is used later. (See
 * parser code for handling terminals.)
 */
class PlaceholderRecord : public Record
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  /**
   * Default constructor.
   */
  PlaceholderRecord() = default;

  /**
   * Copy constructor.
   */
  PlaceholderRecord(const PlaceholderRecord&) = default;

  /**
   * Move constructor.
   */
  PlaceholderRecord(PlaceholderRecord&&) = default;

  /**
   * Destructor.
   */
  ~PlaceholderRecord() = default;

  /**
   * Constructor.
   */
  PlaceholderRecord(Token &theToken) :
    myToken(theToken)
  {
  }

  /**
   * Copy assignment placeholder.
   */
  PlaceholderRecord& operator=(const PlaceholderRecord&) = default;

  /**
   * Move assignment placeholder.
   */
  PlaceholderRecord& operator=(PlaceholderRecord&&) = default;

  /**
   * @see Record::extract
   */
  std::string extract() const noexcept override
  {
    return myToken.getToken();
  }

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Scanned token */
  Token myToken;
};

#endif
