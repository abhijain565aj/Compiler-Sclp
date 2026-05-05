int main()
{
	int arr[5];
	int i,sum;
	i=0;
	while(i<5)
	{	
		read arr[i];
		i=i+1;
	}
	i=0;
	sum=0;
	while(i<5)
	{	
		print arr[i];
		sum=sum+arr[i];
	}
	print sum;
	return 0;
}
