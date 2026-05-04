// Global state
let frames = [];
let frameSize = 0;
let fifoPointer = 0;

function isInFrames(page) {
    return frames.includes(page);
}

function fifoReplace(page) {
    frames[fifoPointer] = page;
    fifoPointer = (fifoPointer + 1) % frameSize;
}

function lruReplace(page, pageHistory, currentStep) {
    let lruIndex = 0;
    let oldestUse = Infinity;

    for (let i = 0; i < frames.length; i++) {
        let lastUsed = -1;
        for (let j = currentStep - 1; j >= 0; j--) {
            if (pageHistory[j] === frames[i]) {
                lastUsed = j;
                break;
            }
        }
        if (lastUsed < oldestUse) {
            oldestUse = lastUsed;
            lruIndex = i;
        }
    }

    frames[lruIndex] = page;
}

function runSimulation() {
    const frameInput = document.getElementById("frameCount").value.trim();
    const pageInput = document.getElementById("pageString").value.trim();

    if (!frameInput || !pageInput) {
        alert("Please fill in both fields.");
        return;
    }

    frameSize = parseInt(frameInput);
    const pages = pageInput.split(/[\s,]+/).map(Number);

    if (isNaN(frameSize) || frameSize <= 0) {
        alert("Enter a valid number of frames.");
        return;
    }

    if (pages.some(isNaN)) {
        alert("Page string should only contain numbers.");
        return;
    }

    // Reset state
    frames = new Array(frameSize).fill(-1);
    fifoPointer = 0;

    let totalHits = 0;
    let totalFaults = 0;
    let faultsInWindow = 0;
    let currentAlgo = "FIFO";
    const windowSize = 5;

    const tableBody = document.getElementById("resultsBody");
    tableBody.innerHTML = "";

    for (let i = 0; i < pages.length; i++) {
        const page = pages[i];

        // Check window and maybe switch algorithm
        if (i > 0 && i % windowSize === 0) {
            const faultRate = faultsInWindow / windowSize;
            if (faultRate > 0.6 && currentAlgo === "FIFO") {
                currentAlgo = "LRU";
            } else if (faultRate <= 0.4 && currentAlgo === "LRU") {
                currentAlgo = "FIFO";
            }
            faultsInWindow = 0;
        }

        let result;
        if (isInFrames(page)) {
            result = "HIT";
            totalHits++;
        } else {
            result = "FAULT";
            totalFaults++;
            faultsInWindow++;

            // Try to find empty slot
            let placed = false;
            for (let j = 0; j < frameSize; j++) {
                if (frames[j] === -1) {
                    frames[j] = page;
                    placed = true;
                    break;
                }
            }

            if (!placed) {
                if (currentAlgo === "FIFO") {
                    fifoReplace(page);
                } else {
                    lruReplace(page, pages, i);
                }
            }
        }

        const frameSnapshot = frames.map(f => f === -1 ? "-" : f).join("  ");

        const row = document.createElement("tr");
        row.innerHTML = `
            <td>${i + 1}</td>
            <td>${page}</td>
            <td class="${result === 'HIT' ? 'hit' : 'fault'}">${result}</td>
            <td class="algo-tag">${currentAlgo}</td>
            <td class="frame-cell">${frameSnapshot}</td>
        `;
        tableBody.appendChild(row);
    }

    // Show summary
    document.getElementById("totalHits").textContent = totalHits;
    document.getElementById("totalFaults").textContent = totalFaults;
    const hitRate = ((totalHits / pages.length) * 100).toFixed(1);
    document.getElementById("hitRate").textContent = hitRate + "%";

    document.getElementById("summary").style.display = "flex";
    document.getElementById("resultsSection").style.display = "block";
}
