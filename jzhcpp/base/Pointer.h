// Pointer.h
// Definition of the Ptr template class.
// Copyleft 2014 - 2018, jzh just do it


#ifndef BASE_POINTER_H
#define BASE_POINTER_H

#include "Basic.h"

namespace yep {

template<typename T>
class ReleasePtr {
public:
	static void release(T* p)
	{
		// <p can be nullptr>
		delete p;
	}
};

template<typename T>
class ReleaseArrayPtr {
public:
	static void release(T* p)
	{
		// <p can be nullptr>
		delete[] p;
	}
};

template<typename T, typename R = ReleasePtr<T> >
class Ptr {
	template<typename B, typename BR> friend class Ptr; // 父子类调用

	T* ptr_;
	volatile uint32* count_;

private:
	inline void increase()
	{
		if (count_) {
			INCRC(count_);
		}
	}

	inline void decrease()
	{
		if (count_) {
			if (DECRC(count_) == 0) {
				R::release(ptr_);
				ptr_ = nullptr;
				delete count_;
				count_ = nullptr;
			}
		}
	}

	// <for cast. note: volatile>
	Ptr(T* p, volatile uint32* c): ptr_(p), count_(c)
	{
		if (ptr_) {
			increase();
		}
	}

	Ptr& assign(T* p)
	{
		if (ptr_ != p) {
			// <swap temp call ~Ptr release this>
			Ptr temp(p); 
			swap(temp);
		}
		return *this;
	}

	Ptr& assign(const Ptr& p) {
		if (this != &p) {
			Ptr temp(p);
			swap(temp);
		}
		return *this;
	}

	template<typename C, typename CR>
	Ptr& assign(const Ptr<C, CR>& p) {
		if (ptr_ != p.ptr_) {
			Ptr temp(p);
			swap(temp);
		}
		return *this;
	}


public:
	~Ptr()
	{
		decrease();
	}

	Ptr() 
		: ptr_(nullptr)
		, count_(nullptr)
	{
	}

	Ptr(T* p)
	{
		if (p) {
			ptr_ = p;
			count_ = new uint32(1);
		}
	}

	Ptr(const Ptr<T, R>& p)
		: ptr_(p.ptr_)
		, count_(p.count_)
	{
		increase();
	}

	Ptr(Ptr<T, R>&& p)
		: ptr_(p.ptr_)
		, count_(p.count_)
	{
		p.count_ = nullptr;
		p.ptr_ = nullptr;
	}

	template<typename C, typename CR>
	Ptr(const Ptr<C, CR>& p)
		: ptr_(nullptr)
		, count_(nullptr)
	{
		T* converted = p.ptr();
		if (converted)
		{
			ptr_ = p.ptr_;
			count_ = p.count_;
			increase();;
		}

	}

	Ptr<T, R>& operator=(T* p)
	{
		return assign(p);
	}

	Ptr& operator=(const Ptr& p) 
	{
		return assign(p);
	}

	Ptr& operator=(Ptr<T, R>&&p)
	{
		if (this != &p) {
			decrease();
			this->count_ = p.count_;
			this->ptr_ = p.ptr_;
			p.count_ = nullptr;
			p.ptr_ = nullptr;
		}
		return *this;
	}

	template<typename C, typename CR>
	Ptr& operator=(const Ptr<C, CR>& p)
	{
		return assign(p);
		/*
		// <implicit cast>
		T* canconvert = p.ptr();
		decrease();
		if (canconvert) {
			this->ptr_ = p.ptr_;
			this->count_ = p.count_;
			increase();
		} else {
			this->ptr_ = nullptr;
			this->count_ = nullptr;
		}
		return *this;
		*/
	}

	template<typename C>
	Ptr<C, R> Cast()const
	{
		C* converted = dynamic_cast<C*>(ptr_);

		if (converted) {
			return Ptr<C, R>(converted, count_);;
		}
		return Ptr<C,R>(nullptr, nullptr);
	}

public:
	T * operator->()const
	{
		return ptr_;
	}

	T* ptr()const
	{
		return ptr_;
	}

	uint32 refcount()const
	{
		if (count_) {
			return *count_;
		}
		return 0;
	}

	void swap(Ptr& p)
	{
		std::swap(ptr_, p.ptr_);
		std::swap(count_, p.count_);
	}

	void reset() {
		assign(0);
	}

	void reset(T* p) {
		assign(p);
	}

	void reset(const Ptr& p)
	{
		assign(p);
	}

	template<typename C, typename CR>
	void reset(const Ptr<C, CR>& p)
	{
		assign<C, CR>(p);
	}

	operator bool()const
	{
		return ptr_ != nullptr;
	}

	bool operator==(const T* p) const
	{
		return ptr_ == p;
	}

	bool operator==(const Ptr& p)const
	{
		return ptr_ == p.ptr_;
	}

	bool operator!=(const T* p) const
	{
		return ptr_ != p;
	}

	bool operator!=(const Ptr& p)const
	{
		return ptr_ != p.ptr_;
	}

	bool operator>(const T* p) const
	{
		return ptr_ > p;
	}

	bool operator>(const Ptr& p)const
	{
		return ptr_ > p.ptr_;
	}

	bool operator>=(const T* p) const
	{
		return ptr_ >= p;
	}

	bool operator>=(const Ptr& p)const
	{
		return ptr_ >= p.ptr_;
	}

	bool operator<(const T* p) const
	{
		return ptr_ < p;
	}

	bool operator<(const Ptr& p)const
	{
		return ptr_ < p.ptr_;
	}

	bool operator<=(const T* p) const
	{
		return ptr_ <= p;
	}

	bool operator<=(const Ptr& p)const
	{
		return ptr_ <= p.ptr_;
	}
};

} // namespace yep

#endif // !BASE_POINTER_H

