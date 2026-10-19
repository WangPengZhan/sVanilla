import requests
import json
import argparse
import time
import base64
import os

third_party = [
    {
      "name": "Qt",
      "github": "https://github.com/qt/qtbase",
      "license": "https://github.com/qt/qtbase/blob/dev/LICENSE.LGPLv3",
      "website": "https://www.qt.io/"
    },
    {
      "name": "zlib",
      "github": "https://github.com/madler/zlib",
      "license": "https://github.com/madler/zlib/blob/master/zlib.h",
      "website": "-"
    },
    {
      "name": "crashpad",
      "github": "https://chromium.googlesource.com/crashpad/crashpad",
      "license": "https://chromium.googlesource.com/crashpad/crashpad/+/main/LICENSE",
      "website": "-"
    },
    {
      "name": "curl",
      "github": "https://github.com/curl/curl",
      "license": "https://github.com/curl/curl/blob/master/COPYING",
      "website": "https://curl.se/"
    },
    {
      "name": "sqlite",
      "github": "https://github.com/sqlite/sqlite",
      "license": "https://github.com/sqlite/sqlite/blob/master/LICENSE.md",
      "website": "https://www.sqlite.org/"
    },
    {
      "name": "openssl",
      "github": "https://github.com/openssl/openssl",
      "license": "https://github.com/openssl/openssl/blob/master/LICENSE",
      "website": "https://www.openssl.org/"
    },
    {
      "name": "nlohmann-json",
      "github": "https://github.com/nlohmann/json",
      "license": "https://github.com/nlohmann/json/blob/develop/LICENSE.MIT",
      "website": "https://nlohmann.me/json/"
    },
    {
      "name": "googletest",
      "github": "https://github.com/google/googletest",
      "license": "https://github.com/google/googletest/blob/master/googletest/LICENSE",
      "website": "https://github.com/google/googletest"
    },
    {
      "name": "minizip",
      "github": "https://github.com/nmoinvaz/minizip",
      "license": "https://github.com/nmoinvaz/minizip/blob/master/LICENSE.md",
      "website": "-"
    },
    {
      "name": "qwindowkit",
      "github": "https://github.com/stdware/qwindowkit",
      "license": "https://github.com/stdware/qwindowkit/blob/main/LICENSE",
      "website": "-"
    },
    {
      "name": "fmt",
      "github": "https://github.com/fmtlib/fmt",
      "license": "https://github.com/fmtlib/fmt/blob/master/LICENSE",
      "website": "https://fmt.dev/"
    },
    {
      "name": "libxml2",
      "github": "https://gitlab.gnome.org/GNOME/libxml2",
      "license": "https://gitlab.gnome.org/GNOME/libxml2/-/blob/master/Copyright",
      "website": "https://gnome.pages.gitlab.gnome.org/libxml2/"
    },
    {
      "name": "ftxui",
      "github": "https://github.com/ArthurSonzogni/FTXUI",
      "license": "https://github.com/ArthurSonzogni/FTXUI/blob/main/LICENSE",
      "website": "https://arthursonzogni.com/FTXUI/"
    },
    {
      "name": "indicators",
      "github": "https://github.com/p-ranav/indicators",
      "license": "https://github.com/p-ranav/indicators/blob/master/LICENSE",
      "website": "-"
    },
    {
      "name": "spdlog",
      "github": "https://github.com/gabime/spdlog",
      "license": "https://github.com/gabime/spdlog/blob/master/LICENSE",
      "website": "https://github.com/gabime/spdlog"
    },
    {
      "name": "sVanillaPlugins",
      "github": "https://github.com/WangPengZhan/sVanillaPlugins",
      "license": "https://github.com/WangPengZhan/sVanillaPlugins/blob/dev/LICENSE",
      "website": "-"
    },
    {
      "name": "sVanillaPluginCommon",
      "github": "https://github.com/WangPengZhan/sVanillaPluginCommon",
      "license": "https://github.com/WangPengZhan/sVanillaPluginCommon/blob/dev/LICENSE",
      "website": "-"
    },
    {
      "name": "vanillastyle",
      "github": "https://github.com/AlanusMeminius/VanillaStyle",
      "license": "-",
      "website": "-"
    },
    {
      "name": "vcpkg",
      "github": "https://github.com/microsoft/vcpkg",
      "license": "https://github.com/microsoft/vcpkg/blob/master/LICENSE.txt",
      "website": "https://vcpkg.io/"
    },
    {
      "name": "ffmpeg",
      "github": "https://github.com/FFmpeg/FFmpeg",
      "license": "https://github.com/FFmpeg/FFmpeg/blob/master/LICENSE.md",
      "website": "https://ffmpeg.org/"
    },
    {
      "name": "aria2",
      "github": "https://github.com/aria2/aria2",
      "license": "https://github.com/aria2/aria2/blob/master/COPYING",
      "website": "https://aria2.github.io/"
    }
]

qt_license_lgplv3 = """
GNU LESSER GENERAL PUBLIC LICENSE
Version 3, 29 June 2007

Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
Everyone is permitted to copy and distribute verbatim copies
of this license document, but changing it is not allowed.

This version of the GNU Lesser General Public License incorporates
the terms and conditions of version 3 of the GNU General Public
License, supplemented by the additional permissions listed below.

0. Additional Definitions.

As used herein, "this License" refers to version 3 of the GNU Lesser
General Public License, and the "GNU GPL" refers to version 3 of the GNU
General Public License.

"The Library" refers to a covered work governed by this License,
other than an Application or a Combined Work as defined below.

An "Application" is any work that makes use of an interface provided
by the Library, but which is not otherwise based on the Library.
Defining a subclass of a class defined by the Library is deemed a mode
of using an interface provided by the Library.

A "Combined Work" is a work produced by combining or linking an
Application with the Library. The particular version of the Library
with which the Combined Work was made is also called the "Linked
Version".

The "Minimal Corresponding Source" for a Combined Work means the
Corresponding Source for the Combined Work, excluding any source code
for portions of the Combined Work that, considered in isolation, are
based on the Application, and not on the Linked Version.

The "Corresponding Application Code" for a Combined Work means the
object code and/or source code for the Application, including any data
and utility programs needed for reproducing the Combined Work from the
Application, but excluding the System Libraries of the Combined Work.

1. Exception to Section 3 of the GNU GPL.

You may convey a covered work under sections 3 and 4 of this License
without being bound by section 3 of the GNU GPL.

2. Conveying Modified Versions.

If you modify a copy of the Library, and, in your modifications, a
facility refers to a function or data to be supplied by an Application
that uses the facility (other than as an argument passed when the
facility is invoked), then you may convey a copy of the modified
version:

a) under this License, provided that you make a good faith effort to
ensure that, in the event an Application does not supply the
function or data, the facility still operates, and performs
whatever part of its purpose remains meaningful, or

b) under the GNU GPL, with none of the additional permissions of
this License applicable to that copy.

3. Object Code Incorporating Material from Library Header Files.

The object code form of an Application may incorporate material from
a header file that is part of the Library. You may convey such object
code under terms of your choice, provided that, if the incorporated
material is not limited to numerical parameters, data structure
layouts and accessors, or small macros, inline functions and templates
(ten or fewer lines in length), you do both of the following:

a) Give prominent notice with each copy of the object code that the
Library is used in it and that the Library and its use are
covered by this License.

b) Accompany the object code with a copy of the GNU GPL and this license
document.

4. Combined Works.

You may convey a Combined Work under terms of your choice that,
taken together, effectively do not restrict modification of the
portions of the Library contained in the Combined Work and reverse
engineering for debugging such modifications, if you also do each of
the following:

a) Give prominent notice with each copy of the Combined Work that
the Library is used in it and that the Library and its use are
covered by this License.

b) Accompany the Combined Work with a copy of the GNU GPL and this license
document.

c) For a Combined Work that displays copyright notices during
execution, include the copyright notice for the Library among
these notices, as well as a reference directing the user to the
copies of the GNU GPL and this license document.

d) Do one of the following:

0) Convey the Minimal Corresponding Source under the terms of this
License, and the Corresponding Application Code in a form
suitable for, and under terms that permit, the user to
recombine or relink the Application with a modified version of
the Linked Version to produce a modified Combined Work, in the
manner specified by section 6 of the GNU GPL for conveying
Corresponding Source.

1) Use a suitable shared library mechanism for linking with the
Library. A suitable mechanism is one that (a) uses at run time
a copy of the Library already present on the user's computer
system, and (b) will operate properly with a modified version
of the Library that is interface-compatible with the Linked
Version.

e) Provide Installation Information, but only if you would otherwise
be required to provide such information under section 6 of the
GNU GPL, and only to the extent that such information is
necessary to install and execute a modified version of the
Combined Work produced by recombining or relinking the
Application with a modified version of the Linked Version. (If
you use option 4d0, the Installation Information must accompany
the Minimal Corresponding Source and Corresponding Application
Code. If you use option 4d1, you must provide the Installation
Information in the manner specified by section 6 of the GNU GPL
for conveying Corresponding Source.)

5. Combined Libraries.

You may place library facilities that are a work based on the
Library side by side in a single library together with other library
facilities that are not Applications and are not covered by this
License, and convey such a combined library under terms of your
choice, if you do both of the following:

a) Accompany the combined library with a copy of the same work based
on the Library, uncombined with any other library facilities,
conveyed under the terms of this License.

b) Give prominent notice with the combined library that part of it
is a work based on the Library, and explaining where to find the
accompanying uncombined form of the same work.

6. Revised Versions of the GNU Lesser General Public License.

The Free Software Foundation may publish revised and/or new versions
of the GNU Lesser General Public License from time to time. Such new
versions will be similar in spirit to the present version, but may
differ in detail to address new problems or concerns.

Each version is given a distinguishing version number. If the
Library as you received it specifies that a certain numbered version
of the GNU Lesser General Public License "or any later version"
applies to it, you have the option of following the terms and
conditions either of that published version or of any later version
published by the Free Software Foundation. If the Library as you
received it does not specify a version number of the GNU Lesser
General Public License, you may choose any version of the GNU Lesser
General Public License ever published by the Free Software Foundation.

If the Library as you received it specifies that a proxy can decide
whether future versions of the GNU Lesser General Public License shall
apply, that proxy's public statement of acceptance of any version is
permanent authorization for you to choose that version for the
Library.
"""


def get_license(owner:str, repo:str) -> str:     
    try:
        url = f"https://api.github.com/repos/{owner}/{repo}/license"
        headers = {
            "Accept": "application/vnd.github.v3+json",
        }

        response = requests.get(url, headers=headers)
        if response.status_code == 200:
            data = response.json()
            license_content = data.get("content")
            decoded_content = base64.b64decode(license_content).decode('utf-8')
            return decoded_content
        else:
            print(f"Failed to fetch license content. Url: {url}. Status code: {response.status_code}")
    except Exception as e:
        print(f"An error occurred: {str(e)}")
    return "-"

def get_owner_and_repo(github_url):
    parts = github_url.strip("/").split("/")
    owner = parts[-2]
    repo = parts[-1]

    return owner, repo


def load_vcpkg_versions(status_filename):
    if not status_filename or not os.path.isfile(status_filename):
        return

    package_names = {
        "crashpad": "crashpad",
        "googletest": "gtest",
        "minizip": "minizip",
        "qwindowkit": "qwindowkit",
        "libxml2": "libxml2",
        "ftxui": "ftxui",
        "indicators": "indicators",
        "vanillastyle": "vanillastyle",
    }
    packages = {}
    current_package = None
    with open(status_filename, "r", encoding="utf-8") as file:
        for line in file:
            if line.startswith("Package: "):
                current_package = line.removeprefix("Package: ").strip()
            elif line.startswith("Version: ") and current_package:
                packages[current_package] = line.removeprefix("Version: ").strip()
            elif not line.strip():
                current_package = None

    for lib in third_party:
        package_name = package_names.get(lib["name"])
        if package_name and package_name in packages:
            lib["version"] = packages[package_name]


def get_licenses():
    for lib in third_party:
        if lib["name"] == "Qt":
            lib["license"] = qt_license_lgplv3
            continue
        if lib["name"] == "crashpad":
            response = requests.get(lib["license"] + "?format=TEXT")
            if response.status_code == 200:
                lib["license"] = base64.b64decode(response.content).decode("utf-8")
            else:
                print(f"Failed to fetch license content. Url: {lib['license']}. Status code: {response.status_code}")
                lib["license"] = "-"
            continue
        if lib["name"] == "libxml2":
            license_url = "https://gitlab.gnome.org/GNOME/libxml2/-/raw/master/Copyright"
            response = requests.get(license_url)
            if response.status_code == 200:
                lib["license"] = response.text
            else:
                print(f"Failed to fetch license content. Url: {license_url}. Status code: {response.status_code}")
                lib["license"] = "-"
            continue
        owner, repo = get_owner_and_repo(lib["github"])
        lib["license"] = get_license(owner, repo)
        time.sleep(0.001)


def write_to_json(filename:str, licenses=None):
    json_context = json.dumps(third_party if licenses is None else licenses, indent=4)
    with open(filename, "w", encoding="utf-8") as file:
        file.write(json_context)

def main():
    parser = argparse.ArgumentParser(description='Get authors and emails')
    parser.add_argument('json_filename', help='Path to the CSV file to write')
    parser.add_argument('--vcpkg-status', help='Path to the vcpkg installed status file')

    args = parser.parse_args()
    load_vcpkg_versions(args.vcpkg_status)

    if os.path.exists(args.json_filename):
        try:
            with open(args.json_filename, "r", encoding="utf-8") as file:
                existing_licenses = json.load(file)
            existing_by_name = {license["name"]: license for license in existing_licenses}
            existing_names = set(existing_by_name)
            required_names = {license["name"] for license in third_party}
            if required_names.issubset(existing_names):
                updated_licenses = []
                for license in third_party:
                    existing = existing_by_name[license["name"]]
                    updated = {
                        **existing,
                        **{key: value for key, value in license.items() if key != "license"},
                    }
                    if "version" not in license:
                        updated.pop("version", None)
                    updated_licenses.append(updated)
                write_to_json(args.json_filename, updated_licenses)
                return
        except (OSError, json.JSONDecodeError, KeyError, TypeError):
            pass

    get_licenses()
    write_to_json(args.json_filename)


if __name__ == "__main__":
    main()
