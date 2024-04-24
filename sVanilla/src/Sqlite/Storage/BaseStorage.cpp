#include "BaseStorage.h"

namespace sqlite
{

const std::string& BaseStorage::tableName() const
{
    return m_tableName;
}

}  // namespace  sqlite
