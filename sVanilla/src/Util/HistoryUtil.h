#pragma once
#include <list>
#include <string>

class SearchHistory
{
public:
    explicit SearchHistory() = default;
    ~SearchHistory() = default;
    static SearchHistory& global();

    SearchHistory(const SearchHistory& other) = delete;
    SearchHistory& operator=(const SearchHistory& other) = delete;

    void append(const std::string& uri);
    [[nodiscard]] const std::list<std::string>& history() const;

private:
    std::list<std::string> m_history;
};
