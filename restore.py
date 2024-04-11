# Restore any file encoded in base64 after being smtp sended
import base64
import sys

filename = sys.argv[1]
with open(filename, 'r') as file:
    base64_string = file.read()

file_data = base64.b64decode(base64_string)

with open(filename[:-4], 'wb') as decoded_file:
    decoded_file.write(file_data)

print("File " + filename[:-4] + " restored succesfully")
