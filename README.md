# murpkg

murpkg is an unofficial package manager for [MSYS2](https://www.msys2.org). It simplifies managing PKGBUILD files from user repositories on GitHub by automating the process of cloning, building, and installing packages. It uses MSYS2's `pacman` to handle the installation. The default repository is [MUR-packages](https://github.com/moskensoap/MUR-packages), which hosts PKGBUILD files for software not available in the [official MSYS2 repository](https://packages.msys2.org/queue), including repackaged native Windows programs and multiple versions of Java, Python, PHP. It also provides tools for managing different versions of these languages.

Its mission is to offer an alternative package management approach on Windows beyond Chocolatey and Scoop.

`murpkg` can also manage packages from other GitHub repositories. To add such repositories, the PKGBUILD files in those repositories must have `pkgname` values containing `__`. Use the `repo add` command to include these repositories:

```
murpkg repo add <reponame> <github-web-URL.git>
```

## Installation

1. Install [MSYS2](https://www.msys2.org).

2. Make MSYS2 Binaries Accessible Globally (Recommended):

    By default, binaries in `/opt/bin` and other MSYS2 directories are only accessible within the MSYS2 shell. To make these binaries accessible globally within Windows (e.g., from the Command Prompt or PowerShell), follow these steps to add them to your Windows PATH environment variable:  

* Navigate to System Properties > Advanced System Settings > Environment Variables.  

* Under "User Variables" (or "System Variables" if you want the change to apply to all users), select the "Path" variable and click "Edit".  

* Append the following paths, separated by semicolons (;), to the end of the list:

```
    C:\msys64\ucrt64\bin
    C:\msys64\mingw64\bin
    C:\msys64\mingw32\bin
    C:\msys64\clang64\bin
    C:\msys64\clang32\bin
    C:\msys64\clangarm64\bin
    C:\msys64\usr\local\bin
    C:\msys64\usr\bin
    C:\msys64\bin
    C:\msys64\opt\bin
```

* **Note**: Adjust the paths as necessary if your MSYS2 installation is located in a different directory.  
 
* Click "OK" to save your changes and close the dialog boxes.  
 
* Additionally, to ensure that MSYS2 can inherit the Windows PATH (e.g., to access Windows programs within MSYS2), you can set the `MSYS2_PATH_TYPE` environment variable to `inherit`.

3. Clone MUR-packages project to your computer by running:

    ```
    git clone https://github.com/moskensoap/MUR-packages.git
    ```

4. Prepare the build environment. See the [MSYS2 new package guide](https://www.msys2.org/dev/new-package/) and run:

    ```
    pacman -S --needed base-devel
    ```

5. Navigate to the `MUR-packages/murpkg` directory and open a MSYS shell here:

    - To address potential CRLF issues, convert line endings:

        ```
        dos2unix PKGBUILD
        ```

    - Build the package:

        ```
        makepkg --cleanbuild --syncdeps --force --noconfirm
        ```

6. To install the package, run:

    ```
    pacman -U *.pkg.tar.zst
    ```

## Usage

```
murpkg command [arguments]

The commands are:

repo init       Initialize a new repository.
repo list       List all available repositories.
repo add        Add a new repository.
repo remove     Remove an existing repository.
update          Update the package database.
list            List all installed packages.
search          Search for a package in the repositories.
show            Show detailed information about a package.
install         Install a package.
reinstall       Reinstall a package.
upgrade         Upgrade all installed packages to the latest version.
remove          Remove a package.
autoremove      Automatically remove unneeded packages.
clean           Clean up the package cache.
help            Show this help message.
version         Show the version of murpkg.
```

## Configuration and Temporary Files

All files created and managed by the murpkg package manager are located in the /home/.mur/murpkg directory.

All files created and managed by MUR-packages are located in the /home/.mur directory.

Generally, manually deleting these files will not cause the program to malfunction; the program will automatically create and read these files as needed.

## Contributing

Suggestions, modifications, feature requests, and other feedback from users are welcome. Contributions via pull requests (PRs) are also appreciated.

## License

This project is licensed under the MIT License.