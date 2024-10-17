import json

def get_vcpkg_dependencies(filePath : str):
    try:
        with open(filePath, 'r') as file:
            data = json.load(file)
    except FileNotFoundError:
        print("no file: {}", filePath)
        return None
    except json.JSONDecodeError:
        print("json decode error")
        return None
    return data


def main():
    sVanilla_vcpkg = get_vcpkg_dependencies('vcpkg.json')
    plugins_deps = get_vcpkg_dependencies('ThirdParty/sVanillaPlugins/vcpkg.json')['dependencies']
    
    sVanilla_deps = sVanilla_vcpkg['dependencies']
    sVanilla_deps_names = { deps['name'] : deps for deps in sVanilla_deps}
    noSync = False
    for deps in plugins_deps:
        if (deps['name'] not in sVanilla_deps_names.keys()):
            noSync = True
            print("plugin_deps no sync, name: {}", deps['name'])
            sVanilla_deps.append(deps)
    
    if (noSync):
        with open('vcpkg.json', 'w', encoding='utf-8') as json_file:
            json.dump(sVanilla_vcpkg, json_file, indent=4)




if __name__ == "__main__":
    main()
