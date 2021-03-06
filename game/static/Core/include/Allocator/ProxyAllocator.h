#pragma once

#include <Allocator/Allocator.h>

#include <Allocator/Policy/BoundCheckingPolicy.h>
#include <Allocator/Policy/MemoryTaggingPolicy.h>
#include <Allocator/Policy/MemoryTrackingPolicy.h>
#include <Allocator/Policy/SyncPolicy.h>

#include <Allocator/NamedAllocator.h>

BEGINNAMESPACE

namespace detail {
	
	template<
		typename AllocationPolicy,
		typename SyncPolicy,
		typename BoundCheckingPolicy,
		typename MemoryTrackingPolicy,
		typename MemoryTaggingPolicy
	>
	class _ProxyAllocator : public NamedAllocator{
	public:
		typedef AllocationPolicy Alloc;
		typedef SyncPolicy Sync;
		typedef BoundCheckingPolicy BoundsChecking;
		typedef MemoryTrackingPolicy MemoryTracking;
		typedef MemoryTaggingPolicy MemoryTagging;
	public:
		static const size_type Overhead = BoundsChecking::SizeFront() + BoundsChecking::SizeBack();
		static const size_type Offset = BoundsChecking::SizeFront();

	public:
		_ProxyAllocator(const ansichar* name) :
			NamedAllocator(name), 
			m_Synchronisation(this),
			m_BoundChecker(this), 
			m_MemoryTracker(this),
			m_MemoryTagger(this)
		{}

		inline _ProxyAllocator(_ProxyAllocator&& other) {
			*this = std::forward<_ProxyAllocator>(other);
		}

		inline _ProxyAllocator& operator = (_ProxyAllocator&& rhs) {
			if (this != &rhs) {
				m_Synchronisation = rhs.m_Synchronisation;
				m_BoundChecker = rhs.m_BoundChecker;
				m_MemoryTracker = rhs.m_MemoryTracker;
				m_MemoryTagger = rhs.m_MemoryTagger;
			}
			return *this;
		}

		inline void* allocate(Alloc& alloc, size_type size, size_type alignment) {
			m_Synchronisation.lock();

			const size_type newSize = size + BoundsChecking::SizeFront() + BoundsChecking::SizeBack();

			Byte* plainMemory = static_cast<Byte*>(alloc.allocate(newSize, alignment, BoundsChecking::SizeFront()));

			m_BoundChecker.front(plainMemory, size); //Write a BoundTracker - front
			m_MemoryTagger.tag(plainMemory + BoundsChecking::SizeFront(), size); //Tag the memory allocation
			m_BoundChecker.back(plainMemory + BoundsChecking::SizeFront() + size); //Write a BoundTracker - back

			m_MemoryTracker.onAllocation(plainMemory, newSize, alignment); //Track the memory allocation to be able to find memory leaks

			m_Synchronisation.unlock();

			return static_cast<void*>(plainMemory + BoundsChecking::SizeFront());
		}

		inline void free(Alloc& alloc, void* ptr) {
			m_Synchronisation.lock();

			Byte* originalMemory = static_cast<Byte*>(ptr) - BoundsChecking::SizeFront();

			size_type memorySize = m_BoundChecker.check(originalMemory);
			m_MemoryTracker.onFree(originalMemory, memorySize + Overhead);
			m_MemoryTagger.untag(originalMemory, memorySize);
			alloc.free(originalMemory);

			m_Synchronisation.unlock();
		}
	protected:
		SyncPolicy m_Synchronisation; //Syncronizes all calles to this proxy
		BoundsChecking m_BoundChecker; //Checks if we write out of bounds 
		MemoryTracking m_MemoryTracker; //Tracks all allocation/free calls
		MemoryTagging m_MemoryTagger; //Taggs memory 
	};
}

template<
	typename AllocationPolicy,
	typename SyncPolicy,
	typename BoundCheckingPolicy,
	typename MemoryTrackingPolicy,
	typename MemoryTaggingPolicy
>
class ProxyAllocator : public detail::_ProxyAllocator<AllocationPolicy, SyncPolicy, BoundCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy> {
	typedef detail::_ProxyAllocator<AllocationPolicy, SyncPolicy, BoundCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy> Parent;

    static const size_type Offset = detail::_ProxyAllocator<AllocationPolicy, SyncPolicy, BoundCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>::Offset;
public:
	inline ProxyAllocator(const char* allocatorName) : Parent(allocatorName), m_Allocator() {}

	inline ProxyAllocator(const char* allocatorName, size_type size) : Parent(allocatorName), m_Allocator(size) {};

	inline ProxyAllocator(ProxyAllocator&& other) {
		*this = std::forward(other);
	}

	inline ProxyAllocator& operator = (ProxyAllocator&& rhs) {
		if (this != &rhs) {
			Parent::operator = (*this);
			m_Allocator = std::move(rhs);
		}
		return *this;
	}

	inline void initialize() {
		m_Allocator.initialize();
	}

	inline void initialize(size_type size) {
		m_Allocator.initialize(size);
	}

	inline void initialize(void* start, size_type size) {
		m_Allocator.initialize(start, size);
	}

	inline void initialize(const void* start, const void* end) {
		m_Allocator.initialize(start, end);
	}

	inline void* allocate(size_type size, size_type alignment) {
		return Parent::allocate(m_Allocator, size, alignment);
	}

	inline void free(void* ptr) {
		return Parent::free(m_Allocator, ptr);
	}

	inline void* getStart() const { return m_Allocator.getStart(); }
	inline void* getEnd() const { return m_Allocator.getEnd(); }
	inline size_type getSize() const { return m_Allocator.getSize(); }
private:
	AllocationPolicy m_Allocator;
};

template<
	typename SyncPolicy,
	typename BoundCheckingPolicy,
	typename MemoryTrackingPolicy,
	typename MemoryTaggingPolicy
>
class ProxyAllocator<PoolAllocator, SyncPolicy, BoundCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy> : public detail::_ProxyAllocator<PoolAllocator, SyncPolicy, BoundCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy> {
	typedef detail::_ProxyAllocator<PoolAllocator, SyncPolicy, BoundCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy> Parent;
    static const size_type Offset = detail::_ProxyAllocator<PoolAllocator, SyncPolicy, BoundCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>::Offset;
public:
	inline ProxyAllocator(const ansichar* allocatorName) : 
		Parent(allocatorName),
		m_Allocator()
	{}

	inline ProxyAllocator(const ansichar* allocatorName, size_type size) 
		: Parent(allocatorName),
		m_Allocator(size)
	{}

	inline ProxyAllocator(ProxyAllocator&& other) {
		*this = std::forward(other);
	}

	inline ProxyAllocator& operator = (ProxyAllocator&& rhs) {
		if (this != &rhs) {
			Parent::operator = (*this);
			m_Allocator = std::move(rhs);
		}
		return *this;
	}

	inline void initialize(size_type objectSize, uint8 alignment, uint8 offset = 0) {
		m_Allocator.initialize(objectSize, alignment, offset);
	}

	inline void initialize(size_type desiredNumObjects, size_type objectSize, uint8 alignment, uint8 offset = 0) {
		m_Allocator.initialize(desiredNumObjects, objectSize, alignment, offset);
	}

	inline void initialize(size_type objectSize, uint8 alignment, void* start, size_type size, uint8 offset = 0) {
		m_Allocator.initialize(objectSize, alignment, start, size, offset);
	}

	inline void initialize(size_type objectSize, uint8 alignment, void* start, void* end, uint8 offset = 0) {
		m_Allocator.initialize(objectSize, alignment, start, end, offset);
	}

	inline void* allocate(size_type size, size_type alignment) {
		return Parent::allocate(m_Allocator, size, alignment);
	}

	inline void free(void* ptr) {
		Parent::free(m_Allocator, ptr);
	}

	inline void* getStart() const { return m_Allocator.getStart(); }
	inline void* getEnd() const { return m_Allocator.getEnd(); }
	inline size_type getSize() const { return m_Allocator.getSize(); }
private:
	PoolAllocator m_Allocator;
};

ENDNAMESPACE
