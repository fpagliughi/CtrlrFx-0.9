/// @file KeyArray.h
///
/// Definition of the CtrlrFx KeyArray class.
///
/// @author Frank Pagliughi
/// @author 

#ifndef __CtrlrFx_KeyArray_h
#define __CtrlrFx_KeyArray_h

#include "CtrlrFx/Array.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A key array allows a key to be associated with each entry into the array.
/// Items in the array can be located by a search for the associated key.
/// This can be used like an std::map for systems that don't use dynamic 
/// memory.

template <typename KT, typename T>
class KeyArray
{
public:
	typedef KT key_t;	///< The key

	static const key_t EMPTY_SLOT = key_t();

private:
	struct KeyPair { key_t key; T val; };
	Array<KeyPair>	arr_;

public:
	//KeyArray();

	/// Constructs a KeyArray with the specified capacity.
	/// The memory for the array is allocated off the heap.
	explicit KeyArray(size_t cap);

	/// Constructs a KeyArray using the memory provided.
	KeyArray(T* arr, size_t cap, bool own=false);

	/// Gets the maximum number of items the array can hold.
	//size_t capacity() const { return arr_.capacity(); }

	/// Gets the number of elements in the array.
	/// @note This is the number of entries that the array can hold, not
	/// the number of available slots.
	size_t size() const { return arr_.size(); }

	/// Determines if the slot at the specified index is empty.
	bool is_empty_at(int i) const;

	/// Gets the index of the entry that has the specified key.
	/// @param key The key to search for.
	/// @return
	/// @li The index (>= 0) for the specified key, if found.
	/// @li -1 if the key is not found
	int find(key_t key) const;

	/// Place the value into the collection, using the specified key.
	/// @param key The key used to identify the entry
	/// @param val The value to store
	int put(key_t key, const T& val);

	/// Gets the value at the specified index and removes it from the array.
	/// @param i Index of the value to retrieve
	/// @param val Pointer to the memory to receive the value
	/// @return
	/// @li @em true If the value was found and retrieved.
	/// @li @em false If the value was not found.
	bool get_at(int i, T* val);

	/// Gets the value corresponding to the the specified key and removes it
	/// from the array.
	/// @param key The key to search.
	/// @param val Pointer to the memory to receive the value.
	/// @return
	/// @li @em true If the value was found and retrieved.
	/// @li @em false If the value was not found.
	bool get(key_t key, T* val);

	/// Removes the entry at the specified index.
	/// It makes the key at the specified index @em EMPTY_SLOT.
	/// @return
	/// @li @em true If the value was found and deleted.
	/// @li @em false If the value was not found.
	bool remove_at(int i);

	/// Removes the entry that has the specified key.
	/// It makes the key at the specified index @em EMPTY_SLOT.
	/// @return
	/// @li @em true If the value was found and deleted.
	/// @li @em false If the value was not found.
	bool remove(key_t key);

	/// Removes all the entries from the array.
	/// It marks all entries as empty.
	void clear();

	/// Gets a variable reference to the value at the specified index.
	/// This allows the entry to be updated in place.
	T& operator[](int i) { return arr_[i].val; }

	/// Gets a constant reference to the value at the specified index.
	const T& operator[](int i) const { return arr_[i].val; }
};

// --------------------------------------------------------------------------

/*
template <typename KT, typename T>
KeyArray<KT,T>::KeyArray()
{
	for (size_t i=0; i<size(); ++i)
		arr_[i].key = EMPTY_SLOT;
}
*/

template <typename KT, typename T>
KeyArray<KT,T>::KeyArray(size_t cap) : arr_(cap)
{
	clear();
}

template <typename KT, typename T>
KeyArray<KT,T>::KeyArray(T* arr, size_t cap, bool own /*=false*/) 
					: arr_(arr, cap, own)
{
	clear();
}

template <typename KT, typename T>
inline bool KeyArray<KT,T>::is_empty_at(int i) const
{
	if (i < 0 || size_t(i) >= size())
		return false;
	return arr_[i].key == EMPTY_SLOT;
}

template <typename KT, typename T>
int KeyArray<KT,T>::find(key_t key) const
{
	for (size_t i=0; i<size(); ++i) {
		if (arr_[i].key == key)
			return (int) i;
	}

	return -1;
}

template <typename KT, typename T>
int KeyArray<KT,T>::put(key_t key, const T& val)
{
	int i;

	if ((i = find(EMPTY_SLOT)) >= 0) {
		arr_[i].key = key;
		arr_[i].val = val;
	}
	return i;
}

template <typename KT, typename T>
bool KeyArray<KT,T>::get_at(int i, T* val)
{
	if (i >= 0) {
		*val = arr_[i].val;
		arr_[i].key = EMPTY_SLOT;
		return true;
	}

	return false;
}

template <typename KT, typename T>
inline bool KeyArray<KT,T>::get(key_t key, T* val)
{
	return get_at(find(key), val);
}

template <typename KT, typename T>
bool KeyArray<KT,T>::remove_at(int i)
{
	if (i >= 0 && size_t(i) < arr_.size()) {
		arr_[i].key = EMPTY_SLOT;
		return true;
	}
	return false;
}

template <typename KT, typename T>
bool KeyArray<KT,T>::remove(key_t key)
{
	return remove_at(find(key));
}

template <typename KT, typename T>
void KeyArray<KT,T>::clear()
{
	for (size_t i=0; i<arr_.size(); ++i)
		arr_[i].key = EMPTY_SLOT;
}


/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_KeyArray_h

