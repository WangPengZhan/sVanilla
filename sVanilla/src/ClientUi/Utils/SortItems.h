#pragma once
#undef max
#undef min

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
