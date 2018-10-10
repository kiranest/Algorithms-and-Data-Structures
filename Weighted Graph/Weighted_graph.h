#ifndef WEIGHTED_GRAPH_H
#define WEIGHTED_GRAPH_H

#ifndef nullptr
#define nullptr 0
#endif

#include <iostream>
#include <limits>
#include <queue>
#include "Exception.h"


class Weighted_graph {
	private:
        int graph_size;
        double ** matrix;
        int edges;
        double * vertex_distances;
        bool * vertex_visited;
        int * previous_vertex;
        int * vertex_degree;
		static const double INF;

        // Pair stores a vertex and its distance to the source vertex
        class Pair{
            private:
                double edge_weight;
                int adjacent_vertex;
            public:
                Pair(int, double);
                double weight() const;
                int vertex() const;
        };

        // comparator is used for the priority_queue initialization of a min heap
        class comparator{
        public:
            int operator() (const Pair& p1, const Pair& p2)
            {
                return p1.weight() > p2.weight();
            }
        };



	public:
		Weighted_graph( int = 50 );
		~Weighted_graph();

		int degree( int ) const;
		int edge_count() const;
		double adjacent( int, int ) const;
		double distance( int, int );


		void insert( int, int, double );

	// Friends

	friend std::ostream &operator<<( std::ostream &, Weighted_graph const & );
};

// define INF using the numeric_limits library
const double Weighted_graph::INF = std::numeric_limits<double>::infinity();

// Constructor
Weighted_graph::Weighted_graph( int n):
graph_size( n > 0 ? n : 1 ),
matrix( new double * [graph_size] ),
edges( 0 ),
vertex_distances( new double[graph_size] ),
vertex_visited( new bool[graph_size]),
previous_vertex( new int[graph_size] ),
vertex_degree( new int[graph_size] )
{
    // Implementation of a lower-triangular adjacency matrix
    matrix[0] = nullptr;
    matrix[1] = new double [graph_size * (graph_size - 1)/2];

    matrix[1][0] = INF;

    // Initialize the degrees of the vertices to be 0
    for(int i = 0; i < graph_size; i++){
        vertex_degree[i] = 0;
    }

    // Loop that sets up the pointers of the lower-triangular adjacency matrix and
    for(int i = 2; i < graph_size; i++){
        matrix[i] = matrix[i - 1] + i - 1;

        // Initialize all edges in the adjacency matrix to INF
        for(int j = 0; j < i; j++){
            matrix[i][j] = INF;
        }
    }


}

// Destructor
Weighted_graph::~Weighted_graph() {
    //Deallocate all allocated memory for the arrays
    delete [] matrix[1];
    delete [] matrix;
    delete [] vertex_distances;
    delete [] vertex_visited;
    delete [] vertex_degree;
    delete [] previous_vertex;
}

// Return the degree of the argument vertex
int Weighted_graph::degree( int n ) const {
    return vertex_degree[n];
}

// Return the number of edges in the graph
int Weighted_graph::edge_count() const{
    return edges;
}

// Return the weight of the edge connecting the two argument vertices
double Weighted_graph::adjacent( int m, int n ) const {
    // Throw an illegal argument exception if the vertices don't correspond to any in the graph
    if(m < 0 || n < 0 || m >= graph_size || n >= graph_size){
        throw illegal_argument();
    }

    // The weight between the same vertices is 0
    if( m == n )
        return 0;
    else if(m > n)                  //Ensure proper access into the lower triangular matrix[][], 1st index > 2nd index
        return matrix[m][n];
    else
        return matrix[n][m];
}

// Return the shortest distance between the two argument vertices
double Weighted_graph::distance(int m, int n) {
    // Throw an illegal argument exception if the vertices don't correspond to any in the graph
    if(m < 0 || n < 0 || m > graph_size || n > graph_size)
        throw illegal_argument();
    // The distance between the same two vertices is 0
    if(m == n)
        return 0;

    // Initialize all elements in the arrays for the use in the Dijkstra's algorithm
    for(int i = 0; i < graph_size; i++){
        vertex_visited[i] = false;
        vertex_distances[i] = INF;
        previous_vertex[i] = -1;
    }

    // The source vertex has a distance of 0
    vertex_distances[m] = 0;

    // Use the priority_queue from the STL to declare a min heap
    // The min heap stores a vertex and its distance to the source in a Pair object
    // The distance to the source is used to compare between two Pair objects in the min heap
    std::priority_queue<Pair, std::vector<Pair>, comparator> min_heap;

    // Initially, push only the source vertex and its distance into the min heap
    min_heap.push(Pair(m,0));

    // Keep track of the current_vertex
    int current_vertex = m;

    //Implementation of Dijkstra's algorithm
    // Keep looping while the min heap has elements in it ( i.e. while there are unvisited vertices )
    while(!min_heap.empty()) {

        // Get the Pair with the smallest distance to the source
        Pair visiting = min_heap.top();

        // Remove the top object from the min heap
        min_heap.pop();

        // Lazy deletion: in our implementation of Dijkstra's algorithm, priorities are not updated in the min heap.
        // New Pair objects are added to the min heap if a smaller distance is found for any vertex.
        // Since the newer Pair objects will always be popped first, we can simply skip the older Pair object
        // when it is encountered by checking if its vertex has been visited
        if(vertex_visited[visiting.vertex()])
            continue;

        // If the visiting vertex is the second argument vertex, then a smaller distance to our argument vertex
        // cannot be found, thus we have the shortest distance to it.
        if(visiting.vertex() == n){
            return vertex_distances[n];
        }

        // Set the vertex we are visiting to have been visited
        vertex_visited[visiting.vertex()] = true;

        // Update tracker for our current vertex
        current_vertex = visiting.vertex();

        // Cycle through the graph for each vertex adjacent to the current vertex
        for (int i = 0; i < graph_size; i++) {
            // Only unvisited vertices can have their distances updates so ignore visited vertices
            if (!vertex_visited[i]) {
                // Ensure proper access into the lower triangular matrix[][], 1st index > 2nd index
                if (i < current_vertex) {
                    //Check if there is an edge between our graph and the vertex we are checking
                    if (matrix[current_vertex][i] != INF) {
                        // Check if a shorter distance has been found between the source vertex and the current vertex
                        if (vertex_distances[current_vertex] + matrix[current_vertex][i] < vertex_distances[i]) {
                            // Update the distance if the new distance is smaller
                            vertex_distances[i] = vertex_distances[current_vertex] + matrix[current_vertex][i];

                            // Update the vertex that the smaller distance comes from
                            previous_vertex[i] = current_vertex;

                            // Push a Pair with the current vertex and its new distance into the min heap
                            min_heap.push(Pair(i, vertex_distances[i]));
                        }
                    }
                } else if (i > current_vertex) {
                    //Check if there is an edge between our graph and the vertex we are checking
                    if (matrix[i][current_vertex] != INF) {
                        if (vertex_distances[current_vertex] + matrix[i][current_vertex] < vertex_distances[i]) {
                            // Update the distance if the new distance is smaller
                            vertex_distances[i] = vertex_distances[current_vertex] + matrix[i][current_vertex];

                            // Update the vertex that the smaller distance comes from
                            previous_vertex[i] = current_vertex;

                            // Push a Pair with the current vertex and its new distance into the min heap
                            min_heap.push(Pair(i, vertex_distances[i]));
                        }
                    }
                }
            }
        }

    }

    // If no more Pair objects are in the min heap, then there are no more vertices to visit since all remaining vertices
    // have a distance of infinity to them. Therefore, we haven't found a path between the two argument vertices
    return INF;
}

// Insert an edge with a weight between two vertices in the graph
void Weighted_graph::insert( int m, int n, double w){

    // Weights must be between 0 and INF, if not throw an illegal argument exception
    if(w <= 0 || w == INF){
        throw illegal_argument();
    }

    // Throw an illegal argument exception if the vertices don't correspond to any in the graph
    if(m < 0 || n < 0 || m > graph_size || n > graph_size || m == n){
        throw illegal_argument();
    }

    // Keep track of new edges to increase the edge count for the graph and degree count for each vertex
    bool newEdge = true;

    // Ensure proper access into the lower triangular matrix[][], 1st index > 2nd index
    if( n < m ){
        // If the weight between two edges is not INF, then it must have been updated before, thus it is not a new edge
        if(matrix[m][n] != INF)
            newEdge = false;

        matrix[m][n] = w;       // Update the weight in our adjacency matrix
    } else {
        // If the weight between two edges is not INF, then it must have been updated before, thus it is not a new edge
        if(matrix[n][m] != INF)
            newEdge = false;

        matrix[n][m] = w;       // Update the weight in our adjacency matrix
    }
    // Increment the degrees of the vertices and number of edges
    if(newEdge){
        vertex_degree[m]++;
        vertex_degree[n]++;
        edges++;
    }

}

// Constructor for the Pair class
Weighted_graph::Pair::Pair(int v, double w):
edge_weight(w),
adjacent_vertex(v)
{
    // Empty constructor
}


// Return the weight of the edges between the source vertex and this vertex
double Weighted_graph::Pair::weight() const {
    return edge_weight;
}

// Return this vertex
int Weighted_graph::Pair::vertex() const {
    return adjacent_vertex;
}

std::ostream &operator<<( std::ostream &out, Weighted_graph const &graph ) {
	return out;
}

#endif
