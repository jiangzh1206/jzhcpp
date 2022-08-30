#pragma once
#include <memory>

template <typename T> 
class CowPtr {
public:
	using RefPtr = std::shared_ptr<T>;

	CowPtr() :mPtr(nullptr) {}
	~CowPtr(){}
	CowPtr(T* other):mPtr(other){}
	CowPtr(const CowPtr<T>& other):mPtr(other.mPtr){}
	CowPtr<T>& CowPtr(const CowPtr<T>& other) {
		if (other.mPtr != mPtr) {
			mPtr = other.mPtr;
		}
		return *this;
	}
	CowPtr<T>& CowPtr(T* other) {
		mPtr = RefPtr(other);
		
		return *this;
	}

	T& operator*() {
		detach();
		return *mPtr;
	}
	const T& operator*()const {
		return *mPtr
	}
	T* operator->() {
		detach();
		return *mPtr;
	}
	const T* operator->()const {
		detach();
		return *mPtr;
	}

	bool operator==(const CowPtr<T>& other)const {
		return mPtr.get() == other.mPtr.get();
	}
	bool operator!=(const CowPtr<T>& other)const {
		return mPtr.get() != other.mPtr.get();
	}
	bool operator !()const {
		return !mPtr.get();
	}

	operator T* () {
		return mPtr.get();
	}
	operator const T* ()const {
		return mPtr.get();
	}

	T* data() {
		detach();
		return mPtr.get();
	}
	const T* data()const {
		detach();
		return mPtr.get();
	}
	const T* constData()const {
		detach();
		return mPtr.get();
	}
private:
	RefPtr mPtr;
};