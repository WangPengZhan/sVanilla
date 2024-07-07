import argparse
import os
import json

def dir_to_dict(dirpath):
    dir_structure = {}
    
    # 获取当前目录的子目录和文件
    entries = sorted(os.listdir(dirpath))
    subdirs = [entry for entry in entries if os.path.isdir(os.path.join(dirpath, entry))]
    files = [entry for entry in entries if os.path.isfile(os.path.join(dirpath, entry))]
    
    # 添加子目录到字典
    for subdir in subdirs:
        dir_structure[subdir] = dir_to_dict(os.path.join(dirpath, subdir))
    
    # 添加文件列表到字典
    if files:
        dir_structure["files"] = files
    
    return dir_structure

def write_json_to_file(data, filepath):
    with open(filepath, 'w', encoding='utf-8') as json_file:
        json.dump(data, json_file, ensure_ascii=False, indent=4)

def main():
    parser = argparse.ArgumentParser(description='Get filePaths, variable names and output_file')
    parser.add_argument('-d','--dir', default="build/Release", help='dir will be added to qrc')
    parser.add_argument('-o','--output', default="build/Release/manifest.json", help='generator qrc filePath')

    args = parser.parse_args()

    rootdir = args.dir
    manifest = args.output
    
    directory_structure = dir_to_dict(rootdir)
    write_json_to_file(directory_structure, manifest)

if __name__ == "__main__":
    main()
