#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define task structure
typedef struct
{
    char name[10];
    int priority;
    int cpuBurst;
} Task;

// Function prototypes
void loadTasks(Task tasks[], int *n);
void fcfs(Task tasks[], int n);
void sjf(Task tasks[], int n);
void priorityScheduling(Task tasks[], int n);
void roundRobin(Task tasks[], int n, int timeQuantum);
int compareBurst(const void *a, const void *b);
int comparePriority(const void *a, const void *b);

int main()
{
    Task tasks[100];
    int n;

    // Load tasks from file
    loadTasks(tasks, &n);

    // Call each scheduling algorithm
    printf("Task List:\n");
    for (int i = 0; i < n; i++)
    {
        printf("%s (Priority: %d, CPU Burst: %d)\n", tasks[i].name, tasks[i].priority, tasks[i].cpuBurst);
    }
    printf("\n");

    fcfs(tasks, n);
    sjf(tasks, n);
    priorityScheduling(tasks, n);
    roundRobin(tasks, n, 5); // Example time quantum

    return 0;
}

// Function to load tasks from schedule.txt
void loadTasks(Task tasks[], int *n)
{
    FILE *file = fopen("schedule.txt", "r");
    if (file == NULL)
    {
        printf("File does not exist");
        exit(1);
    }
    *n = 0;
    while (fscanf(file, "%[^,], %d, %d\n", tasks[*n].name, &tasks[*n].priority, &tasks[*n].cpuBurst) != EOF)
    {
        (*n)++;
    }
    fclose(file);
}

// First-Come-First-Served Scheduling
void fcfs(Task tasks[], int n)
{
    printf("FCFS Scheduling:\n");
    int waitTime = 0;
    int totalWait = 0;
    for (int i = 0; i < n; i++)
    {
        printf("Task %s (Priority: %d, CPU Burst: %d)\n", tasks[i].name, tasks[i].priority, tasks[i].cpuBurst);
        printf("Wait Time: %d\n", waitTime);
        totalWait += waitTime;
        waitTime += tasks[i].cpuBurst;
    }
    int average_wait_time = 0;
    average_wait_time = totalWait / n;
    printf("Average Wait Time: %.2d\n\n", average_wait_time);
}

// Shortest Job First Scheduling
void sjf(Task tasks[], int n)
{
    // Sort tasks by burst time (ascending order)
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (tasks[j].cpuBurst > tasks[j + 1].cpuBurst)
            {
                Task temp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = temp;
            }
        }
    }
    printf("SJF Scheduling:\n");
    int waitTime = 0;
    int totalWait = 0;
    for (int i = 0; i < n; i++)
    {
        printf("Task %s (Priority: %d, CPU Burst: %d)\n", tasks[i].name, tasks[i].priority, tasks[i].cpuBurst);
        printf("Wait Time: %d\n", waitTime);
        totalWait += waitTime;
        waitTime += tasks[i].cpuBurst;
    }
    int average_wait_time = 0;
    average_wait_time = totalWait / n;
    printf("Average Wait Time: %.2d\n\n", average_wait_time);
}

// Priority Scheduling

void priorityScheduling(Task tasks[], int n)
{

    printf("Priority Scheduling:\n");


for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {

        if (tasks[j].priority < tasks[j + 1].priority || 
           (tasks[j].priority == tasks[j + 1].priority && strcmp(tasks[j].name, tasks[j + 1].name) > 0)) {
            Task temp = tasks[j];
            tasks[j] = tasks[j + 1];
            tasks[j + 1] = temp;
        }
    }
}



    int waitTime = 0;
    int totalWait = 0;
    for (int i = 0; i < n; i++)
    {
        printf("Task %s (Priority: %d, CPU Burst: %d)\n", tasks[i].name, tasks[i].priority, tasks[i].cpuBurst);
        printf("Wait Time: %d\n", waitTime);
        totalWait += waitTime;
        waitTime += tasks[i].cpuBurst;
    }
    int average_wait_time = 0;
    average_wait_time = totalWait / n;
    printf("Average Wait Time: %.2d\n\n", average_wait_time);
}

// Round Robin Scheduling
void roundRobin(Task tasks[], int n, int timeQuantum)
{
    printf("Round Robin Scheduling (Time Quantum: %d):\n", timeQuantum);
    int remaining[n];
    for (int i = 0; i < n; i++)
        remaining[i] = tasks[i].cpuBurst;

    int time = 0;
    int done = 0;
    while (done < n)
    {
        for (int i = 0; i < n; i++)
        {
            if (remaining[i] > 0)
            {
                if (remaining[i] <= timeQuantum)
                {
                    time += remaining[i];
                    printf("Task %s completed at time %d\n", tasks[i].name, time);
                    remaining[i] = 0;
                    done++;
                }
                else
                {
                    time += timeQuantum;
                    remaining[i] -= timeQuantum;
                    printf("Task %s processed until time %d\n", tasks[i].name, time);
                }
            }
        }
    }
    printf("\n");
}
