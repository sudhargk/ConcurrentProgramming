#include<iostream>
#include<stdio.h>
#include<cilk/cilk.h>
#include<vector>
#include<cstdio>
#include<string>
#include<sstream>
#include<stdlib.h>

using namespace std;

static int const NUM_GRIDS =10000;
static int const BLOCK_SIZE=1;
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
	short int indices[][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
	for(int j=0;j<8;j++){
		int currRow = row+indices[j][0];
		int currCol = column+indices[j][1];
		if(currRow<NUM_GRIDS&&currRow>=0&&currCol<NUM_GRIDS&&currCol>=0&&
					oldptr[currRow*NUM_GRIDS+currCol]==true){
				liveNeigbors+=1;
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
					//updatecell(oldptr,newptr,i,j);
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
	latest = p;
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