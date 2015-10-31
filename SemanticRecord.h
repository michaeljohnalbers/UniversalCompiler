#ifndef SEMANTICRECORD_H
#define SEMANTICRECORD_H

/**
 * @file SemanticRecord.h
 * @brief Base class for all semantic records
 *
 * @author Michael Albers
 */

#include <string>

#include "RecordClasses.h"

/**
 * Base class for all semantic records.
 */
class SemanticRecord
{
  // ************************************************************
  // Public
  // ************************************************************
  public:

  enum class Type
  {
    Expression,
    Operator,
    Error,
    Placeholder,
  };

  /**
   * Default constructor.
   */
  SemanticRecord();

  /**
   * Copy constructor.
   */
  SemanticRecord(const SemanticRecord&) = default;

  /**
   * Move constructor.
   */
  SemanticRecord(SemanticRecord&&) = default;

  /**
   * Constructor.
   *
   * @param theError
   *          error for this record
   */
  SemanticRecord(const ErrorRecord &theError);

  /**
   * Constructor.
   *
   * @param theExpression
   *          expression for this record
   */
  SemanticRecord(const ExpressionRecord &theExpression);

  /**
   * Constructor.
   *
   * @param theOperator
   *          operator for this record
   */
  SemanticRecord(const OperatorRecord &theOperator);

  /**
   * Constructor.
   *
   * @param thePlaceholder
   *          placeholder for this record
   */
  SemanticRecord(const PlaceholderRecord &thePlaceholder);

  /**
   * Destructor.
   */
  virtual ~SemanticRecord() = default;

  /**
   * Copy assignment operator.
   */
  SemanticRecord& operator=(const SemanticRecord&) = default;

  /**
   * Move constructor.
   */
  SemanticRecord& operator=(SemanticRecord&&) = default;

  /**
   * Equality operator.
   *
   * @param theRHS
   *          RHS of *this == theRHS
   * @return true if the given record is equal to this record
   */
  bool operator==(const SemanticRecord &theRHS) const noexcept;

  /**
   * Extract record information as a string suitable for
   * code generation.
   *
   * @return semantic information
   */
  std::string extract() const noexcept;

  /**
   * Returns the correct record for this class.
   *
   * @return record
   */
  const Record* getRecord() const noexcept;

  /**
   * Returns the type of this record.
   *
   * @return record type
   */
  Type getType() const noexcept;

  // ************************************************************
  // Protected
  // ************************************************************
  protected:

  // ************************************************************
  // Private
  // ************************************************************
  private:

  /** Operator data */
  OperatorRecord myOperator;

  /** Expression data */
  ExpressionRecord myExpression;

  /** Error record. */
  ErrorRecord myError;

  /** Placeholder record. */
  PlaceholderRecord myPlaceholder;

  /** Which record type to use. */
  Type myType = Type::Operator;
};

#endif
