int main()
{
	int a[3];
	int b[3];
	int i;
	
	a[0]=20;
	a[1]=30;
	a[2]=40;
	b[0]=100;
	b[1]=200;
	b[2]=300;
	i=0;
	while(i<3)
	{
		a[i]=a[i]+b[i];
		b[i]=a[i]-b[i];
		a[i]=a[i]-b[i];
		i=i+1;
	}
	return 0;
}
