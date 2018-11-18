#include <stdio.h>

// index-th float of farey's sequence
// n: degree of farey's sequence
float farey_indexed(int n, int index) 
{	
	printf("func(%d,%d)\n", n, index);
	float ret;

	typedef struct { int d, n; } frac;
	frac f1 = {0, 1}, f2 = {1, n}, t;
	int k;
 
 	ret = 1.0 / n;
	printf("%d/%d %d/%d", 0, 1, 1, n);
	int i;
	for (i = 0; i < index; i++) {
		if (f2.n <= 1) {
			break;
		}
		k = (n + f1.n) / f2.n;
		t = f1, f1 = f2;
		f2 = (frac) { f2.d * k - t.d, f2.n * k - t.n };
		printf(" %d/%d", f2.d, f2.n);
	}
 	ret = ((float)f2.d) / f2.n;
	printf("%c", '\n');
	return ret;
}

// prints a farey sequence of degree(n)
// source: https://rosettacode.org/wiki/Farey_sequence#C
// Farey for n=3: 1/3 1/2 2/3
void farey(int n)
{
	typedef struct { int d, n; } frac;
	frac f1 = {0, 1}, f2 = {1, n}, t;
	int k;
 
	printf("%d/%d %d/%d", 0, 1, 1, n);
	while (f2.n > 1) {
		k = (n + f1.n) / f2.n;
		t = f1, f1 = f2;
		f2 = (frac) { f2.d * k - t.d, f2.n * k - t.n };
		printf(" %d/%d", f2.d, f2.n);
	}
 
	printf('\n');
}

/* https://stackoverflow.com/questions/8194894/finding-the-position-of-a-fraction-in-farey-sequence
  int a, b;
  a = 3;
  b = 5;

  int sum=0;
  int A[1000];
  A[1]=a;

  for(i = 1; i <= n; ++i){
      A[i] = (a*i)/b;
  }

  for(i=2;i<=n;i++)
    A[i]=i*a-a;

  for(i=2;i<=n;i++)
  {
    for(j=i+i;j<=n;j+=i)
      A[j]-=A[i];
  }

  for(i=1;i<=n;i++)
    sum+=A[i];

  ans = sum/b;*/