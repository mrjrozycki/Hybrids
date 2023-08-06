import os 
import glob
import pandas as pd
import numpy as np

# Files are in format called CSP
# First line is the number of items
# Second line is the capacity of the bins
# Next lines are the weights and demands of the items

BIN_CAPACITY = 1000
for l in ("T", "U"):
    os.chdir(f'data/Falkenauer/Falkenauer_{l}')
    for i in [60, 120, 249, 501]:
        df_merged = pd.DataFrame()
        for file in glob.glob("*" + str(i) + "*.txt"):
            # Add to pandas dataframe
            df = pd.read_csv(file, sep="\t", header=None, skiprows=2)
            df.columns = ["core"]
            # We need to create a vbp file
            # First line is the number of dimensions
            # Second line are the capacities of the dimensions
            # Third line is the number of items
            # Next lines are the weights and demands of the items

            # Create vbp file with 1 dimension
            with open(file.replace(".txt", ".vbp"), "w") as f:
                f.write("1\n")
                f.write(str(BIN_CAPACITY) + "\n")
                f.write(str(len(df)) + "\n")
                for index, row in df.iterrows():
                    f.write(str(row["core"]) + " " + "1" + "\n")




            