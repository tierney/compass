/*
 * Expression.h
 * Definition of the structure used to build the syntax tree.
 */
#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief The operation type
 */
typedef enum tagEOperationType {
  eSTRING,
  eVALUE,
  eMULTIPLY,
  ePLUS,
  eAND,
  eOR,
} EOperationType;

/**
 * @brief The expression structure
 */
typedef struct tagSExpression {
  EOperationType type;///< type of operation

  int value;///< valid only when type is eVALUE
  char* string;
  struct tagSExpression* left; ///< left side of the tree
  struct tagSExpression* right;///< right side of the tree
}SExpression;

/**
 * @brief It creates an identifier
 * @param value The number value
 * @return The expression or NULL in case of no memory
 */
SExpression* createNumber(int value);

SExpression* createString(const char* string);

/**
 * @brief It creates an operation
 * @param type The operation type
 * @param left The left operand
 * @param right The right operand
 * @return The expression or NULL in case of no memory
 */
SExpression* createOperation(
    EOperationType type,
    SExpression *left,
    SExpression *right);

/**
 * @brief Deletes a expression
 * @param b The expression
 */
void deleteExpression(SExpression *b);
#ifdef __cplusplus
}
#endif
#endif // __EXPRESSION_H__
