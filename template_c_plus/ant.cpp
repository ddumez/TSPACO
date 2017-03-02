
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <limits.h>
#include <string.h>

#include "tsp.cpp"
#include "utils.cpp"

using namespace std;

class Ant{
    bool init;
    long int* tour;           /* solution */
    bool* visited;        /* auxiliary array for construction to keep track of visited cities */
    long int tour_length;
    long *idum; //pointeur to the seed of the random generator

    double ** probability;    /*pointer to the pheremone of the colony!*/
    TSP * tsp;
    long int size;

    public:
       Ant();
       Ant(TSP* tsp_arg, double ** prob_info, long *idum);
       ~Ant();
       Ant(Ant const& other); 
       
       long int Search();
       void ComputeTourLength();
       long int getTourLength(); 

       void depositPheromone(double  ** pheromone); //the ant depose pheromone

       void print();
       

};

/* Default constructor*/
Ant::Ant(){
  init=false;
}

/* Constructor*/
Ant::Ant (TSP* tsp_arg, double ** prob_info, long *idum){
  size = tsp_arg->getSize();
  tsp  = tsp_arg;
  probability = prob_info; 
  tour = new long int[size];
  visited = new bool[size];
  tour_length = LONG_MAX;
  init = true;
  this->idum = idum;
}

/* Copy constructor*/
Ant::Ant (Ant const& other){
  size = other.size;
  tsp  = other.tsp;
  probability = other.probability;
  tour = new long int[size];
  visited = new bool[size];
  for(int i=0; i<size ; i++){
     tour[i] = other.tour[i];
     visited[i] = other.visited[i];
  }
  tour_length = other.tour_length;
  init=true;
  idum = other.idum;
  
}

Ant::~Ant (){
  if(init){
    delete[] tour;
    delete[] visited;
  }
  init=false;
}

/*Generate tour using probabilities*/
long int Ant::Search(){
  int i,j, parc;
  double sum, acc, random;

  //no visited city
  for(i = 0; i<size; ++i) {visited[i] = false;}

  //choose the start city
  parc = (int)(ran01(idum) * size);
  if (size == parc) {parc = size-1;}
  visited[parc] = true;
  tour[0] = parc;

  i = 0;
  while(i<size) {
    //we are in the city parc
    //we need to compute normalized probability
    sum = 0;
    for(j = 0; j<size; ++j) {
      if (! visited[j]) {sum += probability[parc][j];}
    }

    //compute the next city
    random = ran01(idum);
    acc = 0; j = 0;

    while ((acc < random) && (j<size) ) {
      acc += probability[parc][j] / sum;
      ++j;
      while(visited[j]) {++j;} //skip visited city
    }

    //go to the city j-1
    --j; while(visited[j]) {--j;} //skip visited city
cout<<"ville : "<<j<<endl;    
    parc = j;
    tour[i] = parc;
    visited[parc] = true;

    ++i; //one more city visited
  }

}

/*Compute the length of tour*/
/* ! The tours start and end in the SAME city*/
void Ant::ComputeTourLength() {
  tour_length = 0;
  for(int i = 0; i<size-1; ++i) {
    tour_length += tsp->getDistance(tour[i], tour[i+1]);
  }
  tour_length += tsp->getDistance(tour[size-1], tour[0]);
}

long int Ant::getTourLength(){
  return(tour_length);
}

void Ant::depositPheromone(double  ** pheromone) {
  this->ComputeTourLength();
  for(int i = 0; i<size-1; ++i) {
    pheromone[tour[i]][tour[i+1]] = pheromone[tour[i]][tour[i+1]] + 1.0 / this->getTourLength();
    pheromone[tour[i+1]][tour[i]] = pheromone[tour[i]][tour[i+1]];
  }
  //go back at thestart town
  pheromone[tour[size-1]][tour[0]] = pheromone[tour[size-1]][tour[0]] + 1.0 / this->getTourLength();
  pheromone[tour[0]][tour[size-1]] = pheromone[tour[size-1]][tour[0]];
}

void Ant::print() {
  cout<<"chemin : ";
  for(int i = 0; i<size; ++i) {
    cout<<tour[i]<<" ";
  }
  cout<<endl;
  this->ComputeTourLength();
  cout<<"taille : "<<tour_length<<endl;
}