#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

// Class to represent a patient
class Patient {
public:
    string id;
    char gender;       // 'M' or 'F'
    string arrivalTime; // HH:MM
    string type;       // "Urgent" or "Normal"
    int waitTime;      // Total time waited in minutes

    Patient(string id, char gender, string arrivalTime, string type)
        : id(id), gender(gender), arrivalTime(arrivalTime), type(type), waitTime(0) {}

    // Validates Egyptian National ID format (14 digits)
    static bool validateID(const string& id) {
        return id.length() == 14 && all_of(id.begin(), id.end(), ::isdigit);
    }
};

// Comparator for prioritizing urgent patients in a priority queue
struct UrgentPatientComparator {
    bool operator()(const Patient& a, const Patient& b) const {
        return a.arrivalTime > b.arrivalTime; // Patients with earlier arrival times are served first
    }
};

class PatientSchedulingSystem {
private:
    priority_queue<Patient, vector<Patient>, UrgentPatientComparator> urgentQueue;
    queue<Patient> normalQueue;
    vector<Patient> allPatients;
    vector<Patient> donePatients;

    int totalUrgent = 0;
    int totalNormal = 0;
    double totalWaitTime = 0;

    // Generate random time in HH:MM 24-hour format
    string generateRandomTime() {
        int hours = rand() % 24;
        int minutes = rand() % 60;
        ostringstream oss;
        oss << setw(2) << setfill('0') << hours << ":" << setw(2) << setfill('0') << minutes;
        return oss.str();
    }

    // Generate random ID (14 digits)
    string generateRandomID() {
        ostringstream oss;
        for (int i = 0; i < 14; i++) {
            oss << rand() % 10;
        }
        return oss.str();
    }

    // Generate random type: "Urgent" or "Normal"
    string generateRandomType() {
        return rand() % 2 == 0 ? "Urgent" : "Normal";
    }

public:
    // Populate the list of patients with random data
    void populatePatients(int count) {
        for (int i = 0; i < count; i++) {
            string id = generateRandomID();
            char gender = rand() % 2 == 0 ? 'M' : 'F';
            string arrivalTime = generateRandomTime();
            string type = generateRandomType();

            if (!Patient::validateID(id)) {
                i--; // Skip invalid ID and regenerate
                continue;
            }

            allPatients.emplace_back(id, gender, arrivalTime, type);
        }
    }

    // Dispatch patients to their respective queues based on the current time
    void dispatchPatients(const string& currentTime) {
        auto it = allPatients.begin();
        while (it != allPatients.end()) {
            if (it->arrivalTime <= currentTime) {
                if (it->type == "Urgent") {
                    urgentQueue.push(*it);
                    totalUrgent++;
                }
                else {
                    normalQueue.push(*it);
                    totalNormal++;
                }
                it = allPatients.erase(it); // Remove patient from the vector
            }
            else {
                ++it;
            }
        }
    }

    // Serve patients based on their priority
    void servePatients(int maxToServe) {
        for (int i = 0; i < maxToServe; i++) {
            if (!urgentQueue.empty()) {
                Patient patient = urgentQueue.top();
                urgentQueue.pop();
                servePatient(patient);
            }
            else if (!normalQueue.empty()) {
                Patient patient = normalQueue.front();
                normalQueue.pop();
                servePatient(patient);
            }
            else {
                break;
            }
        }
    }

    // Serve a single patient
    void servePatient(Patient& patient) {
        totalWaitTime += patient.waitTime;
        donePatients.push_back(patient);
        cout << "Serving Patient ID: " << patient.id << ", Type: " << patient.type
            << ", Wait Time: " << patient.waitTime << " minutes.\n";
    }

    // Display the current status of the system
    void displayStatus() {
        cout << "\nWaiting Urgent Patients:\n";
        priority_queue<Patient, vector<Patient>, UrgentPatientComparator> tempUrgentQueue = urgentQueue;
        while (!tempUrgentQueue.empty()) {
            cout << tempUrgentQueue.top().id << " ";
            tempUrgentQueue.pop();
        }

        cout << "\n\nWaiting Normal Patients:\n";
        queue<Patient> tempNormalQueue = normalQueue;
        while (!tempNormalQueue.empty()) {
            cout << tempNormalQueue.front().id << " ";
            tempNormalQueue.pop();
        }

        cout << "\n\nDone Patients:\n";
        for (const auto& patient : donePatients) {
            cout << patient.id << " ";
        }
        cout << "\n";
    }

    // Display the simulation summary
    void displaySummary() {
        cout << "\nSimulation Summary:\n";
        cout << "--------------------------------\n";
        cout << "Total Patients: " << (totalUrgent + totalNormal) << "\n";
        cout << "Urgent Patients: " << totalUrgent << "\n";
        cout << "Normal Patients: " << totalNormal << "\n";
        cout << "Average Waiting Time: " << (donePatients.size() > 0 ? totalWaitTime / donePatients.size() : 0) << " minutes.\n";
    }
};

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed for random number generation

    PatientSchedulingSystem system;

    cout << "Enter the number of patients to simulate (e.g., 100, 300, 700): ";
    int numPatients;
    cin >> numPatients;
    cin.ignore(); // Clear the newline character from the input buffer

    system.populatePatients(numPatients);

    cout << "\nPatient Scheduling System Simulation\n";
    cout << "-------------------------------------\n";

    string currentTime = "00:00"; // Start simulation at midnight
    while (true) {
        cout << "\nCurrent Time: " << currentTime << "\n";
        cout << "Press Enter to advance 1 minute or type 'exit' to stop: ";
        string input;
        getline(cin, input);

        if (input == "exit") break;

        system.dispatchPatients(currentTime);

        // Randomly serve 5 to 10 patients
        system.servePatients(rand() % 6 + 5);

        system.displayStatus();

        // Increment time by 1 minute
        int hours = stoi(currentTime.substr(0, 2));
        int minutes = stoi(currentTime.substr(3, 2)) + 1;
        if (minutes == 60) {
            minutes = 0;
            hours++;
        }
        if (hours == 24) {
            cout << "\nSimulation completed for a full day.\n";
            break;
        }

        ostringstream oss;
        oss << setw(2) << setfill('0') << hours << ":" << setw(2) << setfill('0') << minutes;
        currentTime = oss.str();
    }

    system.displaySummary();

    return 0;
}
