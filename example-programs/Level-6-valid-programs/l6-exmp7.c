int d[3];
int main()
{
	int a,b,c,y,z;
	int *p;

	a=2;
	b=3;
	c=4;
	p=&a;

	d[0]=10;
	//d[1]=20;
	d[2]=30;

	y=d[*p];
	print y;
	print "\n";
	z=d[a+b-2*c+3];
	print z;	
	return 0;
}
