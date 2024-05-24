
#include "HistoryUtil.h"

SearchHistory& SearchHistory::global()
{
    static SearchHistory history;
    return history;
}

void SearchHistory::append(const std::string& uri)
{
    m_history.push_back(uri);
}

const std::list<std::string>& SearchHistory::history() const
{
    return m_history;
}