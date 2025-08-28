import os
import subprocess
import shutil
import platform
import glob

def get_candidates_windows():
    candidates = []
    program_files = [os.environ.get("ProgramFiles", ""), os.environ.get("ProgramFiles(x86)", "")]
    user_profile = os.environ.get("USERPROFILE", "")
    program_data = os.environ.get("ProgramData", "")

    for base in program_files:
        if base:
            candidates.extend([
                os.path.join(base, "LLVM", "bin", "clang-format.exe"),
                os.path.join(base, "Microsoft Visual Studio", "2022", "Community", "VC", "Tools", "Llvm", "bin", "clang-format.exe"),
                os.path.join(base, "Microsoft Visual Studio", "2022", "Professional", "VC", "Tools", "Llvm", "bin", "clang-format.exe"),
            ])
            candidates.extend(glob.glob(os.path.join(base, "LLVM", "*/bin/clang-format.exe")))

    if user_profile:
        candidates.append(os.path.join(user_profile, "scoop", "shims", "clang-format.exe"))
    if program_data:
        candidates.append(os.path.join(program_data, "chocolatey", "bin", "clang-format.exe"))

    return candidates

def get_candidates_macos():
    candidates = [
        "/usr/bin/clang-format",
        "/usr/local/bin/clang-format",
        "/opt/homebrew/bin/clang-format",
        "/opt/local/bin/clang-format",
    ]
    candidates.extend(glob.glob("/usr/local/Cellar/llvm/*/bin/clang-format"))
    candidates.extend(glob.glob("/opt/homebrew/Cellar/llvm/*/bin/clang-format"))
    return candidates

def get_candidates_linux():
    candidates = [
        "/usr/bin/clang-format",
        "/usr/local/bin/clang-format",
        "/snap/bin/clang-format",
    ]
    candidates.extend(glob.glob("/usr/lib/llvm-*/bin/clang-format"))
    candidates.extend(glob.glob("/usr/local/llvm-*/bin/clang-format"))
    return candidates

def find_clang_format():
    path = shutil.which("clang-format")
    if path:
        return path

    system = platform.system()
    candidates = []

    if system == "Windows":
        candidates = get_candidates_windows()
    elif system == "Darwin":
        candidates = get_candidates_macos()
    elif system == "Linux":
        candidates = get_candidates_linux()
    else:
        return None

    for p in candidates:
        if p and os.path.exists(p):
            return p

    return None
 
def format_code(file_path):
    clang_format = find_clang_format()
    print(file_path)
    command = [clang_format, "-i", "--style=file", file_path]
    subprocess.run(command)

def format_dir(dir):
    src_ext = ['.h', '.hpp', '.hxx', '.hh', '.c', '.cpp', 'cxx', '.cc' ]
    for root, dirs, files in os.walk(dir):
        for file_name in files:
            if any([file_name.endswith(ext) for ext in  src_ext]):
                file_path = os.path.join(root, file_name)
                format_code(file_path)
 
def main():
    src_dir = ['example', 'test', 'sVanilla/src']
    for dir in src_dir:
        format_dir(dir)
 
if __name__ == '__main__':
    main()