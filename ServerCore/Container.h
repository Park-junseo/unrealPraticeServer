#pragma once
#include "Types.h"
#include "Allocator.h"

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
using namespace std;

template<typename Type, uint32 Size>
using xArray = array<Type, Size>;

template<typename Type>
using xVector = vector<Type, StlAllocator<Type>>;

template<typename Type>
using xList = list<Type, StlAllocator<Type>>;

template<typename Key, typename Type, typename Pred = less<Key>>
using xMap = map<Key, Type, Pred, StlAllocator<pair<const Key, Type>>>;

template<typename Key, typename Pred = less<Key>>
using xSet = set<Key, Pred, StlAllocator<Key>>;

template<typename Type>
using xDeque = deque<Type, StlAllocator<Type>>;

template<typename Type, typename Container = xDeque<Type>>
using xQueue = queue<Type, Container>;

template<typename Type, typename Container = xDeque<Type>>
using xStack = stack<Type, Container>;

template<typename Type, typename Container = xVector<Type>, typename Pred = less<typename Container::value_type>>
using xPriorityQueue = priority_queue<Type, Container, Pred>;

//using xString = basic_string<char, char_traits<char>, StlAllocator<char>>;
using xString = basic_string<wchar_t, char_traits<wchar_t>, StlAllocator<wchar_t>>;

template<typename Key, typename Type, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using xHashMap = unordered_map<Key, Type, Hasher, KeyEq, StlAllocator<pair<const Key, Type>>>;

template<typename Key,  typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using xHashSet = unordered_set<Key, Hasher, KeyEq, StlAllocator<Key>>;