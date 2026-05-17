#ifndef SORT_H
#define SORT_H

#include <algorithm>
#include <vector>

template <typename T, typename Compare>
void sort_vector(std::vector<T>& items, Compare comp)
{
    std::sort(items.begin(), items.end(), comp);
}

#endif
