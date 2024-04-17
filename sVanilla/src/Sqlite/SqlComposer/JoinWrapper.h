#pragma once
#include "BaseInfo.h"

namespace SQLite
{

enum class JoinType
{
    INNER_JOIN,
    LEFT_JOIN,
    RIGHT_JOIN
};

struct JoinWrapper
{
    JoinType m_joinType;
    ColumnInfo m_leftCol;
    ColumnInfo m_rightCol;
};

}  // namespace SQLite