#pragma once
#include <map>

template <typename K, typename V>
class DMap : private std::map<K, V>
{
public:
	using std::map<K, V>::operator[];
	using std::map<K, V>::begin;
	using std::map<K, V>::end;

	DMap()
	{

	}

	DMap(std::map<K, V>&& Other)
	{
		*this = static_cast<DMap<K, V>>(std::move(Other));
	}

	V Get(K Key) const
	{
		return *const_cast<DMap<K, V>*>(this)->Find(Key);
	}

	V* Find(K Key)
	{
		if (!Contains(Key))
		{
			return nullptr;
		}
		return &std::map<K, V>::operator[](Key);
	}

	V& FindOrAdd(K Key)
	{
		return std::map<K, V>::operator[](Key);
	}

	bool Contains(K Key) const
	{
		return std::map<K, V>::contains(Key);
	}

	size_t Size() const
	{
		return std::map<K, V>::size();
	}

	bool Remove(K Key)
	{
		return std::map<K, V>::erase(Key);
	}
};