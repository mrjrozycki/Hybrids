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

class BinPackingBFD {
public:
    BinPackingBFD(int bin_capacity) : bin_capacity_(bin_capacity) {}
    void addItem(const Item& item);
    int getNumBins() const { return bins_.size(); }

private:
    int bin_capacity_;
    std::vector<Bin> bins_;
};

void BinPackingBFD::addItem(const Item& item) {
  
    auto it = std::min_element(bins_.begin(), bins_.end(), [&](const Bin& bin1, const Bin& bin2) {
        return bin1.getRemainingCapacity() < bin2.getRemainingCapacity() && bin1.canFit(item);
        });

    if (it != bins_.end()) {
     
        it->addItem(item);
    }
    else {
       
        Bin new_bin(bin_capacity_);
        new_bin.addItem(item);
        bins_.push_back(new_bin);
    }
}

int main() {
    int bin_capacity = 10; // Capacity of each bin
    std::vector<int> item_sizes = { 6, 7, 3, 4, 5, 8, 2, 9, 5 };

    BinPackingBFD bin_packing(bin_capacity);

    for (int size : item_sizes) {
        Item item(size);
        bin_packing.addItem(item);
    }

    std::cout << "Number of bins used: " << bin_packing.getNumBins() << std::endl;

    return 0;
}
