#pragma once
namespace Ivy::U
{
template <typename T, int N>
constexpr void Swap(std::array<T, N>& arr, size_t a, size_t b) requires std::totally_ordered<T>
{
    T value = arr[a];
    arr[a] = arr[b];
    arr[b] = value;
}

template <typename T, size_t N>
constexpr std::array<T, N> BubbleSort(const std::array<T, N>& arr) requires std::totally_ordered<T>
{
    std::array<T, N> result = arr;

    for (size_t i = 0; i < N; ++i)
    {
        bool swapped = false;

        for (size_t j = 0; j + 1 < N - i; ++j)
        {
            if (result[j] > result[j + 1])
            {
                Swap(result, j, j + 1);
                swapped = true;
            }
        }

        if (!swapped)
        {
            break;
        }
    }

    return result;
}

template <typename T, size_t N>
constexpr std::array<T, N> InsertionSort(const std::array<T, N>& arr) requires std::totally_ordered<T>
{
    std::array<T, N> result = arr;

    for (size_t i = 1; i < N; ++i)
    {
        T value = result[i];
        size_t j = i;

        while (j > 0 && result[j - 1] > value)
        {
            result[j] = result[j - 1];
            --j;
        }

        result[j] = value;
    }

    return result;
}

template <typename T, size_t N>
constexpr std::array<T, N> SelectionSort(const std::array<T, N>& arr) requires std::totally_ordered<T>
{
    std::array<T, N> result = arr;

    for (size_t i = 0; i < N; ++i)
    {
        size_t minIndex = i;

        for (size_t j = i + 1; j < N; ++j)
        {
            if (result[j] < result[minIndex])
            {
                minIndex = j;
            }
        }

        if (minIndex != i)
        {
            Swap(result, i, minIndex);
        }
    }

    return result;
}
template <typename T, size_t N, size_t M>
constexpr std::array<T, N + M> MergeSort(const std::array<T, N>& left, const std::array<T, M>& right)
    requires std::totally_ordered<T>
{
    std::array<T, N + M> result{};
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;

    while (i < N && j < M)
    {
        if (left[i] < right[j])
        {
            result[k++] = left[i++];
        }
        else
        {
            result[k++] = right[j++];
        }
    }

    while (i < N)
    {
        result[k++] = left[i++];
    }

    while (j < M)
    {
        result[k++] = right[j++];
    }

    return result;
}
} // namespace Ivy::U
