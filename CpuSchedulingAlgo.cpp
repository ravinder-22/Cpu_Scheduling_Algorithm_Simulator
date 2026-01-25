#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <queue>
#include <tuple>
#include <string>

// Formatting colors
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"

using namespace std;

struct Process {
    int pid;
    int arrival;
    int burst;
    int remaining;
    int priority;
    int completion;
    int waiting;
    int turnaround;
    int start;
};

// Sort primarily by Arrival Time, secondarily by PID
bool cmpArrival(const Process& a, const Process& b) {
    if (a.arrival != b.arrival)
        return a.arrival < b.arrival;
    return a.pid < b.pid;
}

// ---------------- Helper Functions ----------------

void printtable(vector<Process>& v) {
    // Sort by PID for a clean final table display
    vector<Process> display = v;
    sort(display.begin(), display.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

    cout << "\n" << YELLOW << "Process Execution Table:" << RESET << "\n";
    cout << "+-----+---------+-------+----------+-----+-----+-----+\n";
    cout << "| PID | Arrival | Burst | Priority |  CT | TAT |  WT |\n";
    cout << "+-----+---------+-------+----------+-----+-----+-----+\n";

    double totalWT = 0, totalTAT = 0;
    for (const auto &ele : display) {
        cout << "| " << setw(3) << ele.pid << " | "
             << setw(7) << ele.arrival << " | "
             << setw(5) << ele.burst << " | "
             << setw(8) << ele.priority << " | "
             << setw(3) << ele.completion << " | "
             << setw(3) << ele.turnaround << " | "
             << setw(3) << ele.waiting << " |\n";
        totalWT += ele.waiting;
        totalTAT += ele.turnaround;
    }
    cout << "+-----+---------+-------+----------+-----+-----+-----+\n";
    
    cout << fixed << setprecision(2);
    cout << "Average Turnaround Time: " << totalTAT / v.size() << "\n";
    cout << "Average Waiting Time:    " << totalWT / v.size() << "\n";
}

void ganttchart(const vector<pair<string, int>>& gantt) {
    cout << "\n" << YELLOW << "Gantt Chart:" << RESET << "\n";
    
    // Merge consecutive identical processes for cleaner display
    vector<pair<string, int>> merged;
    if (!gantt.empty()) {
        merged.push_back(gantt[0]);
        for (size_t i = 1; i < gantt.size(); i++) {
            if (gantt[i].first == merged.back().first) {
                merged.back().second = gantt[i].second; // Extend time
            } else {
                merged.push_back(gantt[i]);
            }
        }
    }

    cout << "+---------+------------+-----------------+\n";
    cout << "| Process | Start Time | Completion Time |\n";
    cout << "+---------+------------+-----------------+\n";

    int prev_time = 0;
    // Handle case where first process doesn't start at 0
    if(!merged.empty() && merged[0].first == "IDLE" && merged[0].second > 0) {
        // IDLE block handles the gap, prev_time starts at 0 naturally
    } 
    else if (!merged.empty()) {
        // Use the start time implied by the first block logic if strictly needed,
        // but usually simulation starts at 0.
    }

    for (const auto &ele : merged) {
        cout << "| " << setw(7) << ele.first << " | "
             << setw(10) << prev_time << " | "
             << setw(15) << ele.second << " |\n";
        prev_time = ele.second;
    }
    cout << "+---------+------------+-----------------+\n";
}

// ---------------- Scheduling Algorithms ----------------

void FCFS(vector<Process> v) {
    sort(v.begin(), v.end(), cmpArrival);
    int time = 0;
    vector<pair<string, int>> gantt;

    for (auto &ele : v) {
        // If CPU is idle before this process arrives
        if (ele.arrival > time) {
            gantt.push_back({"IDLE", ele.arrival});
            time = ele.arrival;
        }

        ele.start = time;
        time += ele.burst;
        ele.completion = time;
        ele.turnaround = ele.completion - ele.arrival;
        ele.waiting = ele.turnaround - ele.burst;
        gantt.push_back({"P" + to_string(ele.pid), ele.completion});
    }

    cout << endl << CYAN << "================ FCFS Scheduling ================\n" << RESET;
    ganttchart(gantt);
    printtable(v);
}

void SJF_NonPreemptive(vector<Process> v) {
    int n = v.size();
    sort(v.begin(), v.end(), cmpArrival);
    
    // Min-heap pair: {Burst Time, Index}
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    vector<pair<string, int>> gantt;
    vector<bool> is_completed(n, false);
    
    int time = 0;
    int completed = 0;
    int i = 0; // Index for iterating through sorted processes

    while (completed < n) {
        // Add all processes that have arrived by current time
        while (i < n && v[i].arrival <= time) {
            pq.push({v[i].burst, i});
            i++;
        }

        if (pq.empty()) {
            // No process available, move time to next arrival
            if (i < n) {
                int next_arrival = v[i].arrival;
                gantt.push_back({"IDLE", next_arrival});
                time = next_arrival;
            }
        } else {
            auto top = pq.top();
            pq.pop();
            int idx = top.second;

            v[idx].start = time;
            time += v[idx].burst;
            v[idx].completion = time;
            v[idx].turnaround = v[idx].completion - v[idx].arrival;
            v[idx].waiting = v[idx].turnaround - v[idx].burst;
            is_completed[idx] = true;
            completed++;

            gantt.push_back({"P" + to_string(v[idx].pid), time});
        }
    }

    cout << endl << CYAN << "========== SJF (Non-Preemptive) Scheduling ==========\n" << RESET;
    ganttchart(gantt);
    printtable(v);
}

void SJF_Preemptive(vector<Process> v) {
    int n = v.size();
    sort(v.begin(), v.end(), cmpArrival);
    
    // Initialize remaining times
    for(auto &p : v) {
        p.remaining = p.burst;
        p.start = -1;
    }

    // Min-heap pair: {Remaining Time, Index}
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    vector<pair<string, int>> gantt;
    
    int time = 0;
    int completed = 0;
    int i = 0;

    while (completed < n) {
        // Push new arrivals
        while (i < n && v[i].arrival <= time) {
            pq.push({v[i].remaining, i});
            i++;
        }

        if (pq.empty()) {
            time++;
            gantt.push_back({"IDLE", time});
        } else {
            auto top = pq.top();
            pq.pop();
            int idx = top.second;

            if (v[idx].start == -1) v[idx].start = time;

            // Execute for 1 unit
            v[idx].remaining--;
            time++;
            gantt.push_back({"P" + to_string(v[idx].pid), time});

            if (v[idx].remaining == 0) {
                completed++;
                v[idx].completion = time;
                v[idx].turnaround = v[idx].completion - v[idx].arrival;
                v[idx].waiting = v[idx].turnaround - v[idx].burst;
            } else {
                // Push back with updated remaining time
                pq.push({v[idx].remaining, idx});
            }
        }
    }

    cout << endl << CYAN << "========== SJF Preemptive (SRTF) Scheduling ==========\n" << RESET;
    ganttchart(gantt);
    printtable(v);
}

void Priority_NonPreemptive(vector<Process> v) {
    int n = v.size();
    sort(v.begin(), v.end(), cmpArrival);

    // Min-heap tuple: {Priority, Arrival, Index}
    // Assumes Lower Number = Higher Priority
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> pq;
    vector<pair<string, int>> gantt;

    int time = 0;
    int completed = 0;
    int i = 0;

    while (completed < n) {
        while (i < n && v[i].arrival <= time) {
            pq.push({v[i].priority, v[i].arrival, i});
            i++;
        }

        if (pq.empty()) {
             if (i < n) {
                int next_arrival = v[i].arrival;
                gantt.push_back({"IDLE", next_arrival});
                time = next_arrival;
            }
        } else {
            auto top = pq.top();
            pq.pop();
            int idx = get<2>(top);

            v[idx].start = time;
            time += v[idx].burst;
            v[idx].completion = time;
            v[idx].turnaround = v[idx].completion - v[idx].arrival;
            v[idx].waiting = v[idx].turnaround - v[idx].burst;
            completed++;

            gantt.push_back({"P" + to_string(v[idx].pid), time});
        }
    }

    cout << endl << CYAN << "======= Priority (Non-Preemptive) Scheduling =======\n" << RESET;
    ganttchart(gantt);
    printtable(v);
}

void Priority_Preemptive(vector<Process> v) {
    int n = v.size();
    sort(v.begin(), v.end(), cmpArrival);

    for(auto &p : v) {
        p.remaining = p.burst;
        p.start = -1;
    }

    // Min-heap tuple: {Priority, Arrival, Index}
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> pq;
    vector<pair<string, int>> gantt;

    int time = 0;
    int completed = 0;
    int i = 0;

    while (completed < n) {
        while (i < n && v[i].arrival <= time) {
            pq.push({v[i].priority, v[i].arrival, i});
            i++;
        }

        if (pq.empty()) {
            time++;
            gantt.push_back({"IDLE", time});
        } else {
            auto top = pq.top();
            pq.pop();
            int idx = get<2>(top);

            if (v[idx].start == -1) v[idx].start = time;

            v[idx].remaining--;
            time++;
            gantt.push_back({"P" + to_string(v[idx].pid), time});

            if (v[idx].remaining == 0) {
                completed++;
                v[idx].completion = time;
                v[idx].turnaround = v[idx].completion - v[idx].arrival;
                v[idx].waiting = v[idx].turnaround - v[idx].burst;
            } else {
                pq.push({v[idx].priority, v[idx].arrival, idx});
            }
        }
    }

    cout << endl << CYAN << "======= Priority (Preemptive) Scheduling =======\n" << RESET;
    ganttchart(gantt);
    printtable(v);
}

void RoundRobin(vector<Process> v, int quantum) {
    int n = v.size();
    sort(v.begin(), v.end(), cmpArrival);

    queue<int> q;
    vector<bool> inQueue(n, false);
    vector<pair<string, int>> gantt;

    for(auto &p : v) {
        p.remaining = p.burst;
        p.start = -1;
    }

    int time = 0;
    int completed = 0;
    int i = 0;

    // Initial fill
    while(i < n && v[i].arrival <= time) {
        q.push(i);
        inQueue[i] = true;
        i++;
    }

    while (completed < n) {
        if (q.empty()) {
            time++;
            // Check for new arrivals during the idle tick
            while(i < n && v[i].arrival <= time) {
                q.push(i);
                inQueue[i] = true;
                i++;
            }
            gantt.push_back({"IDLE", time});
        } else {
            int idx = q.front();
            q.pop();

            if (v[idx].start == -1) v[idx].start = time;

            int exec_time = min(quantum, v[idx].remaining);
            v[idx].remaining -= exec_time;
            time += exec_time;
            
            gantt.push_back({"P" + to_string(v[idx].pid), time});

            // CRITICAL: Check for new arrivals *before* re-queuing current process
            while(i < n && v[i].arrival <= time) {
                if (!inQueue[i]) {
                    q.push(i);
                    inQueue[i] = true;
                }
                i++;
            }

            if (v[idx].remaining > 0) {
                q.push(idx);
            } else {
                completed++;
                v[idx].completion = time;
                v[idx].turnaround = v[idx].completion - v[idx].arrival;
                v[idx].waiting = v[idx].turnaround - v[idx].burst;
            }
        }
    }

    cout << endl << CYAN << "================ Round Robin (Quantum=" << quantum << ") ================\n" << RESET;
    ganttchart(gantt);
    printtable(v);
}

// ---------------- Main ----------------

int main() {
    cout << CYAN << "=============================================\n";
    cout << "         CPU Scheduling Simulator (C++)      \n";
    cout << "=============================================" << RESET << "\n";

    int n;
    cout << "Enter the number of processes: ";
    cin >> n;
    vector<Process> original(n);

    for (int i = 0; i < n; i++) {
        original[i].pid = i + 1;
        cout << "--- Process P" << i + 1 << " ---\n";
        cout << "Arrival Time: "; cin >> original[i].arrival;
        cout << "Burst Time:   "; cin >> original[i].burst;
        cout << "Priority (Lower # = Higher Priority): "; cin >> original[i].priority;
    }

    while (true) {
        cout << CYAN << "\nChoose Scheduling Algorithm (Enter 8 to Exit):\n";
        cout << "1. First Come First Serve (FCFS)\n";
        cout << "2. Shortest Job First (Non-preemptive)\n";
        cout << "3. Shortest Job First (Preemptive) / SRTF\n";
        cout << "4. Priority Scheduling (Non-preemptive)\n";
        cout << "5. Priority Scheduling (Preemptive)\n";
        cout << "6. Round Robin\n";
        cout << "7. Exit\n"; // Removed duplicate SRTF entry, consolidated in option 3
        cout << "Enter your choice: " << RESET;

        int choice;
        cin >> choice;

        if (choice == 7) {
            cout << "Exiting...\n";
            break;
        }

        vector<Process> v = original; // Refresh process data

        switch (choice) {
            case 1: FCFS(v); break;
            case 2: SJF_NonPreemptive(v); break;
            case 3: SJF_Preemptive(v); break;
            case 4: Priority_NonPreemptive(v); break;
            case 5: Priority_Preemptive(v); break;
            case 6: {
                int q;
                cout << "Enter Time Quantum: ";
                cin >> q;
                RoundRobin(v, q);
                break;
            }
            default: cout << "Invalid Choice\n";
        }
    }

    return 0;
}