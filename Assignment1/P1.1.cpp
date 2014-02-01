#include<iostream>
#include <cilk/cilk.h>
#include <cstdlib>
#define DEFAULT_MATRIX_SIZE 8

using namespace std;
static int BLOCK_SIZE=2;
class Matrix{
	double *matrix;
	int size;
	private :
		void init(bool initz);
		void multiply(double *out,int ro,int co,double *A,int ra,int ca,double *B,int rb,int cb,int blocksize,int size);
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
void printMatrix(double*matrix,int size){
	for(int i = 0 ; i < size;i++){
		for(int j=0;j<size;j++){
			std::cout<<matrix[i*size+j]<<"\t";
		}
		std::cout<<std::endl;
	}
}

void Matrix :: multiply(double *out,int ro,int co,double *A,int ra,int ca,double *B,int rb,int cb,int blocksize,int size){
	if(blocksize<=BLOCK_SIZE){
		for(unsigned int i=0;i<blocksize;i++){
			for(unsigned int k=0;k<blocksize;k++){
				for(unsigned int j=0;j<blocksize;j++){
					out[(i+ro)*size+(j+co)] += A[(i+ra)*size+(k+ca)] * B[(k+rb)*size+(j+cb)];
				}	
			}	
		}
	}else{
		int nwsize = blocksize/2;
		int ro1 = ro, ro2 = ro +nwsize;
		int co1 = co, co2 = co+nwsize;
		int ra1 = ra, ra2 = ra+nwsize;
		int ca1 = ca, ca2 = ra+nwsize;
		int rb1 = rb, rb2 = rb+nwsize;
		int cb1 = cb, cb2 = cb+nwsize;

		cilk_spawn multiply(out,ro1,co1,A,ra1,ca1,B,rb1,cb1,nwsize,size);
		cilk_spawn multiply(out,ro1,co2,A,ra1,ca1,B,rb1,cb2,nwsize,size);
		cilk_spawn multiply(out,ro2,co2,A,ra2,ca1,B,rb1,cb2,nwsize,size);	
		cilk_spawn multiply(out,ro2,co1,A,ra2,ca1,B,rb1,cb1,nwsize,size);
		
		cilk_sync;

		cilk_spawn multiply(out,ro2,co1,A,ra2,ca2,B,rb2,cb1,nwsize,size);
		cilk_spawn multiply(out,ro2,co2,A,ra2,ca2,B,rb2,cb2,nwsize,size);		
		cilk_spawn multiply(out,ro1,co2,A,ra1,ca2,B,rb2,cb2,nwsize,size);
		cilk_spawn multiply(out,ro1,co1,A,ra1,ca2,B,rb2,cb1,nwsize,size);	
	}
}

Matrix* Matrix :: square(){
	Matrix *output = new Matrix(size,false);
	multiply(output->matrix,0,0,matrix,0,0,matrix,0,0,size,size);
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


