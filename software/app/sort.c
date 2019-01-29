#include <hf-risc.h>

#define ELEMENTS	128

int sort(int v[], int e){
	int i, j, k;

	for(i=0; i<e-1; i++){
		for(j=i+1; j<e; j++){
			if (v[i] > v[j]){
				k = v[i];
				v[i] = v[j];
				v[j] = k;
			}
		}
	}
}

void swap(int v[], int i, int j){
	int temp;

	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}

void qsort(int v[], int left, int right){
	int i, last;

	if (left >= right)
		return;
	swap(v, left, (left + right) >> 1);
	last = left;
	for (i = left + 1; i <= right; i++)
		if (v[i] < v[left])
			swap(v, ++last, i);
	swap(v, left, last);
	qsort(v, left, last-1);
	qsort(v, last+1, right);
}

int main(void){
	int v[ELEMENTS];
	int i,j;
	volatile unsigned int time;

	for(i=0; i<ELEMENTS; i++)
		v[i] = random();

	printf("\n\nrandom elements: ");
	for(i=0; i<ELEMENTS; i++)
		printf("%d ", v[i]);
	
	time = TIMER0;
	sort(v, ELEMENTS);
	time = TIMER0 - time;

	printf("\n\nsorted elements (%d cycles): ", time);
	for(i=0; i<ELEMENTS; i++)
		printf("%d ", v[i]);

	for(i=0; i<ELEMENTS; i++)
		v[i] = random();

	printf("\n\nrandom elements: ");
	for(i=0; i<ELEMENTS; i++)
		printf("%d ", v[i]);
	
	time = TIMER0;
	qsort(v, 0, ELEMENTS-1);
	time = TIMER0 - time;

	printf("\n\nsorted elements (%d cycles): ", time);
	for(i=0; i<ELEMENTS; i++)
		printf("%d ", v[i]);
}

