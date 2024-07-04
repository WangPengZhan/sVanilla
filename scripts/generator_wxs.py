import os
import argparse
from lxml import etree
import copy

def create_wxs(origin_wxs_file, directory, output_file):
    tree = etree.parse(origin_wxs_file)

    elementDirectory = tree.xpath('//*[@Id="APPLICATIONFOLDER"]')[0]
    old_attr = copy.deepcopy(elementDirectory.attrib)
    elementDirectory.clear()
    elementDirectory.attrib.update(old_attr)
    elementComponentGroup = tree.xpath('//*[@Id="ProductPackageComponents"]')[0]
    add_directory(elementDirectory, elementComponentGroup, directory)

    tree.write(output_file, pretty_print=True, xml_declaration=True, encoding='utf-8')

def add_directory(parent_element, elementComponentGroup, path):
    if len(os.path.basename(path)) == 0:
        directory_id = "APPLICATIONFOLDER"
        directory_element = parent_element
    else:
        directory_id = "app_" + os.path.basename(path)
        directory_element = etree.SubElement(parent_element, "Directory", Id=directory_id, Name=os.path.basename(path))

    for item in os.listdir(path):
        item_path = os.path.join(path, item)
        if os.path.isdir(item_path):
            add_directory(directory_element, elementComponentGroup, item_path)
        elif os.path.isfile(item_path):
            global mainFile
            if os.path.basename(item_path) == mainFile:
                continue

            component_id = "file_" + os.path.basename(item_path)
            component = etree.SubElement(elementComponentGroup, "Component", Id=component_id, Guid="*", Directory=directory_id)
            etree.SubElement(component, "File", Name=os.path.basename(item_path), Source=item_path, 
                             Checksum="yes", KeyPath="yes")

    return directory_id

def main():
    parser = argparse.ArgumentParser(description='Get filePaths, variable names and output_file')
    parser.add_argument('-t','--template', default="scripts\\sVanilla.wxs",help='template wxs path')
    parser.add_argument('-i','--input', default="build\\installed\\bin",help='dir you want to package')
    parser.add_argument('-o','--output', default="build\\sVanilla.wxs",help='generator wxs filePath')

    args = parser.parse_args()
    origin_wxs_file = args.template
    input_directory = args.input
    output_wxs_file = args.output

    global mainFile
    mainFile = os.path.splitext(os.path.basename(args.output))[0] + ".exe"

    create_wxs(origin_wxs_file, input_directory, output_wxs_file)

if __name__ == "__main__":
    main()
