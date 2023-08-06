#include<iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>

// ---------------MB-BFD -> MB-FFD------------------

class Item
{
public:
    Item(int id, int size) : id_(id), size_(size) {}
    int getId() const { return id_; }
    int getSize() const { return size_; }

private:
    int id_;
    int size_;
};

class Bin
{
public:
    Bin(int capacity) : capacity_(capacity), remaining_capacity_(capacity) {}
    bool canFit(const Item& item) const { return remaining_capacity_ >= item.getSize(); }
    bool addItem(const Item& item)
    {
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

class MBBinPacking
{
public:
    MBBinPacking(int binCapacity) : binCapacity_(binCapacity) {}
    void addItem(const Item& item);
    int getNumBins() const { return bins_.size(); }
    const std::vector<Bin>& getBins() const { return bins_; }

private:
    int binCapacity_;
    std::vector<Bin> bins_;
};

void MBBinPacking::addItem(const Item& item)
{
    auto it = std::min_element(bins_.begin(), bins_.end(), [&](const Bin& bin1, const Bin& bin2) {
        return bin1.getRemainingCapacity() < bin2.getRemainingCapacity() && bin1.canFit(item);
        });

    if (it != bins_.end()) {
        it->addItem(item);
    }
    else {
        Bin new_bin(binCapacity_);
        new_bin.addItem(item);
        bins_.push_back(new_bin);
    }
}

bool packItemsIntoBins(std::vector<Item>& items, std::vector<Bin>& bins, int binCapacity)
{
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.getSize() > b.getSize(); // Sort items in descending order
        });

    for (const auto& item : items)
    {
        bool itemPacked = false;
        int bestFitBinIdx = -1;
        int minRemainingSpace = binCapacity + 1;

        for (int i = 0; i < bins.size(); ++i)
        {
            int remainingSpace = bins[i].getRemainingCapacity();
            if (remainingSpace >= item.getSize() && remainingSpace < minRemainingSpace)
            {
                bestFitBinIdx = i;
                minRemainingSpace = remainingSpace;
            }
        }

        if (bestFitBinIdx != -1)
        {
            bins[bestFitBinIdx].addItem(item);
            itemPacked = true;
        }

        if (!itemPacked)
        {
            return false;
        }
    }
    return true;
}

class Multibin
{
public:
    Multibin(int maxBins, int incrementStrategy) : maxBins_(maxBins), incrementStrategy_(incrementStrategy) {}

    void addItem(const Item& item)
    {
        items_.push_back(item);
    }

    bool packItems()
    {
        int n = 1;
        while (n <= maxBins_)
        {
            std::vector<Bin> activeBins(n, Bin(binCapacity_));
            bool success = packItemsIntoBins(items_, activeBins, binCapacity_);
            if (success)
            {
                bins_ = activeBins;
                return true;
            }

            if (incrementStrategy_ == 1)
            {
                n++;
            }
            else
            {
                n += std::min(incrementStrategy_, maxBins_ - n + 1);
            }
        }

        return false;
    }

    void printBins() const
    {
        for (int i = 0; i < bins_.size(); i++)
        {
            std::cout << "Bin " << i + 1 << ": ";
            for (const auto& item : bins_[i].getItems())
            {
                std::cout << "Item " << item.getId() << " (Size: " << item.getSize() << ") ";
            }
            std::cout << std::endl;
        }
    }

private:
    int maxBins_;
    int incrementStrategy_;
    int binCapacity_ = 100;
    std::vector<Item> items_;
    std::vector<Bin> bins_;
};

class HybridMultibin
{
public:
    HybridMultibin(int maxBins, int batchIncrement, int binCapacity)
        : maxBins_(maxBins), batchIncrement_(batchIncrement), binCapacity_(binCapacity) {}

    void addItem(const Item& item);
    bool runHybridAlgorithm();

private:
    int maxBins_;
    int batchIncrement_;
    int binCapacity_;
    std::vector<Item> items_;
};

void HybridMultibin::addItem(const Item& item)
{
    items_.push_back(item);
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

bool HybridMultibin::runHybridAlgorithm()
{
    auto start_time = std::chrono::high_resolution_clock::now(); // start the measure

    // Calculate the lower bound
    int lowerBound = calculateLowerBound(items_, binCapacity_);

    // Determine the threshold based on the number of stacked items
    int threshold = std::max(lowerBound, static_cast<int>(items_.size() * 0.2)); // Adjust the threshold as needed

    // Stage 1: MB-BFD Algorithm
    Multibin multibinBFD(maxBins_, batchIncrement_);
    int stackedItems = 0;

    for (const auto& item : items_)
    {
        multibinBFD.addItem(item);
        stackedItems++;

        if (stackedItems >= threshold)
        {
            multibinBFD.packItems();
            std::cout << "Successfully packed " << stackedItems << " items using MB-BFD algorithm!" << std::endl;
            multibinBFD.printBins();
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            std::cout << "Execution time for MB-BFD: " << duration.count() << " microseconds" << std::endl;

            // Stage 2: MB-FFD Algorithm for the remaining items
            Multibin multibinFFD(binCapacity_,batchIncrement_);
            for (int i = stackedItems; i < items_.size(); i++)
            {
                multibinFFD.addItem(items_[i]);
            }

            if (multibinFFD.packItems())
            {
                std::cout << "Successfully packed remaining items into bins using the MB-FFD algorithm!" << std::endl;
                multibinFFD.printBins();
                auto end_time_ffd = std::chrono::high_resolution_clock::now();
                auto duration_ffd = std::chrono::duration_cast<std::chrono::microseconds>(end_time_ffd - end_time);
                std::cout << "Execution time for MB-FFD: " << duration_ffd.count() << " microseconds" << std::endl;
                return true;
            }
            else
            {
                std::cout << "MB-FFD algorithm failed to find a solution for remaining items." << std::endl;
            }

            return true; // Successfully completed both stages
        }
    }

    std::cout << "Failed to find a solution using the hybrid algorithm." << std::endl;
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Total execution time: " << duration.count() << " microseconds" << std::endl;
    return false;
}

int main()
{
    int maxBins = 3;
    int batchIncrement = 1;
    int binCapacity = 100;

    HybridMultibin hybridMultibin(maxBins, batchIncrement, binCapacity);

    hybridMultibin.addItem(Item(1, 30));
    hybridMultibin.addItem(Item(2, 40));
    hybridMultibin.addItem(Item(3, 60));
    hybridMultibin.addItem(Item(4, 70));
    hybridMultibin.addItem(Item(5, 10));
    hybridMultibin.addItem(Item(6, 80));

    bool success = hybridMultibin.runHybridAlgorithm();

    if (!success)
    {
        std::cout << "Both MB-BFD and MB-FFD algorithms have failed. No feasible solution found." << std::endl;
    }

    return 0;
}