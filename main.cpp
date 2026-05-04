#include <iostream>
#include <vector>
#include <queue>
#include <climits>
using namespace std;

int frameSize;
vector<int> frames;

bool isInFrames(int page) {
    for (int i = 0; i < frames.size(); i++) {
        if (frames[i] == page) return true;
    }
    return false;
}

// FIFO uses a queue to track insertion order
int fifoPointer = 0;

int fifoReplace(int page) {
    int replaced = frames[fifoPointer];
    frames[fifoPointer] = page;
    fifoPointer = (fifoPointer + 1) % frameSize;
    return replaced;
}

// LRU finds the page that was used least recently
int lruReplace(int page, vector<int>& pageHistory, int currentStep) {
    int lruIndex = 0;
    int oldestUse = INT_MAX;

    for (int i = 0; i < frames.size(); i++) {
        int lastUsed = -1;
        for (int j = currentStep - 1; j >= 0; j--) {
            if (pageHistory[j] == frames[i]) {
                lastUsed = j;
                break;
            }
        }
        if (lastUsed < oldestUse) {
            oldestUse = lastUsed;
            lruIndex = i;
        }
    }

    int replaced = frames[lruIndex];
    frames[lruIndex] = page;
    return replaced;
}

void printFrames() {
    cout << "[ ";
    for (int i = 0; i < frames.size(); i++) {
        if (frames[i] == -1) cout << "- ";
        else cout << frames[i] << " ";
    }
    cout << "]";
}

int main() {
    cout << "=== Adaptive Page Replacement Simulation ===" << endl;
    cout << "Enter number of frames: ";
    cin >> frameSize;

    frames.assign(frameSize, -1);
    fifoPointer = 0;

    int n;
    cout << "Enter number of pages in reference string: ";
    cin >> n;

    vector<int> pages(n);
    cout << "Enter page reference string: ";
    for (int i = 0; i < n; i++) cin >> pages[i];

    int totalHits = 0;
    int totalFaults = 0;
    int faultsInWindow = 0;
    int hitsInWindow = 0;
    int windowSize = 5;
    string currentAlgo = "FIFO"; // start with FIFO

    cout << "\nStep | Page | Result | Algorithm | Frames" << endl;
    cout << "---------------------------------------------" << endl;

    for (int i = 0; i < n; i++) {
        int page = pages[i];

        // Switch logic: if fault rate is high in recent window, switch to LRU
        if (i > 0 && i % windowSize == 0) {
            float faultRate = (float)faultsInWindow / windowSize;
            if (faultRate > 0.6 && currentAlgo == "FIFO") {
                currentAlgo = "LRU";
            } else if (faultRate <= 0.4 && currentAlgo == "LRU") {
                currentAlgo = "FIFO";
            }
            faultsInWindow = 0;
            hitsInWindow = 0;
        }

        string result;
        if (isInFrames(page)) {
            result = "HIT ";
            totalHits++;
            hitsInWindow++;
        } else {
            result = "FAULT";
            totalFaults++;
            faultsInWindow++;

            // Find an empty frame first
            bool placed = false;
            for (int j = 0; j < frameSize; j++) {
                if (frames[j] == -1) {
                    frames[j] = page;
                    placed = true;
                    break;
                }
            }

            if (!placed) {
                if (currentAlgo == "FIFO") {
                    fifoReplace(page);
                } else {
                    lruReplace(page, pages, i);
                }
            }
        }

        cout << "  " << i+1 << "  |  " << page << "   |  " << result << "  |  " << currentAlgo << "  |  ";
        printFrames();
        cout << endl;
    }

    cout << "\n=== Summary ===" << endl;
    cout << "Total Hits:   " << totalHits << endl;
    cout << "Total Faults: " << totalFaults << endl;
    cout << "Hit Rate:     " << (float)totalHits / n * 100 << "%" << endl;

    return 0;
}
