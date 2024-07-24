# murpkg

[murpkg](https://github.com/moskensoap/murpkg) is an unofficial package manager for MSYS2, designed to manage PKGBUILD files in user GitHub repositories. It automates the process of cloning repositories, building packages using `makepkg`, and installing them with `pacman -U`. The default repository is [MUR-packages](https://github.com/moskensoap/MUR-packages), which contains PKGBUILD files for software not suitable for the official MSYS2 repository, primarily repackaged native Windows programs linked to the MSYS bin directory. Additionally, it provides multiple versions of Java, Python, PHP, and version management tools for controlling which version is linked to the bin directory.

Its mission is to offer an alternative package management approach on Windows beyond Chocolatey and Scoop.

`murpkg` can also manage packages from other GitHub repositories. To add such repositories, the PKGBUILD files in those repositories must have `pkgname` values containing `__`. Use the `repo add` command to include these repositories:

```
murpkg repo add <reponame> <github-web-URL.git>
```

## Installation

1. Clone MUR-packages project to your computer by running:

    ```
    git clone https://github.com/moskensoap/MUR-packages.git
    ```

2. Prepare the build environment. See the [MSYS2 new package guide](https://www.msys2.org/dev/new-package/) and run:

    ```
    pacman -S --needed base-devel
    ```

3. Enter MUR-packages/murpkg and open a MSYS shell here:

    - To solve CRLF error, you may run:

        ```
        dos2unix PKGBUILD
        ```

    - Make package, run:

        ```
        makepkg --cleanbuild --syncdeps --force --noconfirm
        ```

4. To install the package, run:

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