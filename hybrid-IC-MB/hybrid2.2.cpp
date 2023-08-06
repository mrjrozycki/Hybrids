#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>

//-------------------IC-BFD -> MB-FFD----------------

class Item
{
public:
    Item(int size) : size_(size) {}
    int getSize() const { return size_; }

private:
    int size_;
};

class Bin
{
public:
    Bin(int capacity) : capacity_(capacity), remaining_capacity_(capacity) {}
    bool canFit(const Item& item) const { return remaining_capacity_ >= item.getSize(); }
    bool addItem(const Item& item) {
        if (!canFit(item))
        {
            return false;
        }
        remaining_capacity_ -= item.getSize();
        items.push_back(item);
        return true;
    }
    bool isFull() const { return remaining_capacity_ == 0; }
    int getRemainingCapacity() const { return remaining_capacity_; }
    const std::vector<Item>& getItems() const { return items; }

private:
    int capacity_;
    int remaining_capacity_;
    std::vector<Item> items;
};

bool packItemsIC(const std::vector<Item>& items, std::vector<Bin>& bins, int binCapacity)
{
    // Sort items in decreasing order
    std::vector<Item> sortedItems = items;
    std::sort(sortedItems.begin(), sortedItems.end(), [](const Item& a, const Item& b)
        {
            return a.getSize() > b.getSize();
        });

    for (const auto& item : sortedItems)
    {
        // Best Fit logic for IC-BFD: Find the bin with the smallest remaining capacity
        int bestBinIdx = -1;
        int smallestRemainingCapacity = binCapacity + 1; // Initialize to a value greater than the bin capacity

        for (int i = 0; i < bins.size(); ++i)
        {
            if (bins[i].canFit(item) && bins[i].getRemainingCapacity() < smallestRemainingCapacity)
            {
                bestBinIdx = i;
                smallestRemainingCapacity = bins[i].getRemainingCapacity();
            }
        }

        if (bestBinIdx != -1)
        {
            // Item fits in an existing bin with the best fit, so add it to that bin
            bins[bestBinIdx].addItem(item);
        }
        else
        {
            // Item couldn't fit in any existing bin, so create a new bin and add it
            Bin newBin(binCapacity);
            newBin.addItem(item);
            bins.push_back(newBin);
        }
    }

    return true;
}

bool packItemsICWithReducedCapacity(const std::vector<Item>& items, std::vector<Bin>& bins, int binCapacity)
{
    // Threshold-based approach: Decrease bin capacity before running IC-BFD algorithm
    int reducedBinCapacity = binCapacity / 2; // Example: Set reduced capacity to half of the original capacity
    // Now, run IC-BFD with reduced capacity
    return packItemsIC(items, bins, reducedBinCapacity);
}

bool packItemsIntoBins(std::vector<Item>& items, std::vector<Bin>& bins, int binCapacity)
{
    // Sort items in decreasing order
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b)
        {
            return a.getSize() > b.getSize();
        });

    for (const auto& item : items)
    {
        bool itemPacked = false;
        for (auto& bin : bins)
        {
            if (!bin.isFull() && bin.addItem(item))
            {
                itemPacked = true;
                break;
            }
        }
        if (!itemPacked)
        {
            return false;
        }
    }
    return true;
}

class HybridBinPacking
{
public:
    HybridBinPacking(int maxBins, int batchIncrement, int binCapacity)
        : maxBins_(maxBins), batchIncrement_(batchIncrement), binCapacity_(binCapacity) {}

    void addItem(const Item& item);
    bool runHybridAlgorithm();

private:
    int maxBins_;
    int batchIncrement_;
    int binCapacity_;
    std::vector<Item> items;
};

void HybridBinPacking::addItem(const Item& item)
{
    items.push_back(item);
}

int calculateLowerBound(const std::vector<Item>& items, int binCapacity)
{
    int sumItemSizes = 0;
    for (const auto& item : items)
    {
        sumItemSizes += item.getSize();
    }
    return static_cast<int>(std::round(static_cast<double>(sumItemSizes) / binCapacity));
}

bool HybridBinPacking::runHybridAlgorithm()
{
    auto start_time = std::chrono::high_resolution_clock::now(); //start the measur

    int lowerBound = calculateLowerBound(items, binCapacity_);
    int m = std::min(maxBins_, lowerBound);

    // Stage 1: IC-BFD Algorithm with reduced capacity
    std::vector<Bin> binsIC(m, Bin(binCapacity_));
    bool successIC = packItemsICWithReducedCapacity(items, binsIC, binCapacity_);

    if (successIC)
    {
        // IC-BFD succeeded with reduced capacity
        // Now, restore bin capacities back to normal before proceeding to MB-FFD
        for (auto& bin : binsIC)
        {
            bin = Bin(binCapacity_); // Restore capacity to normal for each bin
        }

        // Proceed to MB-FFD with restored bin capacities
        bool successMB = packItemsIntoBins(items, binsIC, binCapacity_);

        if (successMB)
        {
            // MB-FFD succeeded
            std::cout << "Successfully packed items into bins using the MB-FFD algorithm!" << std::endl;
            for (int i = 0; i < binsIC.size(); i++)
            {
                std::cout << "Bin " << i + 1 << ": ";
                for (const auto& item : binsIC[i].getItems())
                {
                    std::cout << "Item (Size: " << item.getSize() << ") ";
                }
                std::cout << std::endl;
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            std::cout << "Execution time for MB-FFD: " << duration.count() << " microseconds" << std::endl;
            return true;
        }
    }
    return false;
}

int main()
{
    int maxBins = 3;
    int batchIncrement = 1;
    int binCapacity = 100;

    HybridBinPacking hybridBinPacking(maxBins, batchIncrement, binCapacity);

    hybridBinPacking.addItem(Item(10));
    hybridBinPacking.addItem(Item(21));
    hybridBinPacking.addItem(Item(44));

    bool success = hybridBinPacking.runHybridAlgorithm();

    if (!success) {
        std::cout << "Both IC-BFD and MB-FFD algorithms have failed. No feasible solution found." << std::endl;
    }

    return 0;
}
