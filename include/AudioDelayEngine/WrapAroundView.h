#pragma once

#include <ranges>
#include <cassert>

template<std::ranges::random_access_range Range>
class wrap_around  : public std::ranges::view_interface<wrap_around<Range>>
{
public:
    wrap_around(Range& data, size_t startIndex, size_t length)  : range(data), start(startIndex), len(length)
    {
        assert(start <= range.size());
    }

    struct iterator 
    {
        Range* data = nullptr;
        size_t index = 0;
        size_t length = 0;
        size_t pos = 0;

        using value_type = Range::value_type;
        using difference_type = std::ptrdiff_t;

        value_type& operator*() const
        {
            assert(data);

            size_t temp = index;

            while(temp >= data->size())
            {
                temp -= data->size();
            }

            return (*data)[temp];
        }

        iterator& operator++()
        {
            if(++pos == length)
            {
                data = nullptr;
            }

            ++index;

            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const
        {
            //Test whether both null
            if(data == nullptr && other.data == nullptr)
            {
                return true;
            }

            return data == other.data && index == other.index && length == other.length;
        }

        bool operator!=(const iterator& other) const
        {
            return !(*this == other);
        }
    };

    Range& range;
    size_t start;
    size_t len;

    iterator begin()
    {
        return iterator{&range, start, len};
    }

    iterator end()
    {
        return iterator{nullptr, start, len};
    }
};