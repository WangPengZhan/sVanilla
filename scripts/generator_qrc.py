import os
import argparse
from lxml import etree

def generate_qrc(directory, excluded_dirs, excluded_extensions, qrc_output_path):
    qresource = etree.Element('RCC')
    qresource_elem = etree.SubElement(qresource, 'qresource', prefix="/")
    excluded_dirs = [ os.path.abspath(directory + '/' + d) for d in excluded_dirs]
    for root, dirs, files in os.walk(directory):
        dirs[:] = [d for d in dirs if os.path.join(os.path.abspath(root), d) not in excluded_dirs]

        for file in files:
            file_path = os.path.relpath(os.path.join(root, file), directory)
            if not any(file_path.endswith(ext) for ext in excluded_extensions):
                etree.SubElement(qresource_elem, 'file').text = file_path

    xml_str = etree.tostring(qresource, pretty_print=True)

    with open(qrc_output_path, "wb") as f:
        f.write(xml_str)

def main():
    parser = argparse.ArgumentParser(description='Get filePaths, variable names and output_file')
    parser.add_argument('-d','--dir', default="resource",help='dir will be added to qrc')
    parser.add_argument('-o','--output', default="resource/test.qrc",help='generator qrc filePath')

    args = parser.parse_args()

    directory = args.dir
    qrc_output_path = args.output
    excluded_dirs = ['pdf']
    excluded_extensions = ['.qrc', '.md', '.py']
    generate_qrc(directory, excluded_dirs, excluded_extensions, qrc_output_path)

    print("Generated {qrc_output_path}")

if __name__ == '__main__':
    main()
