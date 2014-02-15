#include<iostream>
#include<cilk/cilk.h>
#include<vector>
#include<cstdio>
#include<string>
#include<sstream>
#include<stdlib.h>

using namespace std;

static int const NUM_GRIDS =10000;
static int const BLOCK_SIZE=20;
bool *p,*q,*latest;

void print(){
	for(int i=0;i<NUM_GRIDS;i++){
		for(int j=0;j<NUM_GRIDS;j++){
			cout<<(latest[i*NUM_GRIDS+j]==true)<<" ";
		}
		cout<<endl;
    }
}
void updatecell(bool*oldptr,bool*newptr,int row,int column){
	int liveNeigbors=0;
	for(int i=-1;i<=1;i++){
		for(int j=-1;j<=1;j++){
			if(i!=j&&row+i<NUM_GRIDS&&row+i>=0&&
				column+j<NUM_GRIDS&&column+j>=0&&
					oldptr[(row+i)*NUM_GRIDS+column+j]==true){
				liveNeigbors+=1;
			}
		}
	}
	if(oldptr[row*NUM_GRIDS+column]==true){
		if(liveNeigbors<2){
			newptr[row*NUM_GRIDS + column]=false;
		}else if(liveNeigbors==2||liveNeigbors==3){
			newptr[row*NUM_GRIDS + column]=true;
		}else{
			newptr[row*NUM_GRIDS + column]=false;
		}
	}else if(liveNeigbors==3){
		newptr[row*NUM_GRIDS + column]=true;
	}else{
		newptr[row*NUM_GRIDS + column]=false;
	}
}

void performGameofLife(int numSteps){
	bool *oldptr=p,*newptr=q;
	for(int n=0;n<numSteps;n++){
		cilk_for(int i=0;i<NUM_GRIDS;i+=BLOCK_SIZE){
			cilk_for(int j=0;j<NUM_GRIDS;j+=BLOCK_SIZE){
				for(int r=0;r<BLOCK_SIZE;r++){
					for(int s=0;s<BLOCK_SIZE;s++){
						updatecell(oldptr,newptr,i+r,j+s);
					}
				}
			}
		}
		latest=newptr;
		newptr=oldptr;
		oldptr=latest;
		
    }
}
vector<int> *split(const string s,char delim){
	stringstream stream;
	stream<<s;
	std::string token;
	vector<int> * values;
	values = new vector<int>();
	while(getline(stream, token, delim)) {
			values->push_back(atoi(token.c_str()));
	}
	return values;
}

void init(){
	p = new bool[NUM_GRIDS*NUM_GRIDS];
	q = new bool[NUM_GRIDS*NUM_GRIDS];
}
int readInput(){
	string line;
	vector<int>* values;
	for(int k=0;k<NUM_GRIDS;k++){
		getline(cin,line);
		values = split(line,' ');
		for(int j=0;j<NUM_GRIDS;j++){
			p[k*NUM_GRIDS+j]=values->at(j)==1;
		}
	}
	getline(cin,line);
	return atoi(line.c_str());
}

void writeOutput(){
	print();
}

int main(int argc,char** argv){
	init();
	int steps = readInput();
	performGameofLife(steps);
	writeOutput();
}