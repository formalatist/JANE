import urllib.request
import hashlib
import sys
import os

base_url = "https://www.michaelfogleman.com/static/nes/"

def get_md5(filepath):
	with open(filepath, "rb") as file:
		return hashlib.md5(file.read()).hexdigest()

def get_roms_in_folder(path):
	rom_file_names = []
	rom_file_paths = []
	for root, dirs, files in os.walk(path):
		for file in files:
			if file.endswith(".nes"):
				rom_file_paths.append(root + "\\" + file)
				rom_file_names.append(file[:-4])
	return rom_file_names, rom_file_paths

def get_images_in_folder(path):
	image_files = []
	for root, dirs, files in os.walk(path):
		for file in files:
			if file.endswith(".png"):
				image_files.append(file[:-4])
	return image_files

def download_image(filename, filepath):
	global base_url

	print(filepath)
	md5 = get_md5(filepath)

	url = base_url + md5 + ".png"
	try:
		urllib.request.urlretrieve(url, sys.argv[2] + "\\" + filename + ".png")
	except:
		print("Failed to download " + url)

def main():
	list_of_roms, list_of_filepaths = get_roms_in_folder(sys.argv[1])
	list_of_images = get_images_in_folder(sys.argv[2])
	print(list_of_roms)
	# print(list_of_filepaths)
	print(list_of_images)

	for i in range(len(list_of_roms)):
		if list_of_roms[i] not in list_of_images:
			download_image(list_of_roms[i], list_of_filepaths[i])

if __name__ == "__main__":
	main()
