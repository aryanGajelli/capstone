
# Import the .vox file and read it line by line
def read_vox_file(file_path: str):
    with open(file_path, 'r') as f:
        for line in f:
            print(line)

read_vox_file('../model_processing/out.vox')