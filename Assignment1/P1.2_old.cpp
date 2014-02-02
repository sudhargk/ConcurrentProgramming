#include<iostream>
#include<string.h>
#include <cstdlib>
#include <algorithm>
//#include <cilk/cilk.h>
#define BLOCK_SIZE 2
int **costMatrix;

	void printCostMatrix(int size){
	for(int j=1;j<=size;j++){
		std::cout<<costMatrix[1][j]<<"\t";
	}
	std::cout<<std::endl;
}

void lcs(char* seq1,char *seq2,int index1,int index2,int size1,int size2){
	int row,col;
	std::cout<<"Block "<<index1<<" :: "<<index2<<" size1 :: "<<size1<<" size2 :: "<<size2<<std::endl;
	if(size1<=BLOCK_SIZE&&size2<=BLOCK_SIZE){
		if(index1==0){
			costMatrix[0][0]=0;
			for (int j=0;j<BLOCK_SIZE;j++){
				costMatrix[0][j+index2+1] = 0;
			}
		}
		for(int i=0;i<size1;i++){
			costMatrix[1][0] = 0;
			for(int j=0;j<size2;j++){
				row = (index1+i)+1;
				col = (index2+j)+1;
				std::cout<<row<<" :: "<<col<<std::endl;
				std::cout<<seq1[row-1]<<" :: "<<seq2[col-1]<<std::endl;
				if(seq1[row-1]==seq2[col-1]){
					costMatrix[1][col]=costMatrix[0][col-1]+1;
				}else{
					costMatrix[1][col]=std::max(costMatrix[0][col],costMatrix[1][col-1]);
				}
				printCostMatrix(7);
			}
			for (int j=0;j<size2;j++){
				costMatrix[0][j+index2+1] = costMatrix[1][j+index2+1];
			}
		}
	}else if(size1<=BLOCK_SIZE){
		int firstHalf = (size2+1)/2;
		int secondHalf = size2-firstHalf;
		lcs(seq1,seq2,index1,index2,size1,firstHalf);
		lcs(seq1,seq2,index1,index2+firstHalf,size1,secondHalf);
	}else if(size2<=BLOCK_SIZE){
		int firstHalf = (size1+1)/2;
		int secondHalf = size1-firstHalf;
		lcs(seq1,seq2,index1,index2,firstHalf,size2);
		lcs(seq1,seq2,index1+firstHalf,index2,secondHalf,size2);
	}else{
		int firstHalf1 = (size1+1)/2;
		int secondHalf1 = size1-firstHalf1;
		int firstHalf2 = (size2+1)/2;
		int secondHalf2 = size2-firstHalf2;
		lcs(seq1,seq2,index1,index2,firstHalf1,firstHalf2);
		lcs(seq1,seq2,index1,index2+firstHalf2,firstHalf1,secondHalf2);
		lcs(seq1,seq2,index1+firstHalf1,index2,secondHalf1,firstHalf2);
		lcs(seq1,seq2,index1+firstHalf1,index2+firstHalf2,secondHalf1,secondHalf2);
	}     
}


void init(int size){
	costMatrix=new int*[2];
	for(int i=0;i<=size;i++){
		costMatrix[i]=new int[size+1];
		if(costMatrix[i]==NULL){
		std::cout << "Fatal Error. Cannot allocate memory for matrix"
		            << std::endl;
		std::exit(0);	
		}
	}
}
void computeLCS(char*a, char*b,int length1,int length2){
	lcs(a,b,0,0,length1,length2);
	printCostMatrix(length2);
	for(int j=1;j<=length2;j++){
		if(costMatrix[0][j]!=costMatrix[0][j-1]){
			std::cout<<b[j]<<"\t";
		}
	}
	std::cout<<std::endl;
}
int main(int argc,char**argv){
	char *a = "abcas";
	char *b = "cdasdas";
	int length1 = strlen(a);
	int length2 = strlen(b);
	init(length2);
	if(length1<=length2){
		computeLCS(a,b,length1,length2);
	}else{
		computeLCS(b,a,length2,length1);
	}
	return 0;
}
