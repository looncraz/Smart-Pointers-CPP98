#ifndef B_SMART_POINTERS_H
#define B_SMART_POINTERS_H


#include "SmartPointersPrivate.h"


template <class T> class BWeakPtr;
template <class T> class BEnableSharedFromThis;


template <class T>
class BSharedPtr {
public:
								BSharedPtr();
								BSharedPtr(T* object);
								BSharedPtr(const BEnableSharedFromThis<T>* other);
								BSharedPtr(const BEnableSharedFromThis<T>& other);
								BSharedPtr(const BSharedPtr<T>& other);
								BSharedPtr(const BWeakPtr<T>& other);
								~BSharedPtr();

			void				SetTo(T* object);
			void				SetTo(const BSharedPtr<T>& other);
			void				Unset();
			bool				IsSet() const;

			uint32				CountOwners() const;
			uint32				CountWeak() const;

			T*					operator->();
	const	T*					operator->() const;

			T*					operator*();
	const	T*					operator*() const;

			bool				operator==(const T* pointer) const;

			BSharedPtr<T>&		operator=(const BSharedPtr<T>& other);
			BSharedPtr<T>&		operator=(const BWeakPtr<T>& other);
			BSharedPtr<T>&		operator=(const BEnableSharedFromThis<T>& other);

								operator bool() const;

private:
		friend class BWeakPtr<T>;
		BSharedPtr(const BReference< BPrivate::BSmartPtrData<T> >&);
		BReference< BPrivate::BSmartPtrData<T> >	fData;
};


template <class T>
class BWeakPtr {
public:
								BWeakPtr();
								BWeakPtr(const BEnableSharedFromThis<T>* other);
								BWeakPtr(const BEnableSharedFromThis<T>& other);
								BWeakPtr(const BSharedPtr<T>& reference);
								BWeakPtr(const BWeakPtr<T>& other);
								~BWeakPtr();

			void				Unset();

			// Weaks don't have the ability to
			bool				IsExpired() const;
			bool				IsValid() const;	// Do have a valid data structure?
			BSharedPtr<T>		GetShared();

			uint32				CountOwners() const;
			uint32				CountWeak() const;

								operator bool() const;	// same as IsValid(), not !IsExpired()
private:
		friend class BSharedPtr<T>;
		BReference< BPrivate::BSmartPtrData<T> >	fData;
};


template <class T>
class BEnableSharedFromThis {
protected:
								BEnableSharedFromThis();
	virtual						~BEnableSharedFromThis();

		BSharedPtr<T>			SharedFromThis();
		BWeakPtr<T>				WeakFromThis();

private:
		friend class BSharedPtr<T>;
		friend class BWeakPtr<T>;

		BReference< BPrivate::BSmartPtrData<T> >	fData;
};


// #pragma mark  BSharedPtr<T> Implementation


template <class T>
BSharedPtr<T>::BSharedPtr()
	:
	fData(new BPrivate::BSmartPtrData<T>(new T()))
{
	fData->AcquireShared();
}


template <class T>
BSharedPtr<T>::BSharedPtr(T* object)
	:
	fData(new BPrivate::BSmartPtrData<T>(object))
{
	fData->AcquireShared();
}


template <class T>
BSharedPtr<T>::BSharedPtr(const BEnableSharedFromThis<T>* other)
	:
	fData(other->fData)
{
	if (fData.IsSet())
		fData->AcquireShared();
}


template <class T>
BSharedPtr<T>::BSharedPtr(const BEnableSharedFromThis<T>& other)
	:
	fData(other.fData)
{
	if (fData.IsSet())
		fData->AcquireShared();
}


template <class T>
BSharedPtr<T>::BSharedPtr(const BSharedPtr<T>& other)
	:
	fData(other.fData)
{
	if (fData.IsSet())
		fData->AcquireShared();
}


template <class T>
BSharedPtr<T>::BSharedPtr(const BWeakPtr<T>& other)
	:
	fData(other.fData)
{
	if (fData.IsSet())
		fData->AcquireShared();
}


// This is how BWeakPtr creates a shared ptr
template <class T>
BSharedPtr<T>::BSharedPtr(const BReference< BPrivate::BSmartPtrData<T> >& data)
	:
	fData(data)
{
	if (fData.IsSet())
		fData->AcquireShared();
}


template <class T>
BSharedPtr<T>::~BSharedPtr()
{
	if (fData.IsSet())
		fData->ReleaseShared();
}


template <class T>
void
BSharedPtr<T>::SetTo(T* object)
{
	Unset();
	fData.SetTo(new BPrivate::BSmartPtrData<T>(object));
}


template <class T>
void
BSharedPtr<T>::SetTo(const BSharedPtr<T>& other)
{
	Unset();
	fData = other.fData;

	if (IsSet())
		fData->AcquireShared();
}


template <class T>
void
BSharedPtr<T>::Unset()
{
	if (fData.IsSet()) {
		printf("Release\n");
		fData->ReleaseShared();
		fData.Unset();
	}
}


template <class T>
bool
BSharedPtr<T>::IsSet() const
{
	if (fData.IsSet() && fData->GetObject() != NULL) return true;
	return false;
}


template <class T>
uint32
BSharedPtr<T>::CountOwners() const
{
	if (fData.IsSet())
		return fData->CountOwners();

	return 0;
}


template <class T>
uint32
BSharedPtr<T>::CountWeak() const
{
	if (fData.IsSet())
		return fData->CountWeak();

	return 0;
}


template <class T>
T*
BSharedPtr<T>::operator->()
{
	if (fData.IsSet())
		return fData->GetObject();

	return NULL;
}


template <class T>
const T*
BSharedPtr<T>::operator->() const
{
	if (fData.IsSet())
		return fData->GetObject();

	return NULL;
}


template <class T>
T*
BSharedPtr<T>::operator*()
{
	if (fData.IsSet())
		return fData->GetObject();

	return NULL;
}


template <class T>
const T*
BSharedPtr<T>::operator*() const
{
	if (fData.IsSet())
		return fData->GetObject();

	return NULL;
}


template <class T>
BSharedPtr<T>&
BSharedPtr<T>::operator=(const BSharedPtr<T>& other)
{
	SetTo(other);
	return *this;
}


template <class T>
bool
BSharedPtr<T>::operator==(const T* pointer) const
{
	if (!IsSet() && pointer == NULL) return true;
	if (fData.IsSet()) return pointer == fData->GetObject();
	return false;
}


template <class T>
BSharedPtr<T>::operator bool() const
{
	return IsSet();
}


// #pragma mark  BWeakPtr<T> Implementation


template <class T>
BWeakPtr<T>::BWeakPtr()
{

}


template <class T>
BWeakPtr<T>::BWeakPtr(const BEnableSharedFromThis<T>* other)
	:
	fData(other->fData)
{
}


template <class T>
BWeakPtr<T>::BWeakPtr(const BEnableSharedFromThis<T>& other)
	:
	fData(other.fData)
{
}


template <class T>
BWeakPtr<T>::BWeakPtr(const BSharedPtr<T>& reference)
	:
	fData(reference.fData)
{
}


template <class T>
BWeakPtr<T>::BWeakPtr(const BWeakPtr<T>& other)
	:
	fData(other.fData)
{
}


template <class T>
BWeakPtr<T>::~BWeakPtr()
{
}


template <class T>
void
BWeakPtr<T>::Unset()
{
	return fData.Unset();
}


template <class T>
bool
BWeakPtr<T>::IsExpired() const
{
	return !fData.IsSet() || fData->GetObject() == NULL;
}


template <class T>
bool
BWeakPtr<T>::IsValid() const
{
	return fData.IsSet();
}


template <class T>
BSharedPtr<T>
BWeakPtr<T>::GetShared()
{
	return BSharedPtr<T>(fData);
}


template <class T>
uint32
BWeakPtr<T>::CountOwners() const
{
	if (fData.IsSet())
		return fData->CountOwners();

	return 0;
}


template <class T>
uint32
BWeakPtr<T>::CountWeak() const
{
	if (fData.IsSet())
		return fData->CountWeak();

	return 0;
}


template <class T>
BWeakPtr<T>::operator bool() const
{
	return fData.IsSet();
}


// #pragma mark BEnableSharedFromThis


template <class T>
BEnableSharedFromThis<T>::BEnableSharedFromThis()
	:
	fData(new BPrivate::BSmartPtrData<T>((T*)this))
{
}


template <class T>
BEnableSharedFromThis<T>::~BEnableSharedFromThis()
{
	if (fData.IsSet()) {
		if (fData->CountOwners() > 0) {
			debugger("Oops!  Someone deleted a managed object directly!");
			fprintf(stderr, "Oops!  Someone deleted a managed object directly!\n");
		}
	}
}


template <class T>
BSharedPtr<T>
BEnableSharedFromThis<T>::SharedFromThis()
{
	return BSharedPtr<T>(*this);
}


template <class T>
BWeakPtr<T>
BEnableSharedFromThis<T>::WeakFromThis()
{
	return BWeakPtr<T>(*this);
}


#endif // B_SMART_POINTERS_H
