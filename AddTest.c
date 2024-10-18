
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

 
int sum = 0;
 
void* adder(void *p)
{
	for (int i = 0; i < 1000000-1; i++)  // �����
	{
		int old = sum;
		while (!__sync_bool_compare_and_swap(&sum, old,(old + 1)%10000))  // ���old����sum, �Ͱ�old+1д��sum
		{
			old = sum; // ����old
		}
	}
 
	return NULL;
} 
 
int main()
{
	pthread_t threads[10];
 
	for (int i = 0; i < 10; i++)
	{
		pthread_create(&threads[i], NULL, adder, NULL);
	}
	
	for (int i = 0; i < 10; i++)
	{
		pthread_join(threads[i], NULL);
	}
 
	printf("sum is %d\n", sum);
}

