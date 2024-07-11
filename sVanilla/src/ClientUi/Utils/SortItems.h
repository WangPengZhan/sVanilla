#pragma once
#undef max
#undef min
#include <rapidfuzz/fuzz.hpp>
#include <vector>
#include <QString>

template <typename Iterable, typename QueryType, typename Sentence = typename Iterable::value_type>
void sortInitialsItems(Iterable& items, const std::function<QueryType(Sentence)>& dataExtractor, Qt::SortOrder order)
{
    std::sort(items.begin(), items.end(), [dataExtractor, order](const Sentence& a, const Sentence& b) {
        const auto& beginText = dataExtractor(a);
        const auto& endText = dataExtractor(b);
        switch (order)
        {
        case Qt::AscendingOrder:
        {
            return endText > beginText;
        }
        case Qt::DescendingOrder:
        {
            return beginText > endText;
        }
        default:
            return false;
        }
    });
}

template <typename Query, typename Iterable, typename Sentence = typename Iterable::value_type>
Iterable sortSimilarItems(const Iterable& items, const Query& lookupQuery, const std::function<Query(Sentence)>& dataExtractor,
                          const double similarityThreshold = 0.0)
{
    rapidfuzz::fuzz::CachedRatio<typename Query::value_type> similarityScorer(lookupQuery);
    std::vector<float> scoreList(items.size());

    for (int i = 0; i < items.size(); ++i)
    {
        const auto& item = items.at(i);
        const auto& text = dataExtractor(item);
        scoreList[i] = similarityScorer.similarity(text, similarityThreshold);
    }

    std::vector<std::size_t> rankIndices(items.size());
    std::iota(rankIndices.begin(), rankIndices.end(), 0);

    std::sort(rankIndices.begin(), rankIndices.end(), [&scoreList, &similarityThreshold](std::size_t a, std::size_t b) {
        return (scoreList[a] >= similarityThreshold) && (scoreList[a] > scoreList[b]);
    });

    Iterable sortedItems(items.size());
    for (std::size_t i = 0; i < rankIndices.size(); ++i)
    {
        sortedItems[i] = items[rankIndices[i]];
    }

    return sortedItems;
}
