#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <string>
#include <limits>
#include <random>


using namespace std;

// Function to validate the name
bool isValidName(const string& name)
{
    // the name to be at least 3 characters long
    return name.length() >= 3;
}

// Function to validate the age
bool isValidAge(int age)
{
    return age >= 18 && age <= 100; // a valid age range
}

// Function to validate the mobile number
bool isValidMobileNumber(const string& phoneNumber)
{
    if (phoneNumber.empty())
    {
        return false;
    }

    //  if all characters are numeric
    for (char digit : phoneNumber)
    {
        if (!isdigit(digit))
        {
            return false;
        }
    }

    //  if the length of the phone number is exactly 10
    return (phoneNumber.length() == 10);
}

// Function to validate the address
bool isValidAddress(const string& address)
{
    // should not be empty
    return !address.empty();
}

class AirportSystem
{
private:
    unordered_map<string, vector<pair<string, int>>> graph; // To store the distance between two flights
    // in format (source,destination,distance)

    unordered_map<string, unordered_map<string, int>> prices; // Added to store flight prices
   // in format (source,destination,prices)

public:
    // Function to display the shortest path between two airports along with prices
    void displayShortestPath(const string& start, const unordered_map<string, int>& distances, const unordered_map<string, string>& previous, const string& destination);

// Function to add a flight between two airports with a specified distance and price
    void addFlight(const string& source, const string& destination, int price)
    {
        srand(static_cast<unsigned int>(time(nullptr)));

        int lower_bound = 100; //min 100
        int upper_bound = 1000; //max 1000

        // Generate a random number between 100 and 1000
        int distance = rand() % (upper_bound - lower_bound + 1) + lower_bound;  // Random distance between 100 and 1000

//putting in graph the value of destination and its distance from source (random number generated using rand function)

        graph[source].push_back(make_pair(destination, distance));
        //setting the distance between source and destination

        graph[destination].push_back(make_pair(source, distance));
        //setting the distance between source and destination for round trip

        prices[source][destination] = price; // Set the price for the flight
        prices[destination][source] = price; // Assuming prices are the same for round trips
    }


    // Function to find the shortest path between two airports using Dijkstra's algorithm
    unordered_map<string, string> dijkstra(const string& start, const string& destination)
    {
        unordered_map<string, int> distances;
        unordered_map<string, string> previous;
         // To store the previous airport in the shortest path

        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;

        // Initialize distances to infinity firstly for all airports
        for (const auto& plane : graph)
        {
            distances[plane.first] = numeric_limits<int>::max();
        }

        distances[start] = 0;
        pq.push(make_pair(0, start));

        // Dijkstra's algorithm to find the shortest path
        while (!pq.empty())
        {
            string current = pq.top().second;
            int currentDistance = pq.top().first;
            pq.pop();

            for (const auto& neighbor : graph[current])
            {
                int newDistance = currentDistance + neighbor.second;
                if (newDistance < distances[neighbor.first])
                {
                    distances[neighbor.first] = newDistance;
                    previous[neighbor.first] = current;
                    pq.push(make_pair(newDistance, neighbor.first));
                }
            }
        }

        displayShortestPath(start, distances, previous, destination);


        return previous;
    }

    // Function to display the airport list
    void displayAirportList() const
    {
        cout<<endl<<endl;
        cout << "Airports in the system:\n";
        bool cnt=false;
        for (const auto& plane : graph)
        {
            //accessing all the source airports
            cout << plane.first << endl;
            cnt=true;
        }
        if(cnt==false){
            cout<<"No Airports present";
        }
    }

    // Function to display flight details
    void displayFlightDetails() const
    {
        cout<<endl<<endl;
        cout << "Flight details:\n";
        bool cnt=false;
        for (const auto& plane : graph)
        {
            cout<<endl;
            cnt=true;
            cout << "From " << plane.first << " to:\n";
            for (const auto& flight : plane.second)
            {
                // Retrieve the price using pair.first as the destination
                cout << "  " << flight.first << " - " << flight.second << " km, Price: " << prices.at(plane.first).at(flight.first) << " rupees\n";
            }
        }
        if(cnt==false){
            cout<<" No flight details present";
        }
    }

    // Function to check if an airport is valid
    bool isValidAirport(const string& airport) const
    {
        return graph.find(airport) != graph.end();
    }

    // Function to remove a flight between two airports
    void removeFlight(const string& source, const string& destination)
    {
        if (graph.find(source) != graph.end() && graph.find(destination) != graph.end())
        {
            // Remove the flight from both source and destination
            auto itSource = find_if(graph[source].begin(), graph[source].end(),
                                    [&destination](const pair<string, int>& flight)
            //find if will return the end if condition not satisfied else the iterator will point to that
            {
                return flight.first == destination;
                //checks if first element of the pair is equal to the destination
            });

              //similarly as above,
            auto itDest = find_if(graph[destination].begin(), graph[destination].end(),
                                  [&source](const pair<string, int>& flight)
            {
                return flight.first == source;
            });

            if (itSource != graph[source].end() && itDest != graph[destination].end())
            {
                //removing them from the graph structure
                graph[source].erase(itSource);
                graph[destination].erase(itDest);

                //removing them from the prices
                prices[source].erase(destination);
                prices[destination].erase(source);

                cout << "Flight between " << source << " and " << destination << " removed successfully !!.\n";
            }
            else
            {
                cout << "No such flight found between " << source << " and " << destination << ".\n";
            }
        }
        else
        {
            cout << "Airport not found .\n";
        }
    }

    // Function to display all available routes from a given airport
    void displayAllRoutesFromAirport(const string& airport)
    {
        if (isValidAirport(airport))
        //if the airport coming in as parameter is valid
        {
            cout<<endl;
            cout << "Available routes from " << airport << ":\n";
            for (const auto& flight : graph[airport])
            {
                //accessing the destination and distance between source and destination
                cout << "  " << airport << " to " << flight.first << " - Distance: " << flight.second << " units";

                // Check if prices are available
                if (prices.find(airport) != prices.end() && prices[airport].find(flight.first) != prices[airport].end())
                {
                    cout << ", Price: " << prices.at(airport).at(flight.first) << " units\n";
                }
                else
                {
                    cout << ", Price information not available\n";
                }
            }
        }
        else
        {
            //if airport not valid
            cout << "Invalid airport. Please enter a valid airport.\n";
        }
    }
};
// Function to display the remaining routes between two airports

void AirportSystem::displayShortestPath(const string& start, const unordered_map<string, int>& distances, const unordered_map<string, string>& previous, const string& destination)
{
    cout<<endl<<endl;
    //if destination not found
    if (previous.find(destination) == previous.end())
    {
        cout << "No route found between " << start << " and " << destination << endl;
    }
    else
    {
        //if destination found
        cout << "Shortest distance from " << start << " to " << destination << ": " << distances.at(destination) << " km\n";

        //  if prices are available
        if (prices.find(start) != prices.end() && prices[start].find(destination) != prices[start].end())
        {
            cout << "Total Price: " << prices.at(start).at(destination) << " rupees\n";
        }
        else
        {
            cout<<endl;
            cout << "As there is no direct flight, the total price would be the sum of all other flights taken to reach the destination if available. \n";
        }
         cout<<endl;
        cout << "Flight route:\n";

        string current = destination;
        while (current != start)
        {
            string previousAirport = previous.at(current);

            cout << previousAirport << " to " << current << " - Distance: " << graph[previousAirport][0].second << " km";

            //  if prices are available
            if (prices.find(previousAirport) != prices.end() && prices[previousAirport].find(current) != prices[previousAirport].end())
            {
                cout << ", Price: " << prices.at(previousAirport).at(current) << " rupees\n";
            }
            else
            {
                cout << ", Price information not available\n";
            }

            current = previousAirport;
        }

    }
}



// Function to print the main menu
void printExtendedMenu()
{
    cout<<endl;
    cout << "Extended Airport System Menu:\n";
    cout << "1. Add Flight\n";
    cout << "2. Remove Flight\n";
    cout << "3. Find Shortest Path\n";
    cout << "4. Display Airport List\n";
    cout << "5. Display Flight Details\n";
    cout << "6. Display All Routes from an Airport\n";
    cout << "7. Exit\n";
    cout << "Enter your choice: ";
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    cout<<"-------------------------------*"<<endl;
    cout<<"|                                                            |"<<endl;
    cout<<"|                                                            |"<<endl;
    cout<<"|                                                            |"<<endl;
    cout<<"|                       JET SET GO                           |"<<endl;
    cout<<"|                                                            |"<<endl;
    cout<<"|                                                            |"<<endl;
    cout<<"|                                                            |"<<endl;
    cout<<"-------------------------------*"<<endl;


    string userName;
    int age;
    string address;
    string mobileNumber;

    // Validate inputs
    do
    {
        cout << "Enter your name: ";
        getline(cin, userName);

        if (!isValidName(userName))
        {
            cout << "Invalid name. Please enter a valid name (at least 3 characters).\n";
        }
    }
    while (!isValidName(userName));

    // Validate inputs
    do
    {
        cout << "Enter your age (18-100): ";
        cin >> age;

        if (!isValidAge(age))
        {
            cout << "Invalid age. Please enter a valid age.\n";
        }
    }
    while (!isValidAge(age));

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

    // Validate address
    do
    {
        cout << "Enter your address: ";
        getline(cin, address);

        if (!isValidAddress(address))
        {
            cout << "Invalid address. Please enter a valid address.\n";
        }
    }
    while (!isValidAddress(address));

    // Validate mobile number
    do
    {
        cout << "Enter your mobile number (10 digits): ";
        cin >> mobileNumber;

        if (!isValidMobileNumber(mobileNumber))
        {
            cout << "Invalid mobile number. Please enter a valid 10-digit mobile number.\n";
        }
    }
    while (!isValidMobileNumber(mobileNumber));

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

    AirportSystem airport;

    while (true)
    {
        printExtendedMenu();

        int choice;

        while(!(cin>>choice)){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
           cout<<"invalid, Enter again:  ";
}

        switch (choice)
        {
        case 1:
        {
            // Add Flight
            cout << endl;
            string source, destination;
            int price;

            cout << "Enter source airport: ";
            cin >> source;

            cout << "Enter destination airport: ";
            cin >> destination;

            cout << "Enter price for the flight: ";
            cin >> price;

            airport.addFlight(source, destination, price);
            cout << "Flight added successfully!\n";
            cout << endl
                 << endl;
            break;
        }
        case 2:
        {
            // Remove Flight
            cout<<endl<<endl;
            cout << endl;
            string source, destination;

            cout << "Enter source airport: ";
            cin >> source;

            cout << "Enter destination airport: ";
            cin >> destination;

            airport.removeFlight(source, destination);
            cout << endl
                 << endl;
            break;
        }
        case 3:
        {
            // Find Shortest Path
            string start, dest;

            cout << "Enter source airport: ";
            cin >> start;

            cout << "Enter destination airport: ";
            cin >> dest;

            if (airport.isValidAirport(start) && airport.isValidAirport(dest))
            {
                airport.dijkstra(start, dest);
            }
            else
            {
                cout << "No such airport exists, kindly go through flight details to check further.\n";
            }
            cout << endl
                 << endl;
            break;
        }
        case 4:
            // Display Airport List
            airport.displayAirportList();
            cout << endl;
            break;
        case 5:
            // Display Flight Details
            airport.displayFlightDetails();
            cout << endl;
            break;
        case 6:
        {
            // Display All Routes from an Airport
            string airportname;
            cout << "Enter airport name for its available routes: ";
            cin >> airportname;
            airport.displayAllRoutesFromAirport(airportname);
            cout << endl;
            break;
        }
        default:
            if (choice == 7)
            {
                // Exit
                cout << "Exiting the Extended Airport System. Goodbye!\n";
                cout << endl;
                return 0;
            }
            else
            {
                cout << "Invalid choice. Please try again.\n";
            }
        }
    }

    return 0;
}
