import os
import zipfile
import argparse
import re

target_dirs = ["sVanilla", "ThirdParty", "vcpkg_installed"]

def collect_and_zip_pdb_files(build_dir, output_zip):
    seen = set()
    pdb_files = []
    vc_re = re.compile(r'^vc\d+\.pdb$', re.I)

    for d in target_dirs:
        for root, _, files in os.walk(os.path.join(build_dir, d)):
            if 'debug' in root.lower():
                continue
            for f in files:
                if not f.lower().endswith('.pdb') or vc_re.match(f) or f in seen:
                    continue
                seen.add(f)
                pdb_files.append(os.path.join(root, f))

    print("all pdbs:", pdb_files)

    output_dir = os.path.dirname(output_zip)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    with zipfile.ZipFile(output_zip, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for pdb_file in pdb_files:
            zipf.write(pdb_file, os.path.basename(pdb_file))

def main():
    parser = argparse.ArgumentParser(description='Get filePaths, variable names and output_file')
    parser.add_argument('-o','--output', default="build/pdb.zip",help='collect output')
    parser.add_argument('-b','--build', default="build",help='cmake build dir')

    args = parser.parse_args()

    build = args.build
    output = args.output

    collect_and_zip_pdb_files(build, output)

if __name__ == "__main__":
    main()
