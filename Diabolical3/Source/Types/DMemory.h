#pragma once
#include <memory>

template <typename T>
class DWeakPtr;

template <typename T>
class DSharedPtr : private std::shared_ptr<T>
{
public:
	using std::shared_ptr<T>::operator[];
	using std::shared_ptr<T>::operator->;

	DSharedPtr() {}

	DSharedPtr(T* In) : std::shared_ptr<T>(In) {}

	DSharedPtr(std::shared_ptr<T> Other) : std::shared_ptr<T>(Other) {}

	// constructor to allow casting of the inner type
	template <typename T2>
	DSharedPtr(DSharedPtr<T2>&& Other) : std::shared_ptr<T>::shared_ptr(static_pointer_cast<T>(Other.AsSTLType())) {}

	template <typename T2>
	DSharedPtr(const DSharedPtr<T2>& Other) : std::shared_ptr<T>::shared_ptr(Other.AsSTLType()) {}

	template <typename T2>
	DSharedPtr(const DSharedPtr<T2>* Other) : std::shared_ptr<T>::shared_ptr(Other) {}

	inline std::shared_ptr<T>& AsSTLType()
	{
		static_assert(sizeof(DSharedPtr<T>) == sizeof(std::shared_ptr<T>));
		return *(std::shared_ptr<T>*)this;
	}

	inline DWeakPtr<T> Get()
	{
		return DWeakPtr<T>(*this);
	}

	inline T* GetRaw() const 
	{
		return std::shared_ptr<T>::get();
	}

	inline bool IsValid() const
	{
		return const_cast<DSharedPtr<T>*>(this)->Get().IsValid();
	}

};

template <typename T>
class DWeakPtr : private std::weak_ptr<T>
{
public:
	DWeakPtr() {};

	DWeakPtr(DSharedPtr<T>& Shared) : std::weak_ptr<T>::weak_ptr(Shared.AsSTLType()) {}

	template <typename T2>
	DWeakPtr(DWeakPtr<T2>&& Other) : std::weak_ptr<T>::weak_ptr(DSharedPtr<T>(std::move(Other.Lock())).Get()) {}

	template <typename T2>
	DWeakPtr(DWeakPtr<T2>& Other) : std::weak_ptr<T>::weak_ptr(DSharedPtr<T>(Other.Lock()).Get()) {}

	inline bool Expired() const 
	{
		return std::weak_ptr<T>::expired();
	}

	inline DSharedPtr<T> Lock()
	{
		return std::weak_ptr<T>::lock();
	}

	inline bool IsValid() const
	{
		return std::weak_ptr<T>::get() != nullptr;
	}

	//inline T* Get() const
	//{
	//	return std::weak_ptr<T>::get();
	//}
};

template <typename T>
class DUniquePtr : private std::unique_ptr<T>
{
public:
	using std::unique_ptr<T>::operator->;

	DUniquePtr(std::unique_ptr<T> In) : std::unique_ptr<T>(In.release()) {}

	DUniquePtr(T* In) : std::unique_ptr<T>(In) {}

	DUniquePtr() {}

	T* Get() const 
	{
		return std::unique_ptr<T>::get();
	}

	inline void Swap(DUniquePtr& Other)
	{
		std::unique_ptr<T>::swap(Other);
	}

	inline T* Release()
	{
		return std::unique_ptr<T>::release();
	}
};

class DObjectPtrBase
{
public:
	virtual ~DObjectPtrBase() {}

	bool Serialize(class DBitStream& BitStream);
	bool Deserialize(class DBitStream& BitStream);

	virtual void CopyTo(void* Location) = 0;

	bool GetNetworkId(uint64_t OutNetworkId) const
	{
		if (bHasNetworkId)
		{
			OutNetworkId = NetworkId;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool SetFromNetworkId();

protected:
	virtual void* GetPtrInternal() = 0;

	// Need to store the network id, in case a networked object is deserialized but it's not relevant yet.
	// If it becomes relevant, we can search the object and cache it using this Id.
	uint64_t NetworkId = 0;
	bool bHasNetworkId;
};

// A weak ptr that can be dereferenced without locking.
// It will not be cleaned up during a frame.
template <typename T>
class DObjectPtr : private std::weak_ptr<T>, public DObjectPtrBase
{
public:
	DObjectPtr() {};

	DObjectPtr(DSharedPtr<T>& Shared) : std::weak_ptr<T>::weak_ptr(Shared.AsSTLType()) {}

	template <typename T2>
	DObjectPtr(DObjectPtr<T2>&& Other)
	{
		*this = DObjectPtr<T>(Other.Lock().Get());
	}

	template <typename T2>
	DObjectPtr(DWeakPtr<T2>&& Other)
		: std::weak_ptr<T>::weak_ptr(DSharedPtr<T>(std::move(Other.Lock())).AsSTLType())
	{}

	template <typename T2>
	DObjectPtr(DObjectPtr<T2>& Other)
	{
		*this = DObjectPtr<T>(Other.Lock().Get());
	}

	template <typename T2>
	DObjectPtr(DWeakPtr<T2>& Other)
		: std::weak_ptr<T>::weak_ptr(DSharedPtr<T>(Other.Lock()).AsSTLType())
	{}

	inline bool Expired() const
	{
		return std::weak_ptr<T>::expired();
	}

	inline bool IsValid() const
	{
		return Get() != nullptr;
	}

	inline T* Get() const
	{
		T* Value = std::weak_ptr<T>::get();
		if (Value)
		{
			return Value;
		}

		uint64_t LocalNetworkId = 0;
		if (GetNetworkId(LocalNetworkId))
		{
			const_cast<DObjectPtr<T>*>(this)->SetFromNetworkId();
			return std::weak_ptr<T>::get();
		}

		return nullptr;
	}

	T* operator->()
	{
		return Get();
	}

	const T* operator->() const
	{
		return Get();
	}

	void Reset()
	{
		std::weak_ptr<T>::reset();
	}

	inline DSharedPtr<T> Lock()
	{
		return std::weak_ptr<T>::lock();
	}

	virtual void CopyTo(void* Location) override
	{
		*(DObjectPtr<T>*)Location = *this;
	}

protected:
	virtual void* GetPtrInternal() override
	{
		return (void*)Get();
	}

};