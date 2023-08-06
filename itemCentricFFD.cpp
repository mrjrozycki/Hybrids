#include <iostream>
#include <vector>
#include <algorithm>

class Item {
public:
    Item(int size) : size_(size) {}
    int getSize() const { return size_; }

private:
    int size_;
};

class Bin {
public:
    Bin(int capacity) : capacity_(capacity), remaining_capacity_(capacity) {}
    bool canFit(const Item& item) const { return remaining_capacity_ >= item.getSize(); }
    void addItem(const Item& item) { remaining_capacity_ -= item.getSize(); }
    int getRemainingCapacity() const { return remaining_capacity_; }

private:
    int capacity_;
    int remaining_capacity_;
};

class BinPacking {
public:
    BinPacking(int bin_capacity) : bin_capacity_(bin_capacity) {}
    void addItem(const Item& item);
    int getNumBins() const { return bins_.size(); }

private:
    int bin_capacity_;
    std::vector<Bin> bins_;
};

void BinPacking::addItem(const Item& item) {
    bool placed = false;
    // Iteratinf over existing bins to find a suitable one for the item
    for (auto& bin : bins_) {
        if (bin.canFit(item)) {
            bin.addItem(item);
            placed = true;
            break;
        }
    }

    if (!placed) {
        Bin new_bin(bin_capacity_);
        new_bin.addItem(item);
        bins_.push_back(new_bin);
    }
}

int main() {
    int bin_capacity = 10;
    std::vector<int> item_sizes = { 6, 7, 3, 4, 5, 8, 2, 9, 5 };

    BinPacking bin_packing(bin_capacity);

    // Here create items
    for (int size : item_sizes) {
        Item item(size);
        bin_packing.addItem(item);
    }

    // Usage printinf
    std::cout << "Number of bins used: " << bin_packing.getNumBins() << std::endl;

    return 0;
}
