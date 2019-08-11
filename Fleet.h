#ifndef FLEET_H_INCLUDED
#define FLEET_H_INCLUDED

struct Fleet
{
	static const int	 	DEFAULTSIZE = 8;
	static const int		MAX_GIANT_COUNT = 1;
	static const int		MAX_BIG_COUNT = 2;
	static const int		MAX_MEDIUM_COUNT = 3;
	static const int 		MAX_SMALL_COUNT = 2;
	static const int		MAX_HITPOINTS = 26;
	int						size = 0;
	int						giantCount = 0;
	int						bigCount = 0;
	int						mediumCount = 0;
	int						smallCount = 0;
	int 					hitpoints = MAX_HITPOINTS;
	void reset()
	{
		size = 0;
		giantCount = 0;
		bigCount = 0;
		mediumCount = 0;
		smallCount = 0;
		hitpoints = MAX_HITPOINTS;
	}
};

#endif // FLEET_H_INCLUDED
