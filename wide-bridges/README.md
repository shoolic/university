
# WideBridgesFinder
The aim of the project was to create a tool for finding so-called *wide bridges* in an undirected graph structure. 

***Wide bridge*** is an edge that makes graph incoherent when we remove vertices on both its ends with all edges connected with them.

## Usage
### Input - graph description
* In the first line there is a positive integer number represnting number of all vertices in the graph.
* Below, for every edge in the graph there is a line with two positive integer numbers (separated by a space) representing number of vertices connected by this edge.
* Numaration of vertices starts from 0.  
* The program assumes the correctness of the entered data.

### Output - list of edges being a wide bridge
* Every wide bridge in the graph is printed in a separate line in the same way as an edge.
* When there are no wide bridges in the graph, program displays nothing.
## Tests
The *test.bash* script provides examples of graphs from the folder *graphs* as an input to WideBridgesFinder program and displays results of its execution.