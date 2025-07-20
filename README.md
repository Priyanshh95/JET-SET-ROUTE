# JET-SET-ROUTE

A C++ console application for airport and flight management with user accounts, admin roles, booking, search, statistics, and persistent data storage.

## Features
- User registration and login (with admin/user roles)
- Admins can add or remove flights
- All users can:
  - Book flights between airports
  - View and cancel their bookings
  - Search flights by price range or distance
  - View statistics (most popular routes, busiest airports, user booking stats for admins)
  - Access in-program help
- Data persistence: all flights, users, and bookings are saved in text files
- Robust input validation and error handling

## How to Build and Run

### Windows (using g++)
```
g++ -o jet_set_go.exe jet_set_go.cpp
./jet_set_go.exe
```

### Linux/macOS
```
g++ -o jet_set_go jet_set_go.cpp
./jet_set_go
```

## Usage Instructions
1. **Register or log in** to use the system.
2. **Admins** (first user or with code) can add/remove flights.
3. **All users** can book, view, and cancel flights, search, and view statistics.
4. **Admin code:** To register as admin (if not first user), enter `JETADMIN2024` when prompted during registration.
5. **Data files:**
   - `users.txt` — user accounts and roles
   - `flights.txt` — flights and prices
   - `bookings.txt` — user bookings
6. **Help:** Use the in-program Help menu for instructions at any time.

## Notes
- All data is persistent between runs.
- Input is validated; follow prompts for correct formats.
- For any invalid input, you will be prompted to try again.