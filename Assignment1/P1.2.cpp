#include<iostream>
#include<string.h>
#include <cstdlib>
#include <algorithm>
#include <cilk/cilk.h>

#define STRING_LENGTH 8192
#define BLOCK_SIZE 128
int **costMatrix;
	void printCostMatrix(int size1,int size2){
	for(int i=1;i<=size1;i++){
		for(int j=1;j<=size2;j++){
			std::cout<<costMatrix[i][j]<<"\t";
		}
		std::cout<<std::endl;
	}
	
}

void lcs(char* seq1,char *seq2,int index1,int index2,int size1,int size2){
	int row,col;
	if(size1<=BLOCK_SIZE||size2<=BLOCK_SIZE){
		for(int i=0;i<size1;i++){
			for(int j=0;j<size2;j++){
				row = (index1+i)+1;
				col = (index2+j)+1;
				if(seq1[row-1]==seq2[col-1]){
					costMatrix[row][col]=costMatrix[row-1][col-1]+1;
				}else{
					costMatrix[row][col]=std::max(costMatrix[row-1][col],costMatrix[row][col-1]);
				}
			}
		}
	}
	else if(size1<=BLOCK_SIZE){
		int firstHalf = (size2+1)/2;
		int secondHalf = size2-firstHalf;
		lcs(seq1,seq2,index1,index2,size1,firstHalf);
		lcs(seq1,seq2,index1,index2+firstHalf,size1,secondHalf);
	}else if(size2<=BLOCK_SIZE){
		int firstHalf = (size1+1)/2;
		int secondHalf = size1-firstHalf;
		lcs(seq1,seq2,index1,index2,firstHalf,size2);
		lcs(seq1,seq2,index1+firstHalf,index2,secondHalf,size2);
	}
	else{
		int firstHalf1 = (size1+1)/2;
		int secondHalf1 = size1-firstHalf1;
		int firstHalf2 = (size2+1)/2;
		int secondHalf2 = size2-firstHalf2;
		cilk_spawn lcs(seq1,seq2,index1,index2,firstHalf1,firstHalf2);
		cilk_sync;
		cilk_spawn lcs(seq1,seq2,index1,index2+firstHalf2,firstHalf1,secondHalf2);
		cilk_spawn lcs(seq1,seq2,index1+firstHalf1,index2,secondHalf1,firstHalf2);
		cilk_sync;
		cilk_spawn lcs(seq1,seq2,index1+firstHalf1,index2+firstHalf2,secondHalf1,secondHalf2);
		cilk_sync;
	}     
}


void init(int size1,int size2){
	costMatrix=new int*[size1];
	for(int i=0;i<=size1;i++){
		costMatrix[i]=new int[size2+1];
		if(costMatrix[i]==NULL){
		std::cout << "Fatal Error. Cannot allocate memory for matrix"
		            << std::endl;
		std::exit(0);	
		}
	}
	for (int i=0;i<=size2;i++){
		costMatrix[0][i] = 0;
	}
	for (int i=0;i<=size1;i++){
		costMatrix[i][0] = 0;
	}
}
void backtrace(char*a,char*b,int index1,int index2,int lcs_length){
	if(lcs_length>0){
 		if(a[index1-1]==b[index2-1]){
 			backtrace(a,b,index1-1,index2-1,lcs_length-1);
			std::cout<<b[index2-1]<<"\t";
 		}else{
			if(costMatrix[index1][index2-1]<costMatrix[index1-1][index2]){
				backtrace(a,b,index1-1,index2,lcs_length);
			}else{
				backtrace(a,b,index1,index2-1,lcs_length);
			}
 		}
	}
}
void computeLCS(char*a, char*b,int length1,int length2){
	init(length1,length2);
	lcs(a,b,0,0,length1,length2);
	//sync;
	//printCostMatrix(length1,length2);
	//backtrace(a,b,length1,length2,costMatrix[length1][length2]);
	//std::cout<< std::endl;
}
void randstr(char s[], int n)
{
	const char *src = "abcdefghijklmnopqrstuvwxyz";	//alphabet
	for (int i=0; i<n; i++)
	{
		s[i] = src[rand() % 26];	//random letter generating
	}
}
int main(int argc,char**argv){
	char *a = new char[STRING_LENGTH];
	char *b = new char[STRING_LENGTH];
	randstr(a,STRING_LENGTH);
	randstr(b,STRING_LENGTH);
	int length1 = strlen(a);
	int length2 = strlen(b);
	if(length1<=length2){
		computeLCS(a,b,length1,length2);
	}else{
		computeLCS(b,a,length2,length1);
	}
	return 0;
}
