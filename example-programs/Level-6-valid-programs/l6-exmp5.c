int main()
{
	int a,b,c;
	int *p,*q,*r;
	a=2;
	b=3;
	c=4;
	p=&a;
	q=&b;
	r=&c;
	*p=*q+*r;
	*r=*p-*q;
	*q=*q/7;
	
	return 0;
}
