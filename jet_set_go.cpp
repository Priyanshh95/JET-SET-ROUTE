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
#include <fstream>
#include <set>
#include <sstream>


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

    // Helper to avoid duplicate edges when saving
    set<pair<string, string>> saved_edges;

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

    // Save all flights to a file
    void saveToFile(const string& filename) const {
        ofstream out(filename);
        if (!out) {
            cerr << "Error opening file for saving flights!\n";
            return;
        }
        set<pair<string, string>> written;
        for (const auto& src : graph) {
            for (const auto& dest : src.second) {
                string a = src.first, b = dest.first;
                if (written.count({a, b}) || written.count({b, a})) continue;
                int distance = dest.second;
                int price = 0;
                auto it = prices.find(a);
                if (it != prices.end() && it->second.find(b) != it->second.end())
                    price = it->second.at(b);
                out << a << "," << b << "," << distance << "," << price << "\n";
                written.insert({a, b});
            }
        }
        out.close();
    }

    // Load all flights from a file
    void loadFromFile(const string& filename) {
        ifstream in(filename);
        if (!in) return; // No file to load
        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            size_t p1 = line.find(",");
            size_t p2 = line.find(",", p1+1);
            size_t p3 = line.find(",", p2+1);
            if (p1 == string::npos || p2 == string::npos || p3 == string::npos) continue;
            string src = line.substr(0, p1);
            string dest = line.substr(p1+1, p2-p1-1);
            int distance = stoi(line.substr(p2+1, p3-p2-1));
            int price = stoi(line.substr(p3+1));
            // Insert both directions
            graph[src].push_back({dest, distance});
            graph[dest].push_back({src, distance});
            prices[src][dest] = price;
            prices[dest][src] = price;
        }
        in.close();
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

void searchFlightsByPrice(const AirportSystem& airport, int minPrice, int maxPrice) {
    cout << "\nFlights with price between " << minPrice << " and " << maxPrice << ":\n";
    bool found = false;
    for (const auto& src : airport.graph) {
        for (const auto& dest : src.second) {
            if (src.first < dest.first) { // avoid duplicate pairs
                int price = -1;
                if (airport.prices.find(src.first) != airport.prices.end() && airport.prices.at(src.first).find(dest.first) != airport.prices.at(src.first).end()) {
                    price = airport.prices.at(src.first).at(dest.first);
                }
                if (price >= minPrice && price <= maxPrice) {
                    cout << "From: " << src.first << ", To: " << dest.first << ", Price: " << price << " rupees\n";
                    found = true;
                }
            }
        }
    }
    if (!found) cout << "No flights found in this price range.\n";
}

void searchFlightsByDistance(const AirportSystem& airport, int maxDistance) {
    cout << "\nFlights with distance less than or equal to " << maxDistance << ":\n";
    bool found = false;
    for (const auto& src : airport.graph) {
        for (const auto& dest : src.second) {
            if (src.first < dest.first && dest.second <= maxDistance) { // avoid duplicate pairs
                int price = -1;
                if (airport.prices.find(src.first) != airport.prices.end() && airport.prices.at(src.first).find(dest.first) != airport.prices.at(src.first).end()) {
                    price = airport.prices.at(src.first).at(dest.first);
                }
                cout << "From: " << src.first << ", To: " << dest.first << ", Distance: " << dest.second << " km, Price: " << price << " rupees\n";
                found = true;
            }
        }
    }
    if (!found) cout << "No flights found within this distance.\n";
}

struct User {
    string username;
    string password;
    int age;
    string address;
    string mobile;
    string role; // "admin" or "user"
};

vector<User> loadUsers(const string& filename) {
    vector<User> users;
    ifstream in(filename);
    if (!in) return users;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string username, password, ageStr, address, mobile, role;
        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, ageStr, ',');
        getline(ss, address, ',');
        getline(ss, mobile, ',');
        getline(ss, role, ',');
        if (!username.empty() && !password.empty() && !ageStr.empty() && !address.empty() && !mobile.empty() && !role.empty()) {
            users.push_back({username, password, stoi(ageStr), address, mobile, role});
        }
    }
    in.close();
    return users;
}

void saveUser(const string& filename, const User& user) {
    ofstream out(filename, ios::app);
    if (!out) return;
    out << user.username << "," << user.password << "," << user.age << "," << user.address << "," << user.mobile << "," << user.role << "\n";
    out.close();
}

bool userExists(const vector<User>& users, const string& username) {
    for (const auto& u : users) if (u.username == username) return true;
    return false;
}

User* authenticate(vector<User>& users, const string& username, const string& password) {
    for (auto& u : users) if (u.username == username && u.password == password) return &u;
    return nullptr;
}

struct Booking {
    string username;
    string source;
    string destination;
    int distance;
    int price;
};

vector<Booking> loadBookings(const string& filename) {
    vector<Booking> bookings;
    ifstream in(filename);
    if (!in) return bookings;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string username, source, destination, distStr, priceStr;
        getline(ss, username, ',');
        getline(ss, source, ',');
        getline(ss, destination, ',');
        getline(ss, distStr, ',');
        getline(ss, priceStr, ',');
        if (!username.empty() && !source.empty() && !destination.empty() && !distStr.empty() && !priceStr.empty()) {
            bookings.push_back({username, source, destination, stoi(distStr), stoi(priceStr)});
        }
    }
    in.close();
    return bookings;
}

void saveBooking(const string& filename, const Booking& booking) {
    ofstream out(filename, ios::app);
    if (!out) return;
    out << booking.username << "," << booking.source << "," << booking.destination << "," << booking.distance << "," << booking.price << "\n";
    out.close();
}

void saveAllBookings(const string& filename, const vector<Booking>& bookings) {
    ofstream out(filename);
    if (!out) return;
    for (const auto& b : bookings) {
        out << b.username << "," << b.source << "," << b.destination << "," << b.distance << "," << b.price << "\n";
    }
    out.close();
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

    vector<User> users = loadUsers("users.txt");
    User* currentUser = nullptr;

    while (!currentUser) {
        cout << "\nWelcome to JET SET GO!\n";
        cout << "1. Register\n2. Login\n3. Exit\nEnter choice: ";
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (choice == 1) {
            User newUser;
            cout << "Enter username: "; getline(cin, newUser.username);
            if (userExists(users, newUser.username)) {
                cout << "Username already exists. Try another.\n";
                continue;
            }
            cout << "Enter password: "; getline(cin, newUser.password);
            do {
                cout << "Enter your age (18-100): ";
                cin >> newUser.age;
                if (!isValidAge(newUser.age)) cout << "Invalid age. Please enter a valid age.\n";
            } while (!isValidAge(newUser.age));
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            do {
                cout << "Enter your address: "; getline(cin, newUser.address);
                if (!isValidAddress(newUser.address)) cout << "Invalid address. Please enter a valid address.\n";
            } while (!isValidAddress(newUser.address));
            do {
                cout << "Enter your mobile number (10 digits): "; getline(cin, newUser.mobile);
                if (!isValidMobileNumber(newUser.mobile)) cout << "Invalid mobile number. Please enter a valid 10-digit mobile number.\n";
            } while (!isValidMobileNumber(newUser.mobile));
            // Assign role
            if (users.empty()) {
                newUser.role = "admin";
                cout << "You are the first user and have been assigned admin role.\n";
            } else {
                cout << "Enter admin code to register as admin (or press Enter to register as user): ";
                string code; getline(cin, code);
                if (code == "JETADMIN2024") {
                    newUser.role = "admin";
                    cout << "Admin role assigned.\n";
                } else {
                    newUser.role = "user";
                    cout << "User role assigned.\n";
                }
            }
            users.push_back(newUser);
            saveUser("users.txt", newUser);
            cout << "Registration successful! Please login.\n";
        } else if (choice == 2) {
            string username, password;
            cout << "Enter username: "; getline(cin, username);
            cout << "Enter password: "; getline(cin, password);
            currentUser = authenticate(users, username, password);
            if (!currentUser) cout << "Invalid credentials. Try again.\n";
        } else if (choice == 3) {
            cout << "Goodbye!\n";
            return 0;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    cout << "\nWelcome, " << currentUser->username << "!\n";

    AirportSystem airport;
    // Load flights and prices from file
    airport.loadFromFile("flights.txt");

    vector<Booking> bookings = loadBookings("bookings.txt");

    while (true)
    {
        printExtendedMenu();
        if (currentUser->role == "admin") {
            cout << "8. Book a Flight\n";
            cout << "9. View My Bookings\n";
            cout << "10. Cancel a Booking\n";
            cout << "11. Search Flights by Price Range\n";
            cout << "12. Search Flights by Maximum Distance\n";
        } else {
            cout << "8. Book a Flight\n";
            cout << "9. View My Bookings\n";
            cout << "10. Cancel a Booking\n";
            cout << "11. Search Flights by Price Range\n";
            cout << "12. Search Flights by Maximum Distance\n";
        }
        int choice;
        while(!(cin>>choice)){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
           cout<<"invalid, Enter again:  ";
}
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        // Restrict add/remove flights to admin only
        if ((choice == 1 || choice == 2) && currentUser->role != "admin") {
            cout << "Only admins can add or remove flights.\n";
            continue;
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
        case 8: {
            // Book a Flight
            string source, destination;
            cout << "Enter source airport: "; getline(cin, source);
            cout << "Enter destination airport: "; getline(cin, destination);
            if (!airport.isValidAirport(source) || !airport.isValidAirport(destination)) {
                cout << "Invalid source or destination airport.\n";
                break;
            }
            // Find the distance and price
            int distance = -1, price = -1;
            for (const auto& flight : airport.graph[source]) {
                if (flight.first == destination) {
                    distance = flight.second;
                    break;
                }
            }
            if (distance == -1) {
                cout << "No direct flight found.\n";
                break;
            }
            if (airport.prices.find(source) != airport.prices.end() && airport.prices[source].find(destination) != airport.prices[source].end()) {
                price = airport.prices[source][destination];
            }
            if (price == -1) {
                cout << "Price information not available.\n";
                break;
            }
            Booking booking{currentUser->username, source, destination, distance, price};
            bookings.push_back(booking);
            saveBooking("bookings.txt", booking);
            cout << "Flight booked successfully!\n";
            break;
        }
        case 9: {
            // View My Bookings
            cout << "\nYour Bookings:\n";
            bool found = false;
            for (const auto& b : bookings) {
                if (b.username == currentUser->username) {
                    cout << "From: " << b.source << ", To: " << b.destination << ", Distance: " << b.distance << " km, Price: " << b.price << " rupees\n";
                    found = true;
                }
            }
            if (!found) cout << "No bookings found.\n";
            break;
        }
        case 10: {
            // Cancel a Booking
            cout << "Enter source airport of booking to cancel: ";
            string source; getline(cin, source);
            cout << "Enter destination airport of booking to cancel: ";
            string destination; getline(cin, destination);
            bool removed = false;
            for (auto it = bookings.begin(); it != bookings.end(); ++it) {
                if (it->username == currentUser->username && it->source == source && it->destination == destination) {
                    bookings.erase(it);
                    saveAllBookings("bookings.txt", bookings);
                    cout << "Booking cancelled.\n";
                    removed = true;
                    break;
                }
            }
            if (!removed) cout << "No such booking found.\n";
            break;
        }
        case 11: {
            // Search Flights by Price Range
            int minPrice, maxPrice;
            cout << "Enter minimum price: "; cin >> minPrice;
            cout << "Enter maximum price: "; cin >> maxPrice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            searchFlightsByPrice(airport, minPrice, maxPrice);
            break;
        }
        case 12: {
            // Search Flights by Maximum Distance
            int maxDistance;
            cout << "Enter maximum distance: "; cin >> maxDistance;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            searchFlightsByDistance(airport, maxDistance);
            break;
        }
        default:
            if (choice == 7)
            {
                // Save flights and prices to file
                airport.saveToFile("flights.txt");
                // Save all bookings to file
                saveAllBookings("bookings.txt", bookings);
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
