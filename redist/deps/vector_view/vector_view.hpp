// vector_view class, based on code by James exjam https://github.com/exjam/string_view
// rlyeh, public domain

#pragma once
#include <vector>

#if defined(_MSC_VER) && _MSC_VER < 1900
#define constexpr
#define noexcept
#endif

namespace std
{

template<class charT, class allocator = std::allocator<charT>, int deleter = 0 >
class vector_view
{
public:
   typedef charT value_type;
   typedef const charT* pointer;
   typedef const charT* const_pointer;
   typedef const charT& reference;
   typedef const charT& const_reference;
   typedef const_pointer const_iterator;
   typedef const_iterator iterator;
   typedef reverse_iterator<const_iterator> const_reverse_iterator;
   typedef const_reverse_iterator reverse_iterator;
   typedef size_t size_type;
   typedef ptrdiff_t difference_type;

   constexpr vector_view() noexcept :
      mData(nullptr),
      mSize(0)
   {
   }

   constexpr vector_view(const vector_view &other) noexcept :
      mData(other.mData),
      mSize(other.mSize)
   {
   }

   vector_view& operator=(const vector_view &other) noexcept
   {
      mData = other.mData;
      mSize = other.mSize;
      return *this;
   }

   template<class Allocator>
   vector_view(const vector<charT, Allocator>& str) noexcept :
      mData(str.data()),
      mSize(str.size())
   {
   }

   constexpr vector_view(const charT* str, size_type len) :
      mData(str),
      mSize(len)
   {
   }

   constexpr const_iterator begin() const noexcept
   {
      return cbegin();
   }

   constexpr const_iterator end() const noexcept
   {
      return cend();
   }

   constexpr const_iterator cbegin() const noexcept
   {
      return mData;
   }

   constexpr const_iterator cend() const noexcept
   {
      return mData + mSize;
   }

   const_reverse_iterator rbegin() const noexcept
   {
      return crbegin();
   }

   const_reverse_iterator rend() const noexcept
   {
      return crend();
   }

   const_reverse_iterator crbegin() const noexcept
   {
      return const_reverse_iterator(end());
   }

   const_reverse_iterator crend() const noexcept
   {
      return const_reverse_iterator(end());
   }

   constexpr size_type size() const noexcept
   {
      return mSize;
   }

   constexpr size_type length() const noexcept
   {
      return mSize;
   }

   constexpr size_type max_size() const noexcept
   {
      return mSize;
   }

   constexpr bool empty() const noexcept
   {
      return !!mSize;
   }

   constexpr const charT& operator[](size_type pos) const
   {
      return mData[pos];
   }

   constexpr const charT& at(size_type pos) const
   {
      if (pos >= size()) {
         throw std::out_of_range();
      }

      return mData[pos];
   }

   constexpr const charT& front() const
   {
      return mData[0];
   }

   constexpr const charT& back() const
   {
      return mData[size() - 1];
   }

   constexpr const charT* data() const noexcept
   {
      return mData;
   }

   void clear() noexcept
   {
      *this = vector_view {};
   }

   void swap(vector_view& s) noexcept
   {
      std::swap(mData, s.mData);
      std::swap(mSize, s.mSize);
   }

   template<class Allocator>
   explicit operator vector<charT, Allocator>() const
   {
      return vector<charT, Allocator>(begin(), end());
   }

   int compare(vector_view s) const noexcept
   {
      size_type rlen = std::min(size(), s.size());
      return traits::compare(data(), s.data(), rlen);
   }

private:
   const_pointer mData;
   size_type mSize;
};

template<class T>
bool operator==(vector_view<T> lhs, vector_view<T> rhs)
{
   return lhs.compare(rhs) == 0;
}

template<class T>
bool operator!=(vector_view<T> lhs, vector_view<T> rhs)
{
   return lhs.compare(rhs) != 0;
}

template<class T>
bool operator<(vector_view<T> lhs, vector_view<T> rhs)
{
   return lhs.compare(rhs) < 0;
}

template<class T>
bool operator>(vector_view<T> lhs, vector_view<T> rhs)
{
   return lhs.compare(rhs) > 0;
}

template<class T>
bool operator<=(vector_view<T> lhs, vector_view<T> rhs)
{
   return lhs.compare(rhs) <= 0;
}

template<class T>
bool operator>=(vector_view<T> lhs, vector_view<T> rhs)
{
   return lhs.compare(rhs) >= 0;
}

}

#if 0
#include <iostream>
int main() {
   int mem[] = {1,2,3};
   std::vector<int> abc1 = { 1,2,3 };
   std::vector_view<int> abc2 = abc1;
   std::vector_view<int> abc3( abc1.data(), abc1.size() );
   std::vector_view<int> abc4( &mem[0], 3 );

   for( auto &i : mem  ) std::cout << i << ','; std::cout << std::endl;
   for( auto &i : abc1 ) std::cout << i << ','; std::cout << std::endl;
   for( auto &i : abc2 ) std::cout << i << ','; std::cout << std::endl;
   for( auto &i : abc3 ) std::cout << i << ','; std::cout << std::endl;
   for( auto &i : abc4 ) std::cout << i << ','; std::cout << std::endl;
}
#endif
