#ifndef _SMART_POINTERS_PRIVATE_H
#define _SMART_POINTERS_PRIVATE_H


#include <stdio.h>

#include <Referenceable.h>


namespace BPrivate {


/*
	BWeakPtr works merely by having a BReference to BSmartPtrData<T>.
	It will fail to acquire a shared pointer if the object's lifetime has expired.
*/
template <class T>
class BSmartPtrData : public BReferenceable {
public:
								BSmartPtrData(T* object);
	virtual						~BSmartPtrData();

			void				AcquireShared();
			void				ReleaseShared();
			int32				CountOwners() const;
			int32				CountWeak() const;

			void				DeleteObject();

			T*					GetObject();
	const	T*					GetObject() const;

			void				LastReferenceReleased();

private:
	BSmartPtrData(const BSmartPtrData<T>& noCopying);
	BSmartPtrData& operator=(const BSmartPtrData<T>& noCopying);

			T*					fObject;
	volatile int32				fOwnerCount;
	volatile int32				fLock;
};


template <class T>
BSmartPtrData<T>::BSmartPtrData(T* object)
	:
	fObject(object),
	fOwnerCount(0),
	fLock(0)
{
}


template <class T>
BSmartPtrData<T>::~BSmartPtrData()
{
	// TODO: ASSERT(fOwnerCount == 0)
	// TODO: ASSERT(fReferenceCount == 0)
	// TODO: ASSERT(fPointer == NULL)
}


template <class T>
void
BSmartPtrData<T>::AcquireShared()
{
	atomic_add(&fOwnerCount, 1);
	// Guard against returning to the caller before any deletion occurs. Doing so could allow
	// the caller to have access to the object as it is being deleted, when the lock is released
	// the object will be NULL if it has been deleted - holding ownership of a NULL object is legal.
	while (atomic_get(&fLock));;
}


template <class T>
void
BSmartPtrData<T>::ReleaseShared()
{
	atomic_add(&fLock, 1);
	if (atomic_add(&fOwnerCount, -1) == 1)
		DeleteObject();
	atomic_add(&fLock, -1);
}


template <class T>
int32
BSmartPtrData<T>::CountOwners() const
{
	return fOwnerCount;
}


template <class T>
int32
BSmartPtrData<T>::CountWeak() const
{	// have to remove self-reference from BReference
	return CountReferences() - (fOwnerCount + 1);
}


template <class T>
void
BSmartPtrData<T>::DeleteObject()
{
	delete fObject;
	fObject = NULL;
}


template <class T>
T*
BSmartPtrData<T>::GetObject()
{
	return fObject;
}


template <class T>
const T*
BSmartPtrData<T>::GetObject() const
{
	return fObject;
}


template <class T>
void
BSmartPtrData<T>::LastReferenceReleased()
{
	// This happens when the last shared or weak reference ceases to exist...
	// This is just a place to debug... currently nothing to do.
	printf("No more references!\n");
}


};

#endif // _SMART_POINTERS_PRIVATE_H
