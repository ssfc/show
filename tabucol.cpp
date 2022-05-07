#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <set>
#include <string>
#include <vector>
using namespace std;

int generate_random_integer(int lower, int upper)
{
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    uniform_int_distribution<> distr(lower, upper); // define the range 

    return distr(gen);
}

class Graph
{
	int num_vertex; // num of vertices; 
    int num_edge; // num of edges; 
    int num_color; // num of colors;    
	vector<vector<int> > adjacent; 
    vector<int> solution;
public:
    // Constructor; 
	void initialize_graph(int input_num_vertex, int input_num_edge, int input_num_color) 
	{
		num_vertex = input_num_vertex; 
        num_edge = input_num_edge;
//        num_color = input_num_color;
        num_color = 63;

		adjacent.resize(num_vertex); // initialize adjacent matrix; 
        for(int i=0;i<adjacent.size();i++)
        {
            adjacent[i].resize(num_vertex);
        }

        solution.resize(num_vertex); // initialize solution; 
		for(int i=0;i<solution.size();i++)
		{
			solution[i] = -1;
		}
	}

    // print adjacent matrix of graph; 
    void print_graph() 
    {
        cout<<"graph: "<<num_vertex<<" "<<num_edge<<" "<<num_color<<endl;
        for(int i=0;i<adjacent.size();i++)
        {
            for(int j=0;j<adjacent[i].size();j++)
            {
                cout<<adjacent[i][j]<<" ";
            }
            cout<<endl;
        }
    }
    
    // function to add an edge to graph; 
	void add_edge(int head, int tail) 
    {
        adjacent[head][tail] = 1;
        adjacent[tail][head] = 1;
    }

    int compute_conflict(vector<int> sol)
    {
        int conflict = 0;
        for(int i=0;i<adjacent.size();i++)
        {
            for(int j=i+1; j<adjacent[i].size();j++)
            {
                if(adjacent[i][j] == 1 && sol[i] == sol[j])
                {
                    conflict++;
                }
            }
        }

        return conflict;
    }
    
    // do tabucol for graph; 
	bool tabucol(int max_walks = 100, int max_iterations = 1000000) 
    {     
        vector<vector<int>> tabu_tenure_table; 
        tabu_tenure_table.resize(num_vertex);
        for(int i=0;i<tabu_tenure_table.size();i++)
        {
            tabu_tenure_table[i].resize(num_color); 
        }

        // initialize each vertex with random color;    
//        cout<<"initial color of each vertex: ";
        for(int i=0;i<solution.size();i++)
        {            
            solution[i] = generate_random_integer(0, num_color-1);     
//            cout<<solution[i]<<' ';        
        }
//        cout<<endl;

        int aspiration_criterion = num_vertex; 
        
        int current_num_conflict; 
        for(int iteration=0; iteration<max_iterations;iteration++)
        {
            // Count vertex pairs (i,j) which are adjacent and have the same color; 
            set<int> set_conflict_vertex;
            current_num_conflict = 0; 
            for(int i=0;i<num_vertex;i++)
            {
                for(int j=i+1;j<num_vertex;j++)
                {
                    if(adjacent[i][j] == 1 && solution[i] == solution[j]) // vertices belongs to one edge have same color; 
                    {
                        set_conflict_vertex.insert(i);
                        set_conflict_vertex.insert(j);
                        current_num_conflict++; 
                    }
                }
            }            

            // convert conflict vertex from set to vector; 
            vector<int> vector_conflict_vertex(set_conflict_vertex.begin(), set_conflict_vertex.end());
            
            if(current_num_conflict == 0) // Found a solution.
            {
                break; 
            }

            vector<int> new_solution(num_vertex, -1);
            int vertex_changed_idx; 
            int vertex_changed; 
            int new_color; 

            for(int step=0; step<max_walks; step++)
            {
                // Choose a random vertex to move; 
                vertex_changed_idx = generate_random_integer(0, vector_conflict_vertex.size()-1); 
                vertex_changed = vector_conflict_vertex[vertex_changed_idx];

                // Choose a new color; 
                int new_color = generate_random_integer(0, num_color-1);
                while(solution[vertex_changed] == new_color)
                {
                    new_color = generate_random_integer(0, num_color-1);
                }

                // Create a neighbor solution; 
                new_solution = solution;
                new_solution[vertex_changed] = new_color; 

                // Count adjacent pairs with the same color in the new solution.
                int new_num_conflict = compute_conflict(new_solution); 
                if(new_num_conflict < current_num_conflict)  // found an improved solution
                {                    
                    if(new_num_conflict < aspiration_criterion)
                    {
                        aspiration_criterion = new_num_conflict; 
                        
                        // permit tabu move if it is better than previous best; 
                        if(tabu_tenure_table[vertex_changed][new_color] > 0)  
                        {
                            tabu_tenure_table[vertex_changed][new_color] = 0; 
                            cout<<"satisfy aspiration tabu released: "<<current_num_conflict<<"->"<<new_num_conflict<<endl; 
                            break; 
                        }

                    }
                    else
                    {                      
                        if(tabu_tenure_table[vertex_changed][new_color] > 0)
                        {
                            // tabu move does not satisfy aspiration; 
                            cout<<"tabu forbidden"<<endl;
                            continue; 
                        }
                    }
                    
                    cout<<"Iteration "<<iteration<<": "<<current_num_conflict<<"->"<<new_num_conflict<<endl;
                    break; 
                }

            }
            
            tabu_tenure_table[vertex_changed][solution[vertex_changed]] = current_num_conflict + generate_random_integer(1,10);            
            
            solution = new_solution; 
            for(int i=0;i<num_vertex;i++)
            {
                for(int j=0;j<num_color;j++)
                {
                    if(tabu_tenure_table[i][j] > 0)
                    {
                        tabu_tenure_table[i][j] = tabu_tenure_table[i][j] - 1;
                    }
                }
            }
        }

        if(current_num_conflict != 0)
            return false;
        else
            cout<<"find answer!"<<endl;
            for(int i=0;i<num_vertex;i++)
            {
                cout<<i<<"->"<<solution[i]<<" ";            
            }
            cout<<endl;
            
            return true;
    }

	void save_vertex_color() // save color of each vertex; 
    {
        ofstream outFile("solution.txt");
        // the important part
        for (const auto &e : solution) outFile << e << "\n";
    }
    
};

int main() 
{
    
    Graph g_test;
    
    int counter = 0;
    int input_num_vertex = 0;
    int input_num_edge = 0;
    int input_num_color = 0;

    ifstream file("DSJC0500.5.txt");
//    ifstream file("chvatal.txt");
    int content;
    int head = -1; // temporary variable recording input edge;  
    int tail = -1; // temporary variable recording input edge;  

    while(file>>content) 
    {
//        cout<<content<<' ';        
        if(counter == 0)
        {
            input_num_vertex = content;            
        }
        else if(counter == 1)
        {
            input_num_edge = content;
        }
        else if(counter == 2)
        {
            input_num_color = content;
            g_test.initialize_graph(input_num_vertex, input_num_edge, input_num_color);
        }
        else
        {
            if(counter % 2 == 1)
            {                
                head = content;
            }
            else
            {
                tail = content; 
                g_test.add_edge(head, tail);
                head = -1; 
                tail = -1; 
            }
        }

        counter++;
    }

//    g_test.print_graph(); 
    g_test.tabucol();
    g_test.save_vertex_color();

    return 0;
}









