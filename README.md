# 🖥️ CPU Scheduling Simulator (C++)

A comprehensive **CPU Scheduling Simulator** written in **C++** that demonstrates and compares classical CPU scheduling algorithms used in Operating Systems.  
The program provides **Gantt charts**, **process execution tables**, and **average waiting & turnaround times** for clear visualization and analysis.

---

## ✨ Features

- Menu-driven interactive simulator
- Colored terminal output for better readability
- Accurate handling of CPU idle time
- Detailed process execution table
- Automatic calculation of:
  - Completion Time (CT)
  - Turnaround Time (TAT)
  - Waiting Time (WT)
- Gantt Chart visualization for each algorithm

---

## 🧠 Scheduling Algorithms Implemented

1. **First Come First Serve (FCFS)**
2. **Shortest Job First (Non-Preemptive)**
3. **Shortest Job First (Preemptive) / SRTF**
4. **Priority Scheduling (Non-Preemptive)**
5. **Priority Scheduling (Preemptive)**
6. **Round Robin Scheduling** (User-defined Time Quantum)

> Note: Lower priority number indicates **higher priority**.

---

## 🛠️ Technologies Used

- **Language:** C++
- **Core Concepts:**
  - CPU Scheduling
  - Priority Queue
  - Queue
  - Sorting Algorithms
  - Preemptive & Non-Preemptive Scheduling
- **Libraries:**
  - `<iostream>`
  - `<vector>`
  - `<queue>`
  - `<algorithm>`
  - `<tuple>`
  - `<iomanip>`

---

## ▶️ How to Compile and Run

### Compile
```bash
g++ cpu_scheduling.cpp -o cpu_scheduling
```

---

## 🧪 Sample Input

```text
Enter the number of processes: 3

--- Process P1 ---
Arrival Time: 0
Burst Time: 5
Priority: 2

--- Process P2 ---
Arrival Time: 1
Burst Time: 3
Priority: 1

--- Process P3 ---
Arrival Time: 2
Burst Time: 4
Priority: 3
```
---

## 📊 Output Description

### 🔹 Gantt Chart
Displays:
- Execution order of processes
- CPU idle intervals
- Start and completion times

### 🔹 Process Execution Table
Includes:
- Process ID (PID)
- Arrival Time
- Burst Time
- Priority
- Completion Time
- Turnaround Time
- Waiting Time

---

## 📁 Project Structure

```text
├── cpu_scheduling.cpp
├── README.md
```

---

## 🎯 Learning Outcomes

- Strong understanding of classical CPU scheduling algorithms  
- Clear comparison between different scheduling strategies  
- Visualization of context switching and CPU idle time  
- Practical implementation of Operating Systems scheduling concepts

---

## 🚀 Future Enhancements

- Multilevel Queue Scheduling  
- Multilevel Feedback Queue Scheduling  
- Starvation prevention using Aging  
- Graphical User Interface (GUI)  
- Exportable Gantt chart output

---

## 👨‍💻 Author

**Ravinder Singh**  
Mathematics & Computing Technology
