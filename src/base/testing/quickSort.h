#pragma once

#include "base/dependencies.h"

namespace dory::testing
{
    template<typename T>
    std::list<T> quickSort(std::list<T> collection)
    {
        if(collection.empty())
        {
            return collection;
        }

        auto collectionSorted = std::list<T>{};
        collectionSorted.splice(collectionSorted.begin(), collection, collection.begin());

        const auto& pivotValue = *collectionSorted.begin();

        auto partitionRange = std::ranges::partition(collection, [&pivotValue](const T& item)
        {
            return item < pivotValue;
        });

        auto lowerHalfCollection = std::list<T>{};
        lowerHalfCollection.splice(lowerHalfCollection.end(), collection, collection.begin(), partitionRange.begin());

        auto lowerHalfCollectionSorted = quickSort(std::move(lowerHalfCollection));
        auto higherHalffCollectionSorted = quickSort(std::move(collection));

        collectionSorted.splice(collectionSorted.end(), higherHalffCollectionSorted);
        collectionSorted.splice(collectionSorted.begin(), lowerHalfCollectionSorted);

        return collectionSorted;
    }

    template<typename T>
    std::list<T> quickSortParallel(std::list<T> collection)
    {
        if(collection.empty())
        {
            return collection;
        }

        auto collectionSorted = std::list<T>{};
        collectionSorted.splice(collectionSorted.begin(), collection, collection.begin());

        const auto& pivotValue = *collectionSorted.begin();

        auto partitionRange = std::ranges::partition(collection, [&pivotValue](const T& item)
        {
            return item < pivotValue;
        });

        auto lowerHalfCollection = std::list<T>{};
        lowerHalfCollection.splice(lowerHalfCollection.end(), collection, collection.begin(), partitionRange.begin());

        auto lowerHalfFuture = std::async(&quickSort<T>, std::move(lowerHalfCollection));
        auto higherHalfCollectionSorted = quickSort(std::move(collection));

        collectionSorted.splice(collectionSorted.end(), higherHalfCollectionSorted);
        collectionSorted.splice(collectionSorted.begin(), lowerHalfFuture.get());

        return collectionSorted;
    }
}