#include <iostream> // Standard Input-Output library for console cin and cout operations
#include <vector>   // Vector library to use dynamic arrays for frames and page storage
#include <queue>    // Queue library, typically used for FIFO (First-In-First-Out) logic
#include <climits>  // Contains constants like INT_MAX, used for finding the oldest page in LRU
using namespace std; // Allows us to use names from the standard namespace without the 'std::' prefix

int frameSize; // Global integer to store the fixed number of memory slots (frames) available
vector<int> frames; // Global vector (dynamic array) representing the current pages held in memory

// Function to check if a specific page is already present in any of the memory frames
bool isInFrames(int page) {
    // Loop through each slot in the frames vector
    for (int i = 0; i < frames.size(); i++) {
        // If the current slot contains the page we are looking for
        if (frames[i] == page) return true; // Return true indicating a 'Hit' (page found)
    }
    return false; // Return false indicating a 'Fault' (page not found in memory)
}

// Global pointer for FIFO algorithm to track which frame index is next for replacement
int fifoPointer = 0;

// Function to handle page replacement using the First-In-First-Out (FIFO) logic
int fifoReplace(int page) {
    int replaced = frames[fifoPointer]; // Store the value of the page currently being replaced
    frames[fifoPointer] = page; // Overwrite the oldest page in the queue with the new page
    // Increment pointer and use modulo to wrap around back to 0 when it exceeds frameSize
    fifoPointer = (fifoPointer + 1) % frameSize; 
    return replaced; // Return the value of the page that was removed
}

// Function to handle page replacement using the Least Recently Used (LRU) logic
int lruReplace(int page, vector<int>& pageHistory, int currentStep) {
    int lruIndex = 0; // Index of the frame that contains the least recently used page
    int oldestUse = INT_MAX; // Initialize with a very large number to find the minimum index

    // Iterate through all frames to find which one was used furthest back in time
    for (int i = 0; i < frames.size(); i++) {
        int lastUsed = -1; // Default value if the page hasn't been used yet (shouldn't happen here)
        // Search backwards through the page history starting from the step before current
        for (int j = currentStep - 1; j >= 0; j--) {
            // If we find the page that matches the one in the current frame slot
            if (pageHistory[j] == frames[i]) {
                lastUsed = j; // Record the index of its last occurrence
                break; // Stop searching once the most recent usage is found
            }
        }
        // If this page's last usage was earlier than the 'oldestUse' found so far
        if (lastUsed < oldestUse) {
            oldestUse = lastUsed; // Update the minimum time/index
            lruIndex = i; // Record this frame as the current candidate for replacement
        }
    }

    int replaced = frames[lruIndex]; // Store the value of the page being evicted
    frames[lruIndex] = page; // Replace the LRU page with the current incoming page
    return replaced; // Return the value of the page that was removed
}

// Helper function to display the current state of all memory frames in the console
void printFrames() {
    cout << "[ "; // Opening bracket for visual grouping
    for (int i = 0; i < frames.size(); i++) {
        // If the frame is empty (initialized to -1), print a dash
        if (frames[i] == -1) cout << "- ";
        // Otherwise, print the page number stored in that frame
        else cout << frames[i] << " ";
    }
    cout << "]"; // Closing bracket for visual grouping
}

// The entry point of the simulation program
int main() {
    // Print the title of the simulation for the user
    cout << "=== Adaptive Page Replacement Simulation ===" << endl;
    // Ask the user for the number of frames (memory capacity)
    cout << "Enter number of frames: ";
    cin >> frameSize; // Read the user input for frame size

    // Initialize the frames vector with the given size and fill with -1 (representing empty)
    frames.assign(frameSize, -1);
    fifoPointer = 0; // Reset the FIFO tracker for a fresh start

    int n; // Variable to store the total number of page requests
    cout << "Enter number of pages in reference string: ";
    cin >> n; // Read the length of the reference sequence

    vector<int> pages(n); // Vector to hold the sequence of incoming page requests
    cout << "Enter page reference string: ";
    // Loop to read each page number in the user-provided reference string
    for (int i = 0; i < n; i++) cin >> pages[i];

    int totalHits = 0; // Counter for successful page lookups (Hits)
    int totalFaults = 0; // Counter for unsuccessful page lookups (Faults)
    int faultsInWindow = 0; // Counter for faults within the current monitoring window
    int hitsInWindow = 0; // Counter for hits within the current monitoring window
    int windowSize = 5; // The number of steps to monitor before considering an algorithm switch
    string currentAlgo = "FIFO"; // Initial algorithm set to First-In-First-Out

    // Print table headers for the step-by-step simulation output
    cout << "\nStep | Page | Result | Algorithm | Frames" << endl;
    cout << "---------------------------------------------" << endl;

    // Iterate through each page in the reference sequence
    for (int i = 0; i < n; i++) {
        int page = pages[i]; // Get the current page being requested

        // Adaptive Logic: Every 'windowSize' steps, check the performance
        if (i > 0 && i % windowSize == 0) {
            // Calculate the ratio of faults to the total window size
            float faultRate = (float)faultsInWindow / windowSize;
            // If the fault rate is high (> 60%) and we are using FIFO, switch to LRU
            if (faultRate > 0.6 && currentAlgo == "FIFO") {
                currentAlgo = "LRU";
            } 
            // If the fault rate is low (<= 40%) and we are using LRU, switch back to FIFO
            else if (faultRate <= 0.4 && currentAlgo == "LRU") {
                currentAlgo = "FIFO";
            }
            faultsInWindow = 0; // Reset window fault counter for the next batch
            hitsInWindow = 0; // Reset window hit counter for the next batch
        }

        string result; // String to hold whether this step was a "HIT" or a "FAULT"
        // Check if the current page is already in memory
        if (isInFrames(page)) {
            result = "HIT "; // Mark as a hit
            totalHits++; // Increment global hit counter
            hitsInWindow++; // Increment window hit counter
        } else {
            result = "FAULT"; // Mark as a fault (page must be loaded/replaced)
            totalFaults++; // Increment global fault counter
            faultsInWindow++; // Increment window fault counter

            // First, try to find an empty slot (-1) before replacing an existing page
            bool placed = false;
            for (int j = 0; j < frameSize; j++) {
                if (frames[j] == -1) {
                    frames[j] = page; // Place the page in the empty slot
                    placed = true; // Mark that it was successfully placed
                    break; // Exit the loop as we found a spot
                }
            }

            // If no empty slot was found, perform replacement based on the current algorithm
            if (!placed) {
                // If the adaptive logic has selected FIFO
                if (currentAlgo == "FIFO") {
                    fifoReplace(page); // Replace using FIFO logic
                } else {
                    // Otherwise, replace using LRU logic (pass history for calculation)
                    lruReplace(page, pages, i);
                }
            }
        }

        // Print the detailed info for the current step of the simulation
        cout << "  " << i+1 << "  |  " << page << "   |  " << result << "  |  " << currentAlgo << "  |  ";
        printFrames(); // Display the contents of all frames after the current step
        cout << endl; // Move to the next line for the next step
    }

    // After all pages are processed, print the final summary of the simulation
    cout << "\n=== Summary ===" << endl;
    cout << "Total Hits:   " << totalHits << endl; // Show total number of hits
    cout << "Total Faults: " << totalFaults << endl; // Show total number of faults
    // Calculate and display the Hit Rate as a percentage
    cout << "Hit Rate:     " << (float)totalHits / n * 100 << "%" << endl;

    return 0; // Indicate successful program termination
}
