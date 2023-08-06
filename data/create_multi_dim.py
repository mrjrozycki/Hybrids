import random

def create_uniform_instance(n, dim, cap, id = None):
    # We need a function to create a bin packing instance
    # with uniform distribution of item sizes
    # and constant bin capacity
    # The number of items is given by the user
    # The function returns a vbp file where
    # the first line is a number of dimensons
    # the second line is a bin capacity
    # the third line is a number of items
    # the fourth line is a list of item sizes

    # Create a list of item sizes with uniform distribution
    # and dim dimensions
    item_sizes = []
    for i in range(n):
        item_sizes.append([])
        for j in range(dim):
            item_sizes[i].append(random.randint(1, cap[j]))

    if id is None:
        f = open(f"data/Multidim/u/uniform_instance_{n}_{dim}.vbp", "w")
    else:
        f = open(f"data/Multidim/u/uniform_instance_{n}_{dim}_{id}.vbp", "w")
    f.write(f"{dim}\n")
    f.write(f"{cap}\n")
    f.write(f"{n}\n")
    for i in range(n):
        for j in range(dim):
            f.write(f"{item_sizes[i][j]} ")
        f.write("\n")
    f.close()
    
    return item_sizes

def decomposition(i):
    numbers = []
    numbers.append(random.randint(i//2, i))
    i -= numbers[0]
    numbers.append(random.randint(i//2, i))
    i -= numbers[1]
    numbers.append(i)
    return numbers

def create_triplet_instance(n, dim, cap, id = None):
    
    bin_num = n // 3
    item_sizes = []
    for b in range(bin_num):
        numbers = []
        for d in range(dim):
            numbers.append(decomposition(cap[d]))
        pivoted_numbers = [list(i) for i in zip(*numbers)]
        for k in range(3):
            item_sizes.append(pivoted_numbers[k])



    if id is not None:
        f = open(f"data/Multidim/t/triplet_instance_{n}_{dim}_{id}.vbp", "w")
    else:
        f = open(f"data/Multidim/t/triplet_instance_{n}_{dim}.vbp", "w")
    f.write(f"{dim}\n")
    for i in range(dim):
        f.write(f"{cap[i]} ")
    f.write("\n")
    f.write(f"{n}\n")
    for i in range(n):
        for j in range(dim):
            f.write(f"{item_sizes[i][j]} ")
        f.write("\n")
    f.close()
    # print(item_sizes)
    return item_sizes


                



if __name__ == "__main__":
    for n in (120, 249, 501):
        for i in range(5):
            create_uniform_instance(n, 2, [100, 200], i)
            create_triplet_instance(n, 2, [100, 200], i)
