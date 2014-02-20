#include<iostream>
#include<string.h>
#include <cstdlib>
#include<cilk/cilk.h>
#define BLOCK_SIZE 16
using namespace std;



void printCostMatrix(int **costMatrix,int size1,int size2){
	for(int i=1;i<=size1;i++){
		for(int j=1;j<=size2;j++){
			std::cout<<costMatrix[i][j]<<" ";
		}
		std::cout<<std::endl;
	}
	
}
void lcs(int **costMatrix,const char* seq1,const char *seq2,int length1,int length2){
	int row,col,colsize,rowsize;
	colsize = (length2+BLOCK_SIZE-1)/BLOCK_SIZE;
	rowsize = (length1+BLOCK_SIZE-1)/BLOCK_SIZE;
	for(int k=0;k<colsize;k++){
		int  numDiag= rowsize<k+1?rowsize:k+1;
		cilk_for(int l=0;l<numDiag;l++){
			for(int i=0;i<BLOCK_SIZE;i++){
				for(int j=0;j<BLOCK_SIZE;j++){
					int row = ((l)*BLOCK_SIZE+i)+1;
					int col = ((k-l)*BLOCK_SIZE+j)+1;
					if(row<=length1&&col<=length2){
						if(seq1[row-1]==seq2[col-1]){
							costMatrix[row][col]=costMatrix[row-1][col-1]+1;
						}else{
							costMatrix[row][col]=std::max(costMatrix[row-1][col],costMatrix[row][col-1]);
						}
					}
				}
			}
			
		}
	}
	for(int k=1;k<rowsize;k++){
		cilk_for(int l=colsize-1;l>=colsize-rowsize+k;l--){
			for(int i=0;i<BLOCK_SIZE;i++){
				for(int j=0;j<BLOCK_SIZE;j++){
					int row = ((k+colsize-l-1)*BLOCK_SIZE+i)+1;
					int col = (l*BLOCK_SIZE+j)+1;
					if(row<=length1&&col<=length2){
						if(seq1[row-1]==seq2[col-1]){
							costMatrix[row][col]=costMatrix[row-1][col-1]+1;
						}else{
							costMatrix[row][col]=std::max(costMatrix[row-1][col],costMatrix[row][col-1]);
						}
					}
				}
			}
			
		}
	}	
}

int ** init(int size1,int size2){
	int ** costMatrix=new int*[size1];
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
	return costMatrix;
}
void delet(int** costMatrix,int size1){
	for(int i=0;i<=size1;i++){
		delete[] costMatrix[i];
	}
	delete[] costMatrix;
}
void backtrace(int **costMatrix,const char*a,const char*b,int index1,int index2,int lcs_length){
	if(lcs_length>0){
 		if(a[index1-1]==b[index2-1]){
 			backtrace(costMatrix,a,b,index1-1,index2-1,lcs_length-1);
			std::cout<<b[index2-1];
 		}else{
			if(costMatrix[index1][index2-1]<costMatrix[index1-1][index2]){
				backtrace(costMatrix,a,b,index1-1,index2,lcs_length);
			}else{
				backtrace(costMatrix,a,b,index1,index2-1,lcs_length);
			}
 		}
	}
}
void computeLCS(const char*a,const char*b){
	if(strlen(a)>strlen(b)){
		const char *c = a;
		a = b ;
		b = c;
	}
	int length1 = strlen(a);
	int length2 = strlen(b);
	//int **costMatrix = init(length1,length2);
	
	//initialzation
	//-----------------------------------------------------------------------------------------
	int ** costMatrix=new int*[length1+1];
	for(int i=0;i<=length1;i++){
		costMatrix[i]=new int[length2+1];
		if(costMatrix[i]==NULL){
			std::cout << "Fatal Error. Cannot allocate memory for matrix"
						<< std::endl;
			std::exit(0);	
		}
	}
	for (int i=0;i<=length2;i++){
		costMatrix[0][i] = 0;
	}
	for (int i=0;i<=length1;i++){
		costMatrix[i][0] = 0;
	}
	//-----------------------------------------------------------------------------------------
	
	lcs(costMatrix,a,b,length1,length2);

	std::cout<<costMatrix[length1][length2]<<" ";
	backtrace(costMatrix,a,b,length1,length2,costMatrix[length1][length2]);
	std::cout<< std::endl;
	
	//deletion
	//-----------------------------------------------------------------------------------------
	for(int i=0;i<=length1;++i){
		delete[] costMatrix[i];
	}
	delete[] costMatrix;
	//-----------------------------------------------------------------------------------------
}

void readInput(){
	string line;
	int i=0;
	const char *seq1,*seq2;
	getline(cin,line);
	do{
		string sequence1;
		getline(cin,line);
		while(line.compare("$")){
			if(line.at(0)!='>'){
				sequence1.append(line);
			}
			getline(cin,line);
		}
		seq1=sequence1.c_str();
		
		string sequence2;
		getline(cin,line);
		while(line.compare("$$")){
			if(!line.at(0)!='>'){
				sequence2.append(line);
			}
			getline(cin,line);
		}
		seq2=sequence2.c_str();
		computeLCS(seq1,seq2);
		getline(cin,line);
	}while(line.compare("$$$"));
}
int main(int argc,char**argv){
	readInput();
	return 0;
}
