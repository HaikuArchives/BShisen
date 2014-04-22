#ifndef __STACK_H__
#define __STACK_H__


template <class TYPE>
class stack
{
public:
	stack();
	~stack();
	
TYPE	pop(void);
void	push(TYPE);
bool	empty(void)const { return _empty; };

private:
TYPE 	data;
stack<TYPE> * next;
bool	_empty;
};



template <class TYPE>
stack<TYPE>::stack()
{
	data = (TYPE)NULL;
	next = NULL;
	_empty = true;
}

template <class TYPE>
stack<TYPE>::~stack()
{
	if (next) delete next;
}

template <class TYPE>
TYPE stack<TYPE>::pop(void)
{
TYPE  t;
stack <TYPE> *n;

	if (_empty)
	{
		fprintf(stderr, "Stack: Can't pop!\n");
		exit(1);
	}

	t = data;

	if (next)
	{
		data = next->data;

		n = next->next;
		next->next = 0;

		delete next;

		next = n;
	}
	else _empty = true;


	return t;

}

template <class TYPE>
void stack<TYPE>::push(TYPE t)
{
stack<TYPE> *n;

	if (_empty)
	{
		data = t;
		_empty = false;
	}
	else
	{
		n = new stack<TYPE>;
		n->data = data;
		data = t;

		n->next = next;
		next = n;
	}

}

#endif
