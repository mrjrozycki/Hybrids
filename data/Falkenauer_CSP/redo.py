import os 
os.chdir('data/Falkenauer_CSP/Falkenauer_T')
import glob
import pandas as pd
import numpy as np

# Files are in format called CSP
# First line is the number of items
# Second line is the capacity of the bins
# Next lines are the weights and demands of the items

BIN_CAPACITY = 1000

for file in glob.glob("*.txt"):
    # Add to pandas dataframe
    df = pd.read_csv(file, sep="\t", header=None, skiprows=2)
    df.columns = ["core", "nb_instances"]
    df["memory"] = 1
    df["inter_degree"] = 0
    df["inter_aff"] = [[] for i in range(len(df))]
    # Rename index
    df.index = df.index + 1
    # Change index name
    df.index.name = "app_id"
    # Change order of columns
    df = df[["nb_instances", "core", "memory", "inter_degree", "inter_aff"]]
    # Save to csv
    df.to_csv(file.replace(".txt", ".csv"), sep="\t")

