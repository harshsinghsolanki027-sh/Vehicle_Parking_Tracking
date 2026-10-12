
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <cmath>
#include <cctype>
#include <limits>

using namespace std;

const int MAX_VEHICLES = 500;
const int MAX_OWNERS = 500;
const int MAX_SLOTS = 100;
const int MAX_RECORDS = 2000;
const string DATA_FILE = "parking_data.txt";

// -------------------- Input and utility functions --------------------

string trim(const string& text)
{
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

string toLowerCase(string text)
{
    for (char& ch : text)
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    return text;
}

bool equalsIgnoreCase(const string& a, const string& b)
{
    return toLowerCase(a) == toLowerCase(b);
}

string readLine(const string& prompt)
{
    cout << prompt;
    string value;
    getline(cin, value);
    return trim(value);
}

int readInt(const string& prompt, int minimum, int maximum)
{
    while (true)
    {
        string line = readLine(prompt);
        stringstream ss(line);
        int value;
        char extra;

        if ((ss >> value) && !(ss >> extra) && value >= minimum && value <= maximum)
            return value;

        cout << "Invalid input. Enter a whole number from "
             << minimum << " to " << maximum << ".\n";
    }
}

bool isValidVehicleType(const string& type)
{
    return equalsIgnoreCase(type, "Car") ||
           equalsIgnoreCase(type, "Bike") ||
           equalsIgnoreCase(type, "Other");
}

string normaliseVehicleType(const string& type)
{
    if (equalsIgnoreCase(type, "Car")) return "Car";
    if (equalsIgnoreCase(type, "Bike")) return "Bike";
    return "Other";
}

string formatTime(long long timestamp)
{
    if (timestamp <= 0) return "Not exited";

    time_t rawTime = static_cast<time_t>(timestamp);
    tm* timeInfo = localtime(&rawTime);
    if (timeInfo == nullptr) return "Unknown";

    char buffer[30];
    if (strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeInfo) == 0)
        return "Unknown";

    return string(buffer);
}

bool isBlank(const string& value)
{
    return trim(value).empty();
}

// -------------------- Base class: Person --------------------
// Owner inherits from Person. The virtual function demonstrates
// runtime polymorphism when displayDetails() is called through Person*.

class Person
{
protected:
    string name;

public:
    Person() : name("") {}

    explicit Person(const string& personName) : name(personName) {}

    virtual void displayDetails() const
    {
        cout << "Name: " << name << '\n';
    }

    virtual ~Person() = default;

    string getName() const { return name; }
    void setName(const string& personName) { name = personName; }
};

// -------------------- Vehicle class --------------------

class Vehicle
{
private:
    string vehicleNo;
    string type;
    string ownerName;

public:
    Vehicle() : vehicleNo(""), type("Other"), ownerName("") {}

    Vehicle(const string& number, const string& vehicleType, const string& name)
        : vehicleNo(number), type(normaliseVehicleType(vehicleType)), ownerName(name) {}

    void display() const
    {
        cout << left << setw(18) << vehicleNo
             << setw(12) << type
             << ownerName << '\n';
    }

    string getVehicleNo() const { return vehicleNo; }
    string getType() const { return type; }
    string getOwnerName() const { return ownerName; }

    void setType(const string& vehicleType) { type = normaliseVehicleType(vehicleType); }
    void setOwnerName(const string& name) { ownerName = name; }
};

// -------------------- Owner class --------------------

class Owner : public Person
{
private:
    string ownerId;
    string phone;

public:
    Owner() : Person(), ownerId(""), phone("") {}

    Owner(const string& id, const string& ownerName, const string& ownerPhone)
        : Person(ownerName), ownerId(id), phone(ownerPhone) {}

    void displayDetails() const override
    {
        cout << "Owner ID: " << ownerId << '\n';
        cout << "Owner Name: " << name << '\n';
        cout << "Phone: " << phone << '\n';
    }

    string getOwnerId() const { return ownerId; }
    string getPhone() const { return phone; }
    void setPhone(const string& newPhone) { phone = newPhone; }
};

// -------------------- Slot class --------------------

class Slot
{
private:
    int slotId;
    string type;
    bool occupied;

public:
    Slot() : slotId(0), type("Any"), occupied(false) {}

    Slot(int id, const string& slotType, bool isOccupied = false)
        : slotId(id), type(slotType), occupied(isOccupied) {}

    void reserve() { occupied = true; }
    void release() { occupied = false; }

    void display() const
    {
        cout << "Slot " << slotId << " | Type: " << type
             << " | Status: " << (occupied ? "Occupied" : "Available") << '\n';
    }

    int getSlotId() const { return slotId; }
    string getType() const { return type; }
    bool isOccupied() const { return occupied; }
    void setOccupied(bool value) { occupied = value; }
};

// -------------------- ParkingRecord class --------------------

class ParkingRecord
{
private:
    int recordId;
    string vehicleNo;
    int slotId;
    long long entryTime;
    long long exitTime;
    double fee;
    bool active;

public:
    ParkingRecord()
        : recordId(0), vehicleNo(""), slotId(0), entryTime(0),
          exitTime(0), fee(0.0), active(false) {}

    ParkingRecord(int id, const string& number, int parkingSlot,
                  long long entry, long long exit, double parkingFee, bool isActive)
        : recordId(id), vehicleNo(number), slotId(parkingSlot),
          entryTime(entry), exitTime(exit), fee(parkingFee), active(isActive) {}

    double calculateFee(double hourlyRate) const
    {
        long long seconds = exitTime - entryTime;
        if (seconds < 0) seconds = 0;

        // Any started hour is charged as a full hour; minimum charge is one hour.
        long long hours = max(1LL, static_cast<long long>(ceil(seconds / 3600.0)));
        return hours * hourlyRate;
    }

    void closeRecord(long long exitTimestamp, double hourlyRate)
    {
        if (active)
        {
            exitTime = exitTimestamp;
            fee = calculateFee(hourlyRate);
            active = false;
        }
    }

    void display() const
    {
        cout << "Record ID: " << recordId
             << " | Vehicle: " << vehicleNo
             << " | Slot: " << slotId << '\n';
        cout << "  Entry: " << formatTime(entryTime) << '\n';
        cout << "  Exit:  " << (active ? "Still parked" : formatTime(exitTime)) << '\n';
        cout << "  Status: " << (active ? "ACTIVE" : "CLOSED")
             << " | Fee: Rs. " << fixed << setprecision(2) << fee << "\n";
    }

    int getRecordId() const { return recordId; }
    string getVehicleNo() const { return vehicleNo; }
    int getSlotId() const { return slotId; }
    long long getEntryTime() const { return entryTime; }
    long long getExitTime() const { return exitTime; }
    double getFee() const { return fee; }
    bool isActive() const { return active; }
};

// Forward declaration needed by FileManager.
class ParkingSystem;

// -------------------- FileManager class --------------------

class FileManager
{
public:
    static bool save(const ParkingSystem& system, const string& fileName);
    static bool load(ParkingSystem& system, const string& fileName);
};

// -------------------- ParkingSystem class --------------------
// Dynamic arrays are allocated with new[] and released with delete[].
// Capacity is fixed for this guided project and checked before insertion.

class ParkingSystem
{
private:
    Vehicle* vehicles;
    Owner* owners;
    Slot* slots;
    ParkingRecord* records;

    int vehicleCount;
    int ownerCount;
    int slotCount;
    int recordCount;

    friend class FileManager;

    int findVehicleIndex(const string& vehicleNo) const
    {
        for (int i = 0; i < vehicleCount; ++i)
            if (equalsIgnoreCase(vehicles[i].getVehicleNo(), vehicleNo))
                return i;
        return -1;
    }

    int findOwnerIndex(const string& ownerId) const
    {
        for (int i = 0; i < ownerCount; ++i)
            if (equalsIgnoreCase(owners[i].getOwnerId(), ownerId))
                return i;
        return -1;
    }

    int findSlotIndex(int slotId) const
    {
        for (int i = 0; i < slotCount; ++i)
            if (slots[i].getSlotId() == slotId)
                return i;
        return -1;
    }

    int findActiveRecordIndex(const string& vehicleNo) const
    {
        for (int i = recordCount - 1; i >= 0; --i)
            if (records[i].isActive() &&
                equalsIgnoreCase(records[i].getVehicleNo(), vehicleNo))
                return i;
        return -1;
    }

    int nextRecordId() const
    {
        int highest = 0;
        for (int i = 0; i < recordCount; ++i)
            if (records[i].getRecordId() > highest)
                highest = records[i].getRecordId();
        return highest + 1;
    }

    double hourlyRateFor(const string& vehicleType) const
    {
        if (equalsIgnoreCase(vehicleType, "Car")) return 30.0;
        if (equalsIgnoreCase(vehicleType, "Bike")) return 15.0;
        return 20.0;
    }

    int findAvailableSlot(const string& vehicleType) const
    {
        for (int i = 0; i < slotCount; ++i)
        {
            bool typeMatches = equalsIgnoreCase(slots[i].getType(), "Any") ||
                               equalsIgnoreCase(slots[i].getType(), vehicleType);
            if (!slots[i].isOccupied() && typeMatches)
                return i;
        }
        return -1;
    }

    void addDefaultSlots()
    {
        if (slotCount != 0) return;

        for (int i = 1; i <= 5 && slotCount < MAX_SLOTS; ++i)
            slots[slotCount++] = Slot(i, "Car", false);

        for (int i = 6; i <= 10 && slotCount < MAX_SLOTS; ++i)
            slots[slotCount++] = Slot(i, "Bike", false);

        for (int i = 11; i <= 12 && slotCount < MAX_SLOTS; ++i)
            slots[slotCount++] = Slot(i, "Other", false);
    }

    void removeRecordAt(int index)
    {
        if (index < 0 || index >= recordCount) return;
        for (int i = index; i < recordCount - 1; ++i)
            records[i] = records[i + 1];
        --recordCount;
    }

public:
    ParkingSystem()
        : vehicles(new Vehicle[MAX_VEHICLES]),
          owners(new Owner[MAX_OWNERS]),
          slots(new Slot[MAX_SLOTS]),
          records(new ParkingRecord[MAX_RECORDS]),
          vehicleCount(0), ownerCount(0), slotCount(0), recordCount(0)
    {
        addDefaultSlots();
    }

    // Prevent accidental shallow copies of dynamically allocated arrays.
    ParkingSystem(const ParkingSystem&) = delete;
    ParkingSystem& operator=(const ParkingSystem&) = delete;

    ~ParkingSystem()
    {
        delete[] vehicles;
        delete[] owners;
        delete[] slots;
        delete[] records;
    }

    void addRecord()
    {
        cout << "\n--- Add Vehicle / Owner Record ---\n";
        string number = readLine("Vehicle registration number: ");
        if (isBlank(number))
        {
            cout << "Vehicle number cannot be empty.\n";
            return;
        }
        if (findVehicleIndex(number) != -1)
        {
            cout << "A vehicle with this registration number already exists.\n";
            return;
        }
        if (vehicleCount >= MAX_VEHICLES || ownerCount >= MAX_OWNERS)
        {
            cout << "Storage capacity reached. Cannot add another vehicle.\n";
            return;
        }

        string type = readLine("Vehicle type (Car/Bike/Other): ");
        if (!isValidVehicleType(type))
        {
            cout << "Invalid type. Enter Car, Bike, or Other.\n";
            return;
        }

        string ownerName = readLine("Owner name: ");
        if (isBlank(ownerName))
        {
            cout << "Owner name cannot be empty.\n";
            return;
        }

        string phone = readLine("Owner phone: ");
        if (isBlank(phone))
        {
            cout << "Phone cannot be empty.\n";
            return;
        }

        vehicles[vehicleCount++] = Vehicle(number, type, ownerName);
        // Vehicle registration number is used as the owner's linked ID.
        owners[ownerCount++] = Owner(number, ownerName, phone);

        cout << "Vehicle and owner record added successfully.\n";
    }

    void displayRecords() const
    {
        cout << "\n========== VEHICLE RECORDS ==========\n";
        if (vehicleCount == 0)
            cout << "No vehicles registered yet.\n";
        else
        {
            cout << left << setw(18) << "Vehicle Number"
                 << setw(12) << "Type" << "Owner Name\n";
            cout << string(55, '-') << '\n';
            for (int i = 0; i < vehicleCount; ++i)
                vehicles[i].display();
        }

        cout << "\n========== PARKING HISTORY ==========\n";
        if (recordCount == 0)
            cout << "No parking transactions yet.\n";
        else
            for (int i = 0; i < recordCount; ++i)
                records[i].display();
    }

    void searchRecord() const
    {
        cout << "\n--- Search Record ---\n";
        string query = readLine("Enter vehicle number or parking record ID: ");
        if (isBlank(query))
        {
            cout << "Search value cannot be empty.\n";
            return;
        }

        bool found = false;
        int vehicleIndex = findVehicleIndex(query);
        if (vehicleIndex != -1)
        {
            found = true;
            cout << "\nVehicle found:\n";
            vehicles[vehicleIndex].display();

            int ownerIndex = findOwnerIndex(vehicles[vehicleIndex].getVehicleNo());
            if (ownerIndex != -1)
            {
                cout << "\nOwner details (called through Person* to demonstrate runtime polymorphism):\n";
                const Person* person = &owners[ownerIndex];
                person->displayDetails();
            }

            cout << "\nRelated parking records:\n";
            bool hasRecords = false;
            for (int i = 0; i < recordCount; ++i)
            {
                if (equalsIgnoreCase(records[i].getVehicleNo(), query))
                {
                    records[i].display();
                    hasRecords = true;
                }
            }
            if (!hasRecords) cout << "No parking transactions for this vehicle yet.\n";
        }
        else
        {
            stringstream ss(query);
            int id;
            char extra;
            if ((ss >> id) && !(ss >> extra))
            {
                for (int i = 0; i < recordCount; ++i)
                {
                    if (records[i].getRecordId() == id)
                    {
                        records[i].display();
                        found = true;
                        break;
                    }
                }
            }
        }

        if (!found) cout << "No matching record found.\n";
    }

    void updateRecord()
    {
        cout << "\n--- Update Vehicle / Owner Record ---\n";
        string number = readLine("Enter vehicle number to update: ");
        int vehicleIndex = findVehicleIndex(number);

        if (vehicleIndex == -1)
        {
            cout << "Vehicle not found.\n";
            return;
        }
        if (findActiveRecordIndex(number) != -1)
        {
            cout << "This vehicle is currently parked. Exit it before changing its details.\n";
            return;
        }

        int ownerIndex = findOwnerIndex(number);
        cout << "Press Enter to keep an existing value.\n";

        string newType = readLine("New vehicle type (Car/Bike/Other): ");
        if (!newType.empty())
        {
            if (!isValidVehicleType(newType))
            {
                cout << "Invalid vehicle type. Update cancelled.\n";
                return;
            }
            vehicles[vehicleIndex].setType(newType);
        }

        string newName = readLine("New owner name: ");
        if (!newName.empty())
        {
            vehicles[vehicleIndex].setOwnerName(newName);
            if (ownerIndex != -1) owners[ownerIndex].setName(newName);
        }

        string newPhone = readLine("New owner phone: ");
        if (!newPhone.empty() && ownerIndex != -1)
            owners[ownerIndex].setPhone(newPhone);

        cout << "Record updated successfully.\n";
    }

    void deleteRecord()
    {
        cout << "\n--- Delete Vehicle Record ---\n";
        string number = readLine("Enter vehicle number to delete: ");
        int vehicleIndex = findVehicleIndex(number);

        if (vehicleIndex == -1)
        {
            cout << "Vehicle not found.\n";
            return;
        }
        if (findActiveRecordIndex(number) != -1)
        {
            cout << "Cannot delete a vehicle while it is parked. Exit it first.\n";
            return;
        }

        string confirm = readLine("Type YES to confirm deletion: ");
        if (confirm != "YES")
        {
            cout << "Deletion cancelled.\n";
            return;
        }

        for (int i = vehicleIndex; i < vehicleCount - 1; ++i)
            vehicles[i] = vehicles[i + 1];
        --vehicleCount;

        int ownerIndex = findOwnerIndex(number);
        if (ownerIndex != -1)
        {
            for (int i = ownerIndex; i < ownerCount - 1; ++i)
                owners[i] = owners[i + 1];
            --ownerCount;
        }

        // Remove closed history belonging to this vehicle as part of deletion.
        for (int i = recordCount - 1; i >= 0; --i)
            if (equalsIgnoreCase(records[i].getVehicleNo(), number))
                removeRecordAt(i);

        cout << "Vehicle, linked owner, and related parking history deleted.\n";
    }

    void parkVehicle()
    {
        cout << "\n--- Park Vehicle (Entry Transaction) ---\n";
        string number = readLine("Enter registered vehicle number: ");
        int vehicleIndex = findVehicleIndex(number);

        if (vehicleIndex == -1)
        {
            cout << "Vehicle is not registered. Use Add Record first.\n";
            return;
        }
        if (findActiveRecordIndex(number) != -1)
        {
            cout << "This vehicle is already parked.\n";
            return;
        }
        if (recordCount >= MAX_RECORDS)
        {
            cout << "Parking record capacity reached.\n";
            return;
        }

        int slotIndex = findAvailableSlot(vehicles[vehicleIndex].getType());
        if (slotIndex == -1)
        {
            cout << "No suitable parking slot is currently available.\n";
            return;
        }

        long long now = static_cast<long long>(time(nullptr));
        int id = nextRecordId();
        records[recordCount++] = ParkingRecord(
            id, vehicles[vehicleIndex].getVehicleNo(),
            slots[slotIndex].getSlotId(), now, 0, 0.0, true);
        slots[slotIndex].reserve();

        cout << "Vehicle parked successfully.\n";
        cout << "Parking Record ID: " << id << '\n';
        cout << "Allocated Slot: " << slots[slotIndex].getSlotId() << '\n';
        cout << "Entry Time: " << formatTime(now) << '\n';
    }

    void exitVehicle()
    {
        cout << "\n--- Exit Vehicle (Payment Transaction) ---\n";
        string number = readLine("Enter vehicle number: ");
        int recordIndex = findActiveRecordIndex(number);

        if (recordIndex == -1)
        {
            cout << "No active parking record found for this vehicle.\n";
            return;
        }

        int vehicleIndex = findVehicleIndex(number);
        if (vehicleIndex == -1)
        {
            cout << "Vehicle details are missing; cannot calculate fee.\n";
            return;
        }

        long long now = static_cast<long long>(time(nullptr));
        records[recordIndex].closeRecord(now, hourlyRateFor(vehicles[vehicleIndex].getType()));

        int slotIndex = findSlotIndex(records[recordIndex].getSlotId());
        if (slotIndex != -1) slots[slotIndex].release();

        cout << "\nVehicle exit completed.\n";
        cout << "Entry: " << formatTime(records[recordIndex].getEntryTime()) << '\n';
        cout << "Exit:  " << formatTime(records[recordIndex].getExitTime()) << '\n';
        cout << "Fee: Rs. " << fixed << setprecision(2)
             << records[recordIndex].getFee() << '\n';
        cout << "Fee rule: each started hour is charged as a full hour; minimum one hour.\n";
    }

    void mainTransaction()
    {
        while (true)
        {
            cout << "\n========== PARKING TRANSACTION ==========\n";
            cout << "1. Park Vehicle (Entry)\n";
            cout << "2. Exit Vehicle (Calculate Fee)\n";
            cout << "3. Show Active Parking\n";
            cout << "4. Back to Main Menu\n";

            int choice = readInt("Choose an option: ", 1, 4);
            if (choice == 1) parkVehicle();
            else if (choice == 2) exitVehicle();
            else if (choice == 3) showActiveParking();
            else break;
        }
    }

    void showActiveParking() const
    {
        cout << "\n--- Currently Parked Vehicles ---\n";
        bool found = false;
        for (int i = 0; i < recordCount; ++i)
        {
            if (records[i].isActive())
            {
                records[i].display();
                found = true;
            }
        }
        if (!found) cout << "No vehicles are currently parked.\n";
    }

    void report() const
    {
        int occupied = 0;
        int active = 0;
        int completed = 0;
        double revenue = 0.0;

        for (int i = 0; i < slotCount; ++i)
            if (slots[i].isOccupied()) ++occupied;

        for (int i = 0; i < recordCount; ++i)
        {
            if (records[i].isActive()) ++active;
            else
            {
                ++completed;
                revenue += records[i].getFee();
            }
        }

        cout << "\n========== PARKING REPORT ==========\n";
        cout << "Registered vehicles : " << vehicleCount << '\n';
        cout << "Total parking slots : " << slotCount << '\n';
        cout << "Available slots     : " << (slotCount - occupied) << '\n';
        cout << "Occupied slots      : " << occupied << '\n';
        cout << "Active transactions : " << active << '\n';
        cout << "Completed records   : " << completed << '\n';
        cout << "Total collected fees: Rs. " << fixed << setprecision(2) << revenue << '\n';

        cout << "\n--- Slot Status ---\n";
        for (int i = 0; i < slotCount; ++i)
            slots[i].display();
    }

    void saveData()
    {
        if (FileManager::save(*this, DATA_FILE))
            cout << "Data saved successfully to " << DATA_FILE << ".\n";
        else
            cout << "Error: data could not be saved. Check folder permissions.\n";
    }

    void loadData()
    {
        if (FileManager::load(*this, DATA_FILE))
            cout << "Data loaded successfully from " << DATA_FILE << ".\n";
        else
            cout << "No saved data file found (or it could not be read). Current data was kept.\n";
    }

    void saveLoadMenu()
    {
        while (true)
        {
            cout << "\n========== SAVE / LOAD ==========\n";
            cout << "1. Save Data\n";
            cout << "2. Load Data (replaces current in-memory data)\n";
            cout << "3. Back to Main Menu\n";

            int choice = readInt("Choose an option: ", 1, 3);
            if (choice == 1) saveData();
            else if (choice == 2)
            {
                string confirm = readLine("Unsaved changes may be replaced. Continue? (YES/NO): ");
                if (confirm == "YES") loadData();
                else cout << "Load cancelled.\n";
            }
            else break;
        }
    }

    void run()
    {
        loadData();
        cout << "\nWelcome to Vehicle Parking & Tracking System.\n";

        bool running = true;
        while (running)
        {
            cout << "\n========================================\n";
            cout << "     VEHICLE PARKING & TRACKING\n";
            cout << "========================================\n";
            cout << "1. Add Record (Vehicle / Owner)\n";
            cout << "2. Display Records\n";
            cout << "3. Search Record\n";
            cout << "4. Update Record\n";
            cout << "5. Delete Record\n";
            cout << "6. Main Transaction / Process\n";
            cout << "7. Report\n";
            cout << "8. Save / Load\n";
            cout << "9. Exit\n";

            int choice = readInt("Enter your choice (1-9): ", 1, 9);

            try
            {
                switch (choice)
                {
                    case 1: addRecord(); break;
                    case 2: displayRecords(); break;
                    case 3: searchRecord(); break;
                    case 4: updateRecord(); break;
                    case 5: deleteRecord(); break;
                    case 6: mainTransaction(); break;
                    case 7: report(); break;
                    case 8: saveLoadMenu(); break;
                    case 9:
                        saveData();
                        cout << "Thank you for using the system. Goodbye!\n";
                        running = false;
                        break;
                }
            }
            catch (const exception& ex)
            {
                cout << "An unexpected error occurred: " << ex.what() << '\n';
            }
            catch (...)
            {
                cout << "An unknown error occurred. Please try again.\n";
            }
        }
    }
};

// -------------------- FileManager implementation --------------------
// Text persistence uses std::quoted so names containing spaces are preserved.

bool FileManager::save(const ParkingSystem& system, const string& fileName)
{
    ofstream out(fileName);
    if (!out.is_open()) return false;

    out << "VEHICLE_COUNT " << system.vehicleCount << '\n';
    for (int i = 0; i < system.vehicleCount; ++i)
    {
        out << "VEHICLE "
            << quoted(system.vehicles[i].getVehicleNo()) << ' '
            << quoted(system.vehicles[i].getType()) << ' '
            << quoted(system.vehicles[i].getOwnerName()) << '\n';
    }

    out << "OWNER_COUNT " << system.ownerCount << '\n';
    for (int i = 0; i < system.ownerCount; ++i)
    {
        out << "OWNER "
            << quoted(system.owners[i].getOwnerId()) << ' '
            << quoted(system.owners[i].getName()) << ' '
            << quoted(system.owners[i].getPhone()) << '\n';
    }

    out << "SLOT_COUNT " << system.slotCount << '\n';
    for (int i = 0; i < system.slotCount; ++i)
    {
        out << "SLOT " << system.slots[i].getSlotId() << ' '
            << quoted(system.slots[i].getType()) << ' '
            << (system.slots[i].isOccupied() ? 1 : 0) << '\n';
    }

    out << "RECORD_COUNT " << system.recordCount << '\n';
    out << fixed << setprecision(2);
    for (int i = 0; i < system.recordCount; ++i)
    {
        out << "RECORD " << system.records[i].getRecordId() << ' '
            << quoted(system.records[i].getVehicleNo()) << ' '
            << system.records[i].getSlotId() << ' '
            << system.records[i].getEntryTime() << ' '
            << system.records[i].getExitTime() << ' '
            << system.records[i].getFee() << ' '
            << (system.records[i].isActive() ? 1 : 0) << '\n';
    }

    return out.good();
}

bool FileManager::load(ParkingSystem& system, const string& fileName)
{
    ifstream in(fileName);
    if (!in.is_open()) return false;

    // Clear existing in-memory data only after confirming the file can be opened.
    system.vehicleCount = 0;
    system.ownerCount = 0;
    system.slotCount = 0;
    system.recordCount = 0;

    string tag;
    while (in >> tag)
    {
        if (tag == "VEHICLE_COUNT" || tag == "OWNER_COUNT" ||
            tag == "SLOT_COUNT" || tag == "RECORD_COUNT")
        {
            int ignoredCount;
            if (!(in >> ignoredCount)) return false;
        }
        else if (tag == "VEHICLE")
        {
            string number, type, ownerName;
            if (!(in >> quoted(number) >> quoted(type) >> quoted(ownerName))) return false;
            if (system.vehicleCount < MAX_VEHICLES)
                system.vehicles[system.vehicleCount++] = Vehicle(number, type, ownerName);
        }
        else if (tag == "OWNER")
        {
            string id, name, phone;
            if (!(in >> quoted(id) >> quoted(name) >> quoted(phone))) return false;
            if (system.ownerCount < MAX_OWNERS)
                system.owners[system.ownerCount++] = Owner(id, name, phone);
        }
        else if (tag == "SLOT")
        {
            int id, occupied;
            string type;
            if (!(in >> id >> quoted(type) >> occupied)) return false;
            if (system.slotCount < MAX_SLOTS)
                system.slots[system.slotCount++] = Slot(id, type, occupied != 0);
        }
        else if (tag == "RECORD")
        {
            int id, slotId, active;
            string vehicleNo;
            long long entry, exit;
            double fee;
            if (!(in >> id >> quoted(vehicleNo) >> slotId >> entry >> exit >> fee >> active))
                return false;
            if (system.recordCount < MAX_RECORDS)
                system.records[system.recordCount++] =
                    ParkingRecord(id, vehicleNo, slotId, entry, exit, fee, active != 0);
        }
        else
        {
            // Unknown tags indicate a damaged or incompatible data file.
            return false;
        }
    }

    if (system.slotCount == 0)
        system.addDefaultSlots();

    // Reconcile occupied slots with active records to keep data consistent.
    for (int i = 0; i < system.slotCount; ++i)
        system.slots[i].release();

    for (int i = 0; i < system.recordCount; ++i)
    {
        if (system.records[i].isActive())
        {
            int slotIndex = system.findSlotIndex(system.records[i].getSlotId());
            if (slotIndex != -1) system.slots[slotIndex].reserve();
        }
    }

    return true;
}

// -------------------- main() --------------------

int main()
{
    try
    {
        ParkingSystem system;
        system.run();
    }
    catch (const bad_alloc&)
    {
        cerr << "Error: not enough memory to start the application.\n";
        return 1;
    }
    catch (const exception& ex)
    {
        cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
