#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

// ---------------FFD -> BFD-------------------

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
    void addItem(const Item& item) { remaining_capacity_ -= item.getSize(); }
    int getRemainingCapacity() const { return remaining_capacity_; }

private:
    int capacity_;
    int remaining_capacity_;
};

class BinPacking
{
public:
    BinPacking(int bin_capacity) : bin_capacity_(bin_capacity) {}
    void addItem(const Item& item);
    int getNumBins() const { return bins_.size(); }

private:
    int bin_capacity_;
public:std::vector<Bin> bins_;
};

void BinPacking::addItem(const Item& item)
{
    bool placed = false;
    // Iterating over existing bins to find a suitable one for the item
    for (auto& bin : bins_)
    {
        if (bin.canFit(item))
        {
            bin.addItem(item);
            placed = true;
            break;
        }
    }

    if (!placed)
    {
        Bin new_bin(bin_capacity_);
        new_bin.addItem(item);
        bins_.push_back(new_bin);
    }
}

class BinPackingBFD
{
public:
    BinPackingBFD(int bin_capacity) : bin_capacity_(bin_capacity) {}
    void addItem(const Item& item);
    int getNumBins() const { return bins_.size(); }

private:
    int bin_capacity_;
public:std::vector<Bin> bins_;
};

void BinPackingBFD::addItem(const Item& item)
{
    auto it = std::min_element(bins_.begin(), bins_.end(), [&](const Bin& bin1, const Bin& bin2) {
        return bin1.getRemainingCapacity() < bin2.getRemainingCapacity() && bin1.canFit(item);
        });

    if (it != bins_.end())
    {
        it->addItem(item);
    }
    else
    {
        Bin new_bin(bin_capacity_);
        new_bin.addItem(item);
        bins_.push_back(new_bin);
    }
}

class HybridBinPacking
{
public:
    HybridBinPacking(int bin_capacity, int threshold) : bin_capacity_(bin_capacity), threshold_(threshold) {}
    bool runHybridAlgorithm(const std::vector<int>& item_sizes);
    int getNumBins() const { return bins_.size(); }

private:
    int bin_capacity_;
    int threshold_;
    std::vector<Bin> bins_;
};

bool HybridBinPacking::runHybridAlgorithm(const std::vector<int>& item_sizes)
{
    // Phase 1 - Run FFD or BFD with m bins

    auto start_time = std::chrono::high_resolution_clock::now();  //start the measure

    int numBins = threshold_;

    //Using counterr to keep trackk of the items stacked
    int numItemsStacked = 0;

    BinPacking packing(bin_capacity_);

    // Ensure there are m bins available in the packing
    for (int i = 0; i < numBins; ++i)
    {
        packing.bins_.emplace_back(bin_capacity_);
    }

    for (int i = 0; i < numBins; ++i)
    {
        for (int j = numItemsStacked; j < item_sizes.size(); ++j)
        {
            Item item(item_sizes[j]);
            if (packing.bins_[i].canFit(item))
            {
                packing.bins_[i].addItem(item);
                numItemsStacked++;
            }
        }
        if (packing.getNumBins() > 0)
        {
            std::cout << "FFD with " << numBins << " bins successful." << std::endl;
            bins_ = packing.bins_;
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            std::cout << "Execution time for FFD: " << duration.count() << " microseconds" << std::endl;

            return true;
        }
    }

    // Phase 2 - Run BFD to place left-over items in the activated containers.
    BinPackingBFD bfd(bin_capacity_);

    for (const Bin& bin : packing.bins_)
    {
        Item item(bin.getRemainingCapacity()); // Use the remaining capacity of the bin as a new item.
        bfd.addItem(item);
    }

    if (bfd.getNumBins() > 0)
    {
        std::cout << "BFD for left-over items successful." << std::endl;
        bins_ = bfd.bins_;
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        std::cout << "Execution time for BFD: " << duration.count() << " microseconds" << std::endl;
        return true;
    }

    std::cout << "Both algorithms failed to find a solution." << std::endl;
    return false;
}

int main()
{
    int bin_capacity = 10;
    int thresholdContainerVal = 3;
    std::vector<int> item_sizes = { 6, 7, 3, 4, 5, 8, 2, 9, 5 };

    HybridBinPacking hybrid(bin_capacity, thresholdContainerVal);

    if (hybrid.runHybridAlgorithm(item_sizes))
    {
        std::cout << "Number of bins used: " << hybrid.getNumBins() << std::endl;
    }

    return 0;
}