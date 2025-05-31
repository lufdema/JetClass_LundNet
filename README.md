# JetClass_LundNet

## Filling the Variables for Training
The File part_trans_to_root2_multiple.cc works to take the files on JetClass dataset and apply the algorithms to fill them with the variables used in the LundNet model. In this case the program is not restrictive, new variables could be added to use in the model in the case to be necessary.

### How to run the part_trans_to_root2_multiple.cc program?
The program must be run with the make file and in the same folder it has to be the "LundGenerator.hh" "LundGenerator.cc" files. The next is an example of how it should be run in the terminal:
> make part_trans_to_root2_multiple
> 
> ./part_trans_to_root2_multiple

**NOTE:** In this case root must be Linked in the PATH. For this version the program was made with the versions of root 6.32.08 and fastjet 3.4.3
