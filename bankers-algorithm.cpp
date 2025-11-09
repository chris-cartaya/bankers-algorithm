/**
 * Name: Chris Cartaya
 * Date: November 4, 2025
 * Course: COP 4610 Operating Systems
 * Assignment 4, Question 2: Banker's Algorithm Implementation
 * Filename: bankers-algorithm.cpp
 *
 * Description:
 * Implementation of the Banker's Algorithm in C++. This program checks if the system is in a safe state
 *
 * Compilation:
 *  g++ -Wall -Wextra -O2 bankers-algorithm.cpp -o bankers.exe
 *
 * Execution:
 *  ./bankers
 */

#include <iostream>
using namespace std;

// Compile-time sizes (good for array bounds and optimization)
constexpr int NUM_PROCESSES = 5; // rows
constexpr int NUM_RESOURCES = 4; // cols

// ----- Utilities -----

// Prints a NUM_PROCESSES x NUM_RESOURCES matrix.
void printMatrix(const int (&matrix)[NUM_PROCESSES][NUM_RESOURCES])
{
    for (int p = 0; p < NUM_PROCESSES; ++p)
    {
        for (int r = 0; r < NUM_RESOURCES; ++r)
        {
            cout << matrix[p][r] << ' ';
        }
        cout << '\n';
    }
}

// Prints a 1D resource vector (length NUM_RESOURCES).
void printVector(const int (&v)[NUM_RESOURCES])
{
    cout << '{';
    for (int r = 0; r < NUM_RESOURCES; ++r)
    {
        cout << v[r] << (r + 1 < NUM_RESOURCES ? ',' : '}');
    }
}

// Compute need = maxMatrix - allocation
void findNeedMatrix(const int (&maxMatrix)[NUM_PROCESSES][NUM_RESOURCES],
                    const int (&allocation)[NUM_PROCESSES][NUM_RESOURCES],
                    int (&need)[NUM_PROCESSES][NUM_RESOURCES])
{
    for (int p = 0; p < NUM_PROCESSES; ++p)
    {
        for (int r = 0; r < NUM_RESOURCES; ++r)
        {
            need[p][r] = maxMatrix[p][r] - allocation[p][r];
        }
    }
}

// Returns true if a single process's needRow can be satisfied by work now.
// In other words, returns true if need[i][*] <= work[*] for all resources
bool canProcessRun(const int (&needRow)[NUM_RESOURCES],
                   const int (&work)[NUM_RESOURCES])
{
    for (int r = 0; r < NUM_RESOURCES; ++r)
    {
        if (needRow[r] > work[r])
            return false;
    }
    return true;
}

// Banker’s safety check.
// - Inputs: allocation, need, available
// - Output: safeSeq (order of processes that can finish)
// - Return: true if safe, false if unsafe
bool isSafeState(const int (&allocation)[NUM_PROCESSES][NUM_RESOURCES],
                 const int (&need)[NUM_PROCESSES][NUM_RESOURCES],
                 const int (&available)[NUM_RESOURCES],
                 int (&safeSeq)[NUM_PROCESSES])
{
    bool finish[NUM_PROCESSES] = {false}; // which processes have "completed"
    int work[NUM_RESOURCES];              // current available

    // Copy available resources into work[] for simulation.
    // work[] changes during the algorithm; available[] remains unchanged.
    for (int r = 0; r < NUM_RESOURCES; ++r)
    {
        work[r] = available[r];
    }

    // Number of finished processes placed in safeSeq
    int finishedProcessCount = 0;

    // Try to find a process that can finish, up to NUM_PROCESSES times/passes
    for (int pass = 0; pass < NUM_PROCESSES; ++pass)
    {
        bool hasProgressed = false;

        for (int p = 0; p < NUM_PROCESSES; ++p)
        {
            if (!finish[p] && canProcessRun(need[p], work))
            {
                // "Run" process p. It finishes and releases its allocation.
                for (int r = 0; r < NUM_RESOURCES; ++r)
                {
                    work[r] += allocation[p][r];
                }
                finish[p] = true;
                safeSeq[finishedProcessCount++] = p;
                hasProgressed = true;
            }
        }

        // No one could proceed this pass. Stop early.
        if (!hasProgressed)
            break;
    }

    // If any process is unfinished, it is unsafe.
    for (int p = 0; p < NUM_PROCESSES; ++p)
    {
        if (!finish[p])
            return false;
    }

    return true;
}

// Runs a single test with a given Available vector and prints the outcome.
void testAvailableVector(const int (&available)[NUM_RESOURCES],
                         const int (&allocation)[NUM_PROCESSES][NUM_RESOURCES],
                         const int (&need)[NUM_PROCESSES][NUM_RESOURCES])
{
    int safeSeq[NUM_PROCESSES];

    cout << "Testing Available = ";
    printVector(available);
    cout << " ... ";

    if (isSafeState(allocation, need, available, safeSeq))
    {
        cout << "SAFE. Safe sequence: ";
        for (int i = 0; i < NUM_PROCESSES; ++i)
        {
            cout << 'P' << safeSeq[i] << (i + 1 < NUM_PROCESSES ? " -> " : "\n");
        }
    }
    else
    {
        cout << "UNSAFE\n";
    }
}

int main()
{
    const int allocation[NUM_PROCESSES][NUM_RESOURCES] = {
        {1, 2, 0, 2},
        {0, 1, 1, 2},
        {1, 2, 4, 0},
        {1, 2, 0, 1},
        {1, 0, 0, 1}};

    const int maxMatrix[NUM_PROCESSES][NUM_RESOURCES] = {
        {4, 3, 1, 6},
        {2, 4, 2, 4},
        {3, 6, 5, 1},
        {2, 6, 2, 3},
        {3, 1, 1, 2}};

    cout << "allocation:\n";
    printMatrix(allocation);
    cout << '\n';
    cout << "max:\n";
    printMatrix(maxMatrix);
    cout << '\n';

    int need[NUM_PROCESSES][NUM_RESOURCES];
    findNeedMatrix(maxMatrix, allocation, need);
    cout << "need:\n";
    printMatrix(need);
    cout << '\n';

    int available1[NUM_RESOURCES] = {2, 2, 2, 3};
    int available2[NUM_RESOURCES] = {4, 4, 1, 1};
    int available3[NUM_RESOURCES] = {3, 0, 1, 4};
    int available4[NUM_RESOURCES] = {1, 5, 2, 2};

    testAvailableVector(available1, allocation, need);
    testAvailableVector(available2, allocation, need);
    testAvailableVector(available3, allocation, need);
    testAvailableVector(available4, allocation, need);

    return 0;
}
