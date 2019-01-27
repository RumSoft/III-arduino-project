inline int between(int val, int a, int b)
{
	if (val < a)
		return a;
	if (val > b)
		return b;
	return val;
}

inline float betweenF(float val, float a, float b)
{
	if (val < a)
		return a;
	if (val > b)
		return b;
	return val;
}