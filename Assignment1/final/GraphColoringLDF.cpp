#include<iostream>
#include<cilk/cilk.h>
#include<vector>
#include<string>
#include<sstream>
#include<sys/time.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_opadd.h>
#define NUM_VERTICES 1000

using namespace std;


class Graph{
	vector<int>* adjacencyList;
	void computeDegree(int*degree,bool* mark);
	void initmark(bool*mark);
	int getSmallestColor(int vertex,int* color,bool *mark);
public:
	int numNodes;
	int numColorsUsed;
	Graph(int nodes);
	void addEdges(int source,int destn);
	void populate();
	void print();
	int* color();
	int * recolor(int *color);
	bool checkcolor(int * colors);
};


Graph :: Graph(int numNodes){
	this->numNodes = numNodes;
	adjacencyList = new vector<int>[numNodes];
}

void Graph::addEdges(int source, int destn){
	adjacencyList[source].push_back(destn);
}
void Graph::initmark(bool* mark){
	for(int i=0;i<numNodes;i++){
		mark[i]=false;
	}
}
int Graph::getSmallestColor(int vertex,int* color,bool *mark){
	bool markedDegree[numNodes];
	for(int k=0;k<numNodes;k++){
		markedDegree[k]=false;
	}
	for(int k=0;k<adjacencyList[vertex].size();k++){
		int adjVerIndex =adjacencyList[vertex].at(k);
		if(mark[adjVerIndex]){
			int colorIndex =color[adjVerIndex];
			markedDegree[colorIndex]=true;
		}
	}
	//serial
	int colorIndex;
	for(int k=0;k<numNodes;k++){
		if(markedDegree[k]==false){
			colorIndex=k;
			break;
		}
	}
	return colorIndex;
}

void Graph::computeDegree(int* degree, bool* mark){
	cilk_for(int i=0;i<numNodes;i++){
		if(mark[i]==false){
			cilk::reducer_opadd<int> total;
			for(int j=0;j<adjacencyList[i].size();j++){
				if(mark[adjacencyList[i].at(j)]==false){
					total+=1;
				}
			}
			degree[i]=total.get_value();
		}else{
			degree[i]=-1;
		}
	}
}
int* Graph::recolor(int* color){
	bool*mark = new bool[numNodes];
	int *newcolor = new int[numNodes];
	initmark(mark);
	for(int clrIdx = numColorsUsed;clrIdx>=0;clrIdx--){
		cilk_for(int vertex=0;vertex<numNodes;vertex++){
			if(!mark[vertex]&&color[vertex]==clrIdx){
				newcolor[vertex]=getSmallestColor(vertex,newcolor,mark);
				mark[vertex]=true;
			}
		}
	}
	cilk::reducer_max<int> best;
	cilk_for(int k=0;k<numNodes;k++){
        best.calc_max(color[k]);
    }
    numColorsUsed=best.get_value();
	return newcolor;
}


int* Graph::color(){
	int toBeColored = numNodes;
	int *degree= new int[numNodes];
	bool*mark = new bool[numNodes];
	int *color = new int[numNodes];
	initmark(mark);
	computeDegree(degree,mark);
	while(toBeColored>0){
		cilk::reducer_opadd<int> colored;
		cilk_for(int vertex=0;vertex<numNodes;vertex++){
			if(!mark[vertex]){
				bool highestDegree=true;
				for(int j=0;j<adjacencyList[vertex].size();j++){
					int adjVerIndex =adjacencyList[vertex].at(j);
					if((degree[adjVerIndex]>degree[vertex])||
						(degree[adjVerIndex]==degree[vertex]&&adjVerIndex<vertex)){
						highestDegree=false;
					}
				}
				if(highestDegree==true){
					color[vertex]=getSmallestColor(vertex,color,mark);
					mark[vertex]=true;
					colored+=1;
				}
			}
		}
		toBeColored-=colored.get_value();
 		computeDegree(degree,mark);
	}
	cilk::reducer_max<int> best;
	cilk_for(int k=0;k<numNodes;k++){
        best.calc_max(color[k]);
    }
    numColorsUsed=best.get_value();
	return color;
}

bool Graph::checkcolor(int* colors){
	for(int vertex=0;vertex<numNodes;vertex++){
		int mycolor = colors[vertex];
		for(int j=0;j<adjacencyList[vertex].size();j++){
			int adjVerIndex =adjacencyList[vertex].at(j);
			if(mycolor==colors[adjVerIndex])
				return false;
		}
	}
	return true;
}


void Graph::populate(){
	for(int i =0;i<numNodes;i++){
		for(int j=0;j<numNodes;j++){
			if(i!=j){
				addEdges(i,j);
			}
		}
	}
}

void Graph::print(){
	for(int i =0;i<numNodes;i++){
		cout<<"Node : "<<i<<" ";
		vector<int> neigbors = adjacencyList[i];
		for(int j=0;j<neigbors.size();j++){
			cout<<neigbors.at(j)<<"\t";
		}
		cout<<endl;
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


Graph* readInput(){
	string line;
	getline(cin,line);
	vector<int> *values = split(line,' ');
	Graph *graph= new Graph(values->at(0));
	int edges = values->at(1);
	for(int k=0;k<edges;k++){
		getline(cin,line);
		values = split(line,' ');
		graph->addEdges(values->at(0)-1,values->at(1)-1);
		graph->addEdges(values->at(1)-1,values->at(0)-1);
	}
	return graph;	
}

void writeOutput(int* color,int numNodes,int numColorsUsed,int timeelapsed){
    cout<<timeelapsed<<endl;
    cout<<numColorsUsed+1<<std::endl;
	for(int k=0;k<numNodes;k++){
        cout<<k+1<<" "<<color[k]+1<<endl;
    }
}

int main(int argc,char** argv){
	Graph* mygraph=readInput();
	//mygraph->print();
	//Graph *mygraph = new Graph(NUM_VERTICES);
	//mygraph->populate();
	clock_t start=clock();
	int* colors = mygraph->color();
	int numColorsUsed,numIterations=0,*newcolor;
	numColorsUsed=mygraph->numColorsUsed;
 	do{
		newcolor=mygraph->recolor(colors);
		if(numColorsUsed<mygraph->numColorsUsed){
			break;
		}
		colors = newcolor;
		numColorsUsed=mygraph->numColorsUsed;
		numIterations++;
	}while(numIterations<20);
	double timeElapsed = (((double)(clock()-start)*1000)/(double)CLOCKS_PER_SEC);
	if(mygraph->checkcolor(colors)){
		writeOutput(colors,mygraph->numNodes,numColorsUsed,timeElapsed);
	}
}
