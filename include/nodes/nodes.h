/*
 * Copyright (c) 2019 TAOS Data, Inc. <jhtao@taosdata.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the GNU Affero General Public License, version 3
 * or later ("AGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TD_NODES_H_
#define _TD_NODES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tarray.h"
#include "tdef.h"

typedef enum ENodeType {
  QUERY_NODE_COLUMN = 1,
  QUERY_NODE_VALUE,
  QUERY_NODE_OPERATOR,
  QUERY_NODE_LOGIC_CONDITION,
  QUERY_NODE_IS_NULL_CONDITION,
  QUERY_NODE_FUNCTION,
  QUERY_NODE_REAL_TABLE,
  QUERY_NODE_TEMP_TABLE,
  QUERY_NODE_JOIN_TABLE,
  QUERY_NODE_GROUPING_SET,
  QUERY_NODE_ORDER_BY_EXPR,
  QUERY_NODE_STATE_WINDOW,
  QUERY_NODE_SESSION_WINDOW,
  QUERY_NODE_INTERVAL_WINDOW,

  QUERY_NODE_SET_OPERATOR,
  QUERY_NODE_SELECT_STMT
} ENodeType;

/**
 * The first field of a node of any type is guaranteed to be the ENodeType.
 * Hence the type of any node can be gotten by casting it to SNode. 
 */
typedef struct SNode {
  ENodeType type;
} SNode;

#define nodeType(nodeptr) (((const SNode*)(nodeptr))->type)

typedef struct SDataType {
  uint8_t type;
  uint8_t precision;
  uint8_t scale;
  int32_t bytes;
} SDataType;

typedef struct SExprNode {
  ENodeType nodeType;
  SDataType resType;
  char aliasName[TSDB_COL_NAME_LEN];
} SExprNode;

typedef enum EColumnType {
  COLUMN_TYPE_COLUMN = 1,
  COLUMN_TYPE_TAG
} EColumnType;

typedef struct SColumnNode {
  SExprNode node; // QUERY_NODE_COLUMN
  int16_t colId;
  EColumnType colType; // column or tag
  char dbName[TSDB_DB_NAME_LEN];
  char tableName[TSDB_TABLE_NAME_LEN];
  char colName[TSDB_COL_NAME_LEN];
} SColumnNode;

typedef struct SValueNode {
  SExprNode type; // QUERY_NODE_VALUE
  char* literal;
} SValueNode;

typedef enum EOperatorType {
  // arithmetic operator
  OP_TYPE_ADD = 1,
  OP_TYPE_SUB,
  OP_TYPE_MULTI,
  OP_TYPE_DIV,
  OP_TYPE_MOD,

  // comparison operator
  OP_TYPE_GREATER_THAN,
  OP_TYPE_GREATER_EQUAL,
  OP_TYPE_LOWER_THAN,
  OP_TYPE_LOWER_EQUAL,
  OP_TYPE_EQUAL,
  OP_TYPE_NOT_EQUAL,
  OP_TYPE_IN,
  OP_TYPE_NOT_IN,
  OP_TYPE_LIKE,
  OP_TYPE_NOT_LIKE,
  OP_TYPE_MATCH,
  OP_TYPE_NMATCH,

  // json operator
  OP_TYPE_JSON_GET_VALUE,
  OP_TYPE_JSON_CONTAINS
} EOperatorType;

typedef struct SOperatorNode {
  SExprNode type; // QUERY_NODE_OPERATOR
  EOperatorType opType;
  SNode* pLeft;
  SNode* pRight;
} SOperatorNode;

typedef enum ELogicConditionType {
  LOGIC_COND_TYPE_AND,
  LOGIC_COND_TYPE_OR,
  LOGIC_COND_TYPE_NOT,
} ELogicConditionType;

typedef struct SLogicConditionNode {
  ENodeType type; // QUERY_NODE_LOGIC_CONDITION
  ELogicConditionType condType;
  SArray* pParameterList;
} SLogicConditionNode;

typedef struct SIsNullCondNode {
  ENodeType type; // QUERY_NODE_IS_NULL_CONDITION
  SNode* pExpr;
  bool isNot;
} SIsNullCondNode;

typedef struct SFunctionNode {
  SExprNode type; // QUERY_NODE_FUNCTION
  char functionName[TSDB_FUNC_NAME_LEN];
  int32_t funcId;
  SArray* pParameterList; // SNode
} SFunctionNode;

typedef struct STableNode {
  ENodeType type;
  char tableName[TSDB_TABLE_NAME_LEN];
  char tableAliasName[TSDB_COL_NAME_LEN];
} STableNode;

typedef struct SRealTableNode {
  STableNode type; // QUERY_NODE_REAL_TABLE
  char dbName[TSDB_DB_NAME_LEN];
} SRealTableNode;

typedef struct STempTableNode {
  STableNode type; // QUERY_NODE_TEMP_TABLE
  SNode* pSubquery;
} STempTableNode;

typedef enum EJoinType {
  JOIN_TYPE_INNER = 1
} EJoinType;

typedef struct SJoinTableNode {
  STableNode type; // QUERY_NODE_JOIN_TABLE
  EJoinType joinType;
  SNode* pLeft;
  SNode* pRight;
  SNode* pOnCond;
} SJoinTableNode;

typedef enum EGroupingSetType {
  GP_TYPE_NORMAL = 1
} EGroupingSetType;

typedef struct SGroupingSetNode {
  ENodeType type; // QUERY_NODE_GROUPING_SET
  EGroupingSetType groupingSetType;
  SArray* pParameterList; 
} SGroupingSetNode;

typedef enum EOrder {
  ORDER_ASC = 1,
  ORDER_DESC
} EOrder;

typedef enum ENullOrder {
  NULL_ORDER_FIRST = 1,
  NULL_ORDER_LAST
} ENullOrder;

typedef struct SOrderByExprNode {
  ENodeType type; // QUERY_NODE_ORDER_BY_EXPR
  SNode* pExpr;
  EOrder order;
  ENullOrder nullOrder;
} SOrderByExprNode;

typedef struct SLimitInfo {
  uint64_t limit;
  uint64_t offset;
} SLimitInfo;

typedef struct SStateWindowNode {
  ENodeType type; // QUERY_NODE_STATE_WINDOW
  SNode* pCol;
} SStateWindowNode;

typedef struct SSessionWindowNode {
  ENodeType type; // QUERY_NODE_SESSION_WINDOW
  int64_t gap;             // gap between two session window(in microseconds)
  SNode* pCol;
} SSessionWindowNode;

typedef struct SIntervalWindowNode {
  ENodeType type; // QUERY_NODE_INTERVAL_WINDOW
  int64_t interval;
  int64_t sliding;
  int64_t offset;
} SIntervalWindowNode;

typedef struct SSelectStmt {
  ENodeType type; // QUERY_NODE_SELECT_STMT
  bool isDistinct;
  SArray* pProjectionList; // SNode
  SNode* pFromTable;
  SNode* pWhereCond;
  SArray* pPartitionByList; // SNode
  SNode* pWindowClause;
  SArray* pGroupByList; // SGroupingSetNode
  SArray* pOrderByList; // SOrderByExprNode
  SLimitInfo limit;
  SLimitInfo slimit;
} SSelectStmt;

typedef enum ESetOperatorType {
  SET_OP_TYPE_UNION_ALL = 1
} ESetOperatorType;

typedef struct SSetOperator {
  ENodeType type; // QUERY_NODE_SET_OPERATOR
  ESetOperatorType opType;
  SNode* pLeft;
  SNode* pRight;
} SSetOperator;

typedef bool (*FQueryNodeWalker)(SNode* pNode, void* pContext);

bool nodeArrayWalker(SArray* pArray, FQueryNodeWalker walker, void* pContext);
bool nodeTreeWalker(SNode* pNode, FQueryNodeWalker walker, void* pContext);

bool stmtWalker(SNode* pNode, FQueryNodeWalker walker, void* pContext);

bool nodeEqual(const SNode* a, const SNode* b);

void cloneNode(const SNode* pNode);

int32_t nodeToString(const SNode* pNode, char** pStr, int32_t* pLen);
int32_t stringToNode(const char* pStr, SNode** pNode);

bool isTimeorderQuery(const SNode* pQuery);
bool isTimelineQuery(const SNode* pQuery);

#ifdef __cplusplus
}
#endif

#endif /*_TD_NODES_H_*/