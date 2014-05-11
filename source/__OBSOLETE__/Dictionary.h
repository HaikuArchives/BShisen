#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__


template <class KEY, class VALUE>
class Dictionary
{
public:
	Dictionary(int size = 5):
		_data(NULL),
		_resize(size),
		_currsize(0),
		_numentries(0)
	{
		if (!_resize) _resize = 5;	//must have a value
		_data = new ddata[resize];
		if (_data) _currsize = _resize;
	};

	~Dictionary()
	{
		if (_data) delete []data;
	};

void	AddValue(KEY key, VALUE value)
	{
		// must I resize the dictionary?
		if (_currsize == _numentries)
		{
			ddata *old;
			old = _data;
			_data = new ddata[_currsize + _resize];
			memcpy(_data, old, sizeof(struct ddata) * _currsize);
			_currsize += _resize;
			delete []old;
		}

		_data[_num_entries].key = key;
		_data[_num_entries].value = value;
	
		_numentries++;
	};

VALUE	GetValue(KEY key) const
	{
	register int i;
	VALUE ret_val = (VALUE)NULL;

		for (i = 0; i < _numentries; i++)
		{
			if (_data[i].key == key)
			{
				ret_val = _data[i].value;
				break;
			}
		}
		return ret_val;
	};


private:
int _resize;		// amt to resize in
int _currsize;		// current size of dictionary
int _numentries;	// current # of entries
struct ddata
{
	KEY key;
	VALUE value;
};

struct ddata *_data;
};


#endif
