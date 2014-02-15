#include<iostream>
#include <cilk/cilk.h>
#include <cstdlib>
#define DEFAULT_MATRIX_SIZE 5
static const int BLOCK_SIZE=2;
using namespace std;


//Matrix multiplication 
//Contains the implementaion of square of a matrix
class Matrix{
	double *matrix;
	int size;
	private :
		void init(bool initz);
		void multiply(double *out,int ro,int co,double *A,int ra,int ca,double *B,int rb,int cb,
					  int rowsize,int ksize,int colsize,int size);
	public :
		Matrix(bool initz);
		Matrix(int size,bool initz);
		Matrix* square();
		void print();
		~Matrix();
};

Matrix :: ~Matrix(){
	free(matrix);
}

Matrix :: Matrix(bool initz){
	size = DEFAULT_MATRIX_SIZE;
	init(initz);
}

Matrix :: Matrix(int size,bool initz){
	this->size = size;
	init(initz);
}

void Matrix :: init(bool initz){
	matrix=(double*) calloc(size* size, sizeof(double));
	if(matrix==NULL){
		std::cout << "Fatal Error. Cannot allocate for matrix"
		            << std::endl;
		std::exit(0);
	}
	if(initz){
		cilk_for(int i = 0; i <size*size; ++i) {
			matrix[i] = (i) % 1024;
		}
	}
}

void Matrix :: print(){
	for(int i = 0 ; i < size;i++){
		for(int j=0;j<size;j++){
			std::cout<<matrix[i*size+j]<<"\t";
		}
		std::cout<<std::endl;
	}
}
void printMatrix(double*matrix,int ro,int co,int blocksize,int size){
	 std::cout<<"Out :: "<<ro<<co<<endl;
	for(int i = ro ; i < ro+blocksize;i++){
		for(int j=co;j<co+blocksize;j++){
			std::cout<<matrix[i*size+j]<<"\t";
		}
		std::cout<<std::endl;
	}
}

void Matrix :: multiply(double *out,int ro,int co,double *A,int ra,int ca,double *B,int rb,int cb,
						int rowsize,int ksize,int colsize,int size){
	if(rowsize<=BLOCK_SIZE||colsize<=BLOCK_SIZE||ksize<=BLOCK_SIZE){
		for(unsigned int i=0;i<rowsize;i++){
			for(unsigned int k=0;k<ksize;k++){
				for(unsigned int j=0;j<colsize;j++){
					out[(i+ro)*size+(j+co)] += A[(i+ra)*size+(k+ca)] * B[(k+rb)*size+(j+cb)];
				}	
			}	
		}
	}else{
		int nrowsize = (rowsize+1)/2;
		int ncolsize = (colsize+1)/2;
		int ro1 = ro, ro2 = ro+nrowsize;
		int co1 = co, co2 = co+ncolsize;
		int ra1 = ra, ra2 = ra+nrowsize;
		int ca1 = ca, ca2 = ca+ncolsize;
		int rb1 = rb, rb2 = rb+nrowsize;
		int cb1 = cb, cb2 = cb+ncolsize;

		multiply(out,ro1,co1,A,ra1,ca1,B,rb1,cb1,nrowsize,nrowsize,ncolsize,size);
		multiply(out,ro1,co2,A,ra1,ca1,B,rb1,cb2,nrowsize,ncolsize,colsize-ncolsize,size);
		multiply(out,ro2,co2,A,ra2,ca1,B,rb1,cb2,rowsize-nrowsize,ncolsize,colsize-ncolsize,size);	
		multiply(out,ro2,co1,A,ra2,ca1,B,rb1,cb1,rowsize-nrowsize,ncolsize,ncolsize,size);
		
		//cilk_sync;

		multiply(out,ro2,co1,A,ra2,ca2,B,rb2,cb1,rowsize-nrowsize,colsize-ncolsize,ncolsize,size);
		multiply(out,ro2,co2,A,ra2,ca2,B,rb2,cb2,rowsize-nrowsize,rowsize-nrowsize,colsize-ncolsize,size);	
		multiply(out,ro1,co2,A,ra1,ca2,B,rb2,cb2,nrowsize,colsize-ncolsize,colsize-ncolsize,size);
		multiply(out,ro1,co1,A,ra1,ca2,B,rb2,cb1,nrowsize,rowsize-nrowsize,ncolsize,size);	
		
	}
}

Matrix* Matrix :: square(){
	Matrix *output = new Matrix(size,false);
	multiply(output->matrix,0,0,matrix,0,0,matrix,0,0,size,size,size,size);
	return output;
}

int main(int argc,char **argv){
	Matrix *a;
	if(argc ==1){
		a = new Matrix(true); 
	}else if(argc==2){
		int size = atoi(argv[1]); 
		a = new Matrix(size,true);
	}else{
		 std::cout << "Fatal Error. Insufficient Arguments"<< std::endl;
	}
	std::cout << "Input matrix"<< std::endl;
	a->print();
	Matrix* b = a->square();
	std::cout << "Output matrix"<< std::endl;
	b->print();
	return 0;
}


