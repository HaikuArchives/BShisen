#ifndef __RRECT_H__
#define __RRECT_H__

class rRect
{
public:
	rRect(int x_1, int y_1, int x_2, int y_2, int type = 0):
	_x1(x_1), _x2(x_2), _y1(y_1), _y2(y_2), _type(type)
	{ };

int	x1(void) const {return _x1; };
int	x2(void) const {return _x2; };
int	y1(void) const {return _y1; };
int	y2(void) const {return _y2; };
int	type(void) const {return _type; };

void	set_x1(int v) { _x1 = v; };
void	set_x2(int v) { _x2 = v; };
void	set_y1(int v) { _y1 = v; };
void	set_y2(int v) { _y2 = v; };
void	set_type(int v) { _type = v; };

private:

int	_x1;
int	_x2;
int	_y1;
int	_y2;

int	_type;
};

#endif
