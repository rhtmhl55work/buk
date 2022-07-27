#ifndef CDSU_LIST_H_
#define CDSU_LIST_H_
//==============================================================================
// Internet Telephony Software - (c) Copyright 2002, Internet Telephony Group
// Center for Development of Telematics (C-DOT), Bangalore, India.
// Filename	: cdsulist.h 
// Description	: Defines a list template and iterator.
// Ref Docs	:
// Author	: Titty Thomas
// Created On   : Wed Apr 30 10:02:30 IST 2003
// Last Modified: 1. Fri Jul 30 12:05:04 IST 2004
// 		: 	By :Vijaya Kumar
// 		: 	Warning Messages are sloved.
// 		: 2. Mon Aug 2 10:38:01 IST 2004
//		:	By : Vijaya Kumar
//			Bugs bs/36 fixed.
//		:	Insert and Erase Api are added
//		: 3. Mon Aug 16 10:39:08 IST 2004
//		:	By : Vijaya Kumar
//		:	Copy Construtor is added
//		: 	Bugs bs/31 is fixed
//==============================================================================

//------------------------------------------------------------------------------
// Class	: CdSuList 
// Description	: CdSuList is imlemented as a circular doubly linked list
//------------------------------------------------------------------------------

template <class ValType>
class CdSuList
{
public:
	CdSuList ();
	CdSuList (CdSuList<ValType>& list);
	~CdSuList ();

	void push_front (const ValType& data);
	void pop_front ();
	void push_back (const ValType& data);
	void pop_back ();
	void clear ();
	ValType& front ();
	ValType& back ();
	bool empty () const;
	int size () const;

private:

	// Node of the list
	struct ListElement
	{
		ValType data; 		// data 
		ListElement* rLink; 	// right link
		ListElement* lLink; 	// left link
	};

	// The list has special node called a head node, the data field
	// of which doesnot contain any information.
	ListElement* headNode; 
	int count;

public:
	//--------------------------------------------------------------------
	// Class	: iterator 
	// Description	: CdSuList iterator
	//--------------------------------------------------------------------

	class iterator
	{
	public:
		iterator (ListElement* init = NULL);
		ValType& operator* ();
		const ValType& operator* () const;
		iterator& operator++ ();
		iterator operator++ (int);
		bool operator== (const iterator& iter) const;
		bool operator!= (const iterator& iter) const;
	private:
		ListElement* current;
		friend class CdSuList;
	}; // iterator

	iterator begin () const;
	iterator end () const;
	iterator insert (iterator pos, const ValType& data);
	iterator erase (iterator iter);
	iterator erase (iterator first, iterator last);

}; // class CdSuList

//----------------------------------------------------------------------------
// Function	: CdSuList
// Description	: CdSuList constructor.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Initialise head node and count.
//----------------------------------------------------------------------------

template <class ValType>
CdSuList<ValType>::CdSuList () : count (0) 
{ 
    	headNode = new ListElement;
	headNode->lLink = headNode;
	headNode->rLink = headNode;

} // CdSuList::CdSuList ()

//----------------------------------------------------------------------------
// Function	: CdSuList
// Description	: Copy Constructor
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: 
//----------------------------------------------------------------------------

template <class ValType>
CdSuList<ValType>::CdSuList (CdSuList<ValType>& list): count (0)
{ 
	headNode = new ListElement;
	headNode->lLink = headNode;
	headNode->rLink = headNode;

	for (iterator iter = list.begin (); iter != list.end (); ++iter)
	{
		push_back (*iter);
	} // for
} // CdSuList::CdSuList ()

//------------------ ----------------------------------------------------------
// Function	: ~CdSuList 
// Description	: CdSuList desctructor.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Clear the list, delete the head node.
//----------------------------------------------------------------------------

template <class ValType>
CdSuList<ValType>::~CdSuList ()
{
	clear ();
	delete headNode;

} // CdSuList::~CdSuList ()

//----------------------------------------------------------------------------
// Function	: push_front
// Description	: Push an element to the front of the list.
// Inputs	: Element to be added.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Create a new list element and add to the front of the list.
//----------------------------------------------------------------------------

template <class ValType>
void CdSuList<ValType>::push_front (const ValType& data)
{
	ListElement* tmp = new ListElement;
	tmp->data = data;
	tmp->rLink = headNode;
	tmp->lLink = headNode->lLink;
	headNode->lLink->rLink = tmp;
	headNode->lLink = tmp;
    	++count;

} // CdSuList::push_front ()

//----------------------------------------------------------------------------
// Function	: pop_front 
// Description	: Pop an element from the front of the list. 
// Inputs	: None. 
// Return Value	: None. 
// Side Effects	: None.
// Algorithm	: If the list is not empty delete the element at the front of
//		  the list.
//----------------------------------------------------------------------------

template <class ValType>
void CdSuList<ValType>::pop_front ()
{
    	if (!empty ())
	{
		ListElement* tmp = headNode->lLink;
		tmp->lLink->rLink = tmp->rLink;
		tmp->rLink->lLink = tmp->lLink;
		delete tmp;
		--count;
	}

} // CdSuList::pop_front ()
			
//----------------------------------------------------------------------------
// Function	: push_back
// Description	: Push an element to the back of the list.
// Inputs	: Element to be added.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Create a new list element and add to the back of the list. 
//----------------------------------------------------------------------------

template <class ValType>
void CdSuList<ValType>::push_back (const ValType& data)
{
    	ListElement* tmp = new ListElement; 
	tmp->data = data;
	tmp->lLink = headNode;
	tmp->rLink = headNode->rLink;
	headNode->rLink->lLink = tmp;
	headNode->rLink = tmp;
	++count;
} // CdSuList::push_back ()

//----------------------------------------------------------------------------
// Function	: pop_back
// Description	: Pops an element from the back of the list.
// Inputs	: None.
// Return Value	: None. 
// Side Effects	: None.
// Algorithm	: If the list is not empty, delete the element at the back of
//		  the list.
//----------------------------------------------------------------------------

template <class ValType>
void CdSuList<ValType>::pop_back ()
{
    	if (!empty ())
	{
		ListElement* tmp = headNode->rLink;
		tmp->lLink->rLink = tmp->rLink;
		tmp->rLink->lLink = tmp->lLink;
		delete tmp;
		--count;
	}

} // CdSuList::pop_back ()

//----------------------------------------------------------------------------
// Function	: clear
// Description	: Deletes all the elements of the list.
// Inputs	: None.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Delete elements till the list is empty.
//----------------------------------------------------------------------------

template <class ValType>
void CdSuList<ValType>::clear ()
{
	while (!empty ())
	{
	    pop_back ();
	}

} // CdSuList::clear ()

//----------------------------------------------------------------------------
// Function	: front
// Description	: Returns the first element of the list.
// Inputs	: None.
// Return Value	: First element.
// Side Effects	: None.
// Algorithm	: Returns the first element of the list.
//----------------------------------------------------------------------------

template <class ValType>
ValType& CdSuList<ValType>::front ()
{
	return headNode->lLink->data;

} // CdSuList::front ()

//----------------------------------------------------------------------------
// Function	: back 
// Description	: Returns the last element of the list. 
// Inputs	: None.
// Return Value	: Last element.
// Side Effects	: None.
// Algorithm	: Returns the last element of the list.
//----------------------------------------------------------------------------

template <class ValType>
ValType& CdSuList<ValType>::back ()
{
	return headNode->rLink->data;

} // CdSuList::back ()

//----------------------------------------------------------------------------
// Function	: empty 
// Description	: Checks whether the list is empty. 
// Inputs	: None. 
// Return Value	: None. 
// Side Effects	: None.
// Algorithm	: Return true if list is empty, else false.
//----------------------------------------------------------------------------

template <class ValType>
bool CdSuList<ValType>::empty () const
{
    	return ((headNode->lLink == headNode) && (headNode->rLink == headNode));

} // CdSuList::empty ()

//----------------------------------------------------------------------------
// Function	: size
// Description	: Returns the size of the list.
// Inputs	: None. 
// Return Value	: int 
// Side Effects	: None.
// Algorithm	: Return the number of elements in the list.
//----------------------------------------------------------------------------

template <class ValType>
int CdSuList<ValType>::size () const
{
    	return count;

} // CdSuList::empty ()

//----------------------------------------------------------------------------
// Function	: begin
// Description	: Returns an iterator pointing to the beginning of the list.
// Inputs	: None. 
// Return Value	: Iterator
// Side Effects	: None.
// Algorithm	: Creates and returns an iterator pointing to the beginning of
//		  the list.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuList<ValType>::iterator CdSuList<ValType>::begin () const 
{
	return iterator (headNode->lLink);

} // CdSuList::begin ()

//----------------------------------------------------------------------------
// Function	: end
// Description	: Returns an iterator pointing to the end of the list.
// Inputs	: None. 
// Return Value	: Iterator
// Side Effects	: None.
// Algorithm	: Creates and returns an iterator pointing to the end of
//		  the list.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuList<ValType>::iterator CdSuList<ValType>::end () const
{
	return iterator (headNode);

} // CdSuList::end ()

//----------------------------------------------------------------------------
// Function	: insert
// Description	: insert an element before position (iter).
// Inputs	: Position(iterator) and Element to be Inserted.
// Return Value	: iterator of new inserted element.
// Side Effects	: None.
// Algorithm	: Create a new list element and add to the back of the pos. 
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuList<ValType>::iterator CdSuList<ValType>::insert (iterator pos, 
		const ValType& data)
{
    	ListElement* tmp = new ListElement; 
	tmp->data = data;
	tmp->rLink = pos.current->rLink;
	tmp->lLink = pos.current;
	pos.current->rLink->lLink = tmp;
	pos.current->rLink = tmp;
	++count;
	return iterator (tmp);
} // CdSuList::insert ()

//----------------------------------------------------------------------------
// Function	: erase
// Description	: Erases an element pointed by the iterator.
// Inputs	: Iterator. 
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Get the element pointed by the iterator. Adjust the pointers
//		  of the list and delete the element. Return the new iterator.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuList<ValType>::iterator CdSuList<ValType>::erase (iterator iter)
{
	ListElement* tmp = iter.current;
	iterator newiter (tmp->lLink);
	tmp->lLink->rLink = tmp->rLink;
	tmp->rLink->lLink = tmp->lLink;
	delete tmp;
	--count;
	return newiter;

} // CdSuList::erase ()

//----------------------------------------------------------------------------
// Function	: erase
// Description	: Erases an element from first to last.
// Inputs	: Iterator. 
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Get the element pointed by the iterator. Adjust the pointers
//		  of the list and delete the element. Return the new iterator.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuList<ValType>::iterator  CdSuList<ValType>::erase (iterator first,
					iterator last)
{
	while (first != last)
	{
		first = erase (first);
	}
	return last;
} // CdSuList::erase ()


//----------------------------------------------------------------------------
// Function	: iterator 
// Description	: CdSuList iterator constructor.
// Inputs	: list element pointer.
// Return Value	: None.
// Side Effects	: None.
// Algorithm	: Initialise the current list pointer
//----------------------------------------------------------------------------

template <class ValType>
CdSuList<ValType>::iterator::iterator (ListElement* init) : current (init) 
{ 
	// Nothing to do.
} // CdSuList::iterator::iterator ()

//----------------------------------------------------------------------------
// Function	: operator* 
// Description	: Deference operator.
// Inputs	: None.
// Return Value	: Reference of element value at the current position,
// Side Effects	: None.
// Algorithm	: Return the element value at the current position.
//----------------------------------------------------------------------------

template <class ValType>
ValType& CdSuList<ValType>::iterator::operator* ()
{
	return current->data;

} // CdSuList::iterator::operator*  ()
 
//----------------------------------------------------------------------------
// Function	: operator* 
// Description	: Constant dereference operator. 
// Inputs	: None.
// Return Value	: Constant reference of the element value at the current
//		  position. 
// Side Effects	: None.
// Algorithm	: Return the element value at the current position.
//----------------------------------------------------------------------------

template <class ValType>
const ValType& CdSuList<ValType>::iterator::operator* () const
{
	return current->data;
	
} // CdSuList::iterator::operator* ()

//----------------------------------------------------------------------------
// Function	: operator++ 
// Description	: Prefix operator
// Inputs	: None.
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Change the current position to the next element in the list.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuList<ValType>::iterator& CdSuList<ValType>::iterator::operator++ ()
{
	current = current->lLink;
	return *this;
	
} // CdSuList::iterator::operator++

//----------------------------------------------------------------------------
// Function	: operator++ 
// Description	: Postfix operator 
// Inputs	: None.
// Return Value	: Iterator.
// Side Effects	: None.
// Algorithm	: Save the current position. Go to the next element. Return the
//		  saved position.
//----------------------------------------------------------------------------

template <class ValType>
typename CdSuList<ValType>::iterator CdSuList<ValType>::iterator::operator++ (int)
{
	iterator temp = *this;
	++*this;
	return temp;

} // CdSuList::iterator::operator++

//----------------------------------------------------------------------------
// Function	: operator== 
// Description	: Tests equality of two iterators.
// Inputs	: Iterator.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: Return true if iterators point to the same position, else
//		  false.
//----------------------------------------------------------------------------

template <class ValType>
bool CdSuList<ValType>::iterator::operator== (const iterator& iter) const
{
	return current == iter.current;

} // CdSuList::iterator::operator==

//----------------------------------------------------------------------------
// Function	: operator!= 
// Description	: Tests the inequality of two iterators.
// Inputs	: Iterator.
// Return Value	: boolean.
// Side Effects	: None.
// Algorithm	: Return true if iterators point to differnet positions, else
//		  false
//----------------------------------------------------------------------------

template <class ValType>
bool CdSuList<ValType>::iterator::operator!= (const iterator& iter) const
{
	return current != iter.current;

} // CdSuList::iterator::operator!=

//============================================================================
// <End of cdsulist.h>
//============================================================================
#endif // CDSU_LIST_H_
