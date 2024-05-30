#pragma once
#undef max
#undef min
#include <rapidfuzz/fuzz.hpp>
#include <vector>
#include <QString>

#include "ClientUi/VideoList/VideoGridWidget.h"

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
