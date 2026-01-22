#ifndef	_CLIENTWORLD_H_
#define	_CLIENTWORLD_H_

class ClientWorld {
public:
	int	GetFPS() { return(fps); };

	int	fps;
};

extern ClientWorld	theWorld;

#endif
