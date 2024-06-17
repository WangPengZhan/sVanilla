import argparse
import os

def generate_cpp_uint8_array(file_path : str, variant : str):
    try:
        with open(file_path, 'rb') as f:
            data = f.read()
        
        cpp_str = f"const std::vector<uint8_t> {variant} =  {{ { ', '.join(f'0x{byte:02X}' for byte in data) } }}; // {os.path.basename(file_path)}"
    except Exception as e:
        print(f"open {file_path}, error: {e}")

    return cpp_str

def main():
    parser = argparse.ArgumentParser(description='Get filePaths, variable names and output_file')
    parser.add_argument('-f','--filePath', default="",help='Path to the file to read')
    parser.add_argument('-v','--variant', default="",help='Name to generate cpp varint name')
    parser.add_argument('-fs', '--filePaths', nargs='+', help='Path to the files to read')
    parser.add_argument('-vs', '--variables', nargs='+', default=['qrc_background', 'qrc_loading', 'qrc_tip', 'qrc_waitConfirm', 'qrc_complete', 'qrc_init', 'qrc_refresh'], help='Name to generate cpp varint name')
    parser.add_argument('-o', '--output', type=str, default="resource_qrc.h", help='Name to generate cpp file name')

    generate_content = ''
    args = parser.parse_args()
    if args.filePaths is not None and len(args.filePaths):
        generate_content += "#pragma once\n#include <vector>\n"
        generate_content += '\n'.join([generate_cpp_uint8_array(filePath,variable) for filePath ,variable in zip(args.filePaths, args.variables)])
        with open(args.output, 'wb') as f:
            f.write(generate_content.encode())
    else:
        generate_content = generate_cpp_uint8_array(args.filePath, args.variant)
    print(generate_content)


if __name__ == "__main__":
    main()
