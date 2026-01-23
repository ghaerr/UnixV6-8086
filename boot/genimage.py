import os

# Define constants for floppy disk geometry
HEADS = 2
TRACKS = 40
SECTORS_PER_TRACK = 9

# Calculate total size in bytes
TOTAL_SIZE = HEADS * TRACKS * SECTORS_PER_TRACK * 512  # Total size in bytes
BOOT_SIGNATURE_OFFSET = 510
BOOT_SIGNATURE = b'\x55\xAA'
IMG_FILE = 'Unix360.img' 

# Function to write a binary file to a specific offset
def write_file_to_offset(image_file, binary_file, offset):
    with open(binary_file, 'rb') as bin_file:
        data = bin_file.read()
        # Ensure data fits within the floppy disk size constraints
        if len(data) > TOTAL_SIZE:
            raise ValueError("binary_file is too large to fit in the floppy disk image.")

        # Write data at the specified offset
        image_file.seek(offset)
        image_file.write(data)

# Write a.bin to the beginning of the floppy disk image
with open(IMG_FILE, 'wb') as floppy_image:
    # Write empty sectors to reach the desired total size
    floppy_image.write(b'\x00' * TOTAL_SIZE)

    write_file_to_offset(floppy_image, 'boot.com', 0)

    # Write b.bin to offset 512
    write_file_to_offset(floppy_image, r'../unix.com', 512)

    # Write the boot signature at offset 510
    floppy_image.seek(BOOT_SIGNATURE_OFFSET)
    floppy_image.write(BOOT_SIGNATURE)

print("Floppy disk image created and files written successfully.")
