# openMenu Virtual Folder Bundle

<img align="right" width="300" src="https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/blob/main/images/preview.gif?raw=true">

This is a new version of openMenu with virtual folder/subfolder support and other updates and fixes, including a modified version of GD MENU Card Manager to support it. It aims to be the ultimate menu software and SD card management solution for SEGA Dreamcast [GDEMU](https://gdemu.wordpress.com/about/) users.

openMenu was originally written by [mrneo240](https://github.com/mrneo240/openmenu/), later forked by [megavolt85](https://github.com/megavolt85/openmenu), and then finally [sbstnc](https://github.com/sbstnc/openmenu/). With each iteration, important updates have been made to this open source menu software for the GDEMU, a SEGA Dreamcast optical drive emulator.

However due to the fragmented nature of these forks and the lack of time for each of their respective owners to contribute to the them, the author of this new openMenu build has decided to create a brand-new fork, rather than contribute to any of the existing ones. As an active member of the Dreamcast community, this author will work hard to maintain, update, and fix this new version of openMenu.

For those unfamiliar with the branching history of openMenu, it's important to note that **openMenu Virtual Folder Bundle** was built on top of the latest public version of openMenu, meaning it has full support for per-game virtual VMUs with [VM2](https://www.dreammods.net/store/p6/VM2.html), [VMUPro](https://stoneagegamer.com/vmu-pro-handheld-visual-memory-card-for-sega-dreamcast.html), and [USB4MAPLE](http://usb4maple.wikidot.com/).

To accommodate the new openMenu, a modified version of [GD MENU Card Manager](https://github.com/sonik-br/GDMENUCardManager) has also been developed and included in this bundle.

## Table of Contents

- [Current Version](#current-version)
- [Changelog](#changelog)
- [Credits](#credits)
- [Changes to openMenu and GD MENU Card Manager](#changes-to-openmenu-and-gd-menu-card-manager)
  - [Changes Since openMenu v1.3.0](#changes-since-openmenu-v130)
  - [Changes Since GD MENU Card Manager v2.0.1](#changes-since-gd-menu-card-manager-v201)
- [Tutorial Video](#tutorial-video)
- [GD MENU Card Manager Usage](#gd-menu-card-manager-usage)
  - [Managing DAT Files (Artwork and Metadata)](#managing-dat-files-artwork-and-metadata)
    - [Restoring a User's Personal DAT Files](#restoring-a-users-personal-dat-files)
    - [Adding and Editing Artwork and Metadata](#adding-and-editing-artwork-and-metadata)
  - [Managing SD Card](#managing-sd-card)
    - [Initial Setup](#initial-setup)
    - [Understanding the Interface](#understanding-the-interface)
    - [Adding Games](#adding-games)
    - [Removing Games](#removing-games)
    - [Reordering Games](#reordering-games)
    - [Editing Game Information](#editing-game-information)
    - [Virtual Folders (openMenu Only)](#virtual-folders-openmenu-only)
      - [Batch Folder Move and Renaming (Windows Only)](#batch-folder-move-and-renaming-windows-only)
      - [Batch Folder Assignment](#batch-folder-assignment)
    - [Multi-Disc Games](#multi-disc-games)
    - [Disc Types (openMenu Only)](#disc-types-openmenu-only)
    - [Loading Metadata](#loading-metadata)
    - [Searching](#searching)
    - [Sorting](#sorting)
    - [Saving Changes](#saving-changes)
    - [Keyboard Shortcuts](#keyboard-shortcuts)
- [openMenu Usage](#openmenu-usage)
  - [VGA and Region Patching](#vga-and-region-patching)
  - [Using Cheats](#using-cheats)
  - [Virtual VMU Device Support](#virtual-vmu-device-support)
  - [PlayStation Emulator Feature](#playstation-emulator-feature)
  - [Multi-Disc Launcher](#multi-disc-launcher)
  - [Exiting to BIOS](#exiting-to-bios)
    - [Available Options by Context](#available-options-by-context)
    - [Options Explanation](#options-explanation)
  - [Settings Menu](#settings-menu)
    - [Grid/LineDesc Mode Settings](#gridlinedesc-mode-settings)
    - [Scroll Mode Settings](#scroll-mode-settings)
    - [Folders Mode Settings](#folders-mode-settings)
  - [Button Mapping](#button-mapping)
    - [Grid and LineDesc Modes](#grid-and-linedesc-modes)
    - [Scroll Mode](#scroll-mode)
    - [Folders Mode](#folders-mode)
    - [Popup Menus (All Modes)](#popup-menus-all-modes)
  - [Keyboard Support](#keyboard-support)
- [Theme Format](#theme-format)
  - [General](#general)
  - [Colors](#colors)
  - [Game List Positioning](#game-list-positioning)
  - [Artwork Positioning](#artwork-positioning)
  - [Item Details Positioning](#item-details-positioning)
  - [Optional Clock](#optional-clock)
- [Legal and Licensing](#legal-and-licensing)
  - [openMenu](#openmenu)
  - [GD MENU Card Manager](#gd-menu-card-manager)
  - [Third-Party Components](#third-party-components)

## Current Version

The latest version of **openMenu Virtual Folder Bundle** is [1.1-ateam](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/releases/tag/1.1-ateam).

## Changelog

- **Version 1.1-ateam (2026-01-16)**
  - **openMenu**
    - New "Exit to BIOS" options (see [Issue 32](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/32)).
    - Added keyboard support for navigation and input (see [Issue 27](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/27), thanks [Xerxes3rd](https://github.com/Xerxes3rd)).
    - New virtual VMU device Game ID options added (See [Issue 35](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/35)).
    - Changed button mapping in Folders view mode: A launches disc, B goes back a folder, X launches CodeBreaker, Y exits to BIOS, Start opens Settings menu (see [Issue 26](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/26)).
    - Added "Boot Mode" setting for various full/fast boot options (see [Issue 39](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/39)).
    - Renamed Multi-Disc setting options from "Show"/"Hide" to "Show All"/"Compact" (see [Issue 25](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/25)).
    - Instances of "Multidisc" changed to "Multi-Disc" (see [Issue 30](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/30)).
    - New "Multi-Disc Grouping" setting added to Folders mode (see [Issue 31](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/31)).
    - Added info read-out display for cursor's currently highlighted item, with new theme parameters `item_details_x`, `item_details_y`, and `item_details_text_color` (see [Issue 24](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/24)).
    - Changed default sort option label to "Alphabetical" and added new "SD Card Order" sorting option across all view modes (see [Issue 23](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/23)).
    - Added optional real-time clock to Folders view mode (see [Issue 29](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/29)).
    - Renamed "Index" setting to "Display Index Numbers" for a clearer label in Scroll mode (see [Issue 22](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/22)).
    - Added option to hide/show disc details in Scroll mode Settings menu (see [Issue 18](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/18)).
    - Increased maximum supported discs in Multi-Disc launcher from 4 to 10 (see [Issue 17](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/17)).
    - Added new theme parameters `list_count` and `list_marquee_threshold` for Folders mode (see [Issue 16](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/16)).
    - Added blue swirl default theme variant, as well as red/blue swirl versions of a text-only theme (see [Issue 15](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/15) and [Issue 33](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/33)).
    - Added confirmation icon after updating VMU save file (see [Issue 14](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/14)).
    - Long game names in Multi-Disc popup now truncated with ellipsis (see [Issue 12](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/12)).
    - Added "PSX" disc type with optional Bloom emulator launcher for PlayStation disc images (see [Issue 9](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/9)).
    - Disc image now mounts on GDEMU when user selects "Exit to BIOS", enabling direct game launch or playback of a game disc's CDDA from BIOS (see [Issue 6](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/6)).
    - Non-game disc images (e.g., audio CDs) now skip full boot cycle and return directly to BIOS (see [Issue 5](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/5)).
    - Removed non-functional "Aspect" setting from Scroll mode (see [Issue 20](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/20)).
    - Fixed cover art sometimes displaying oversized/zoomed in FolderDefault theme (see [Issue 19](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/19)).
    - Fixed inconsistent margins and positioning of popup windows (see [Issue 8](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/8)).
    - Fixed PlayStation disc launching, which was non-functional in v1.0 (see [Issue 9](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/9)).
    - CodeBreaker launcher turned into navigable menu like all other popup windows (see [Issue 34](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/34)).
    - Folders mode is now default (see [Issue 36](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/36)).
    - "Close" buttons added to Multi-Disc and PlayStation Emulator launchers (see [Issue 37](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/37)).
  - **GD Menu Card Manager**
    - Added blue swirl default theme variant (see [Issue 15](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/15)).
    - Now writes `DISCLIST.TXT` to SD card root (see [Issue 13](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/13)).
    - First entry in list is no longer editable (see [Issue 11](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/11)).
    - Added "PSX" option to Type column dropdown for PlayStation disc images (see [Issue 9](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/9)).
    - Added batch folder rename/move functionality in Windows build (see [Issue 7](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/7)).
    - Folder column cells now have dynamic dropdown for folder selection in Windows build (see [Issue 4](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/4)).
    - "Sort List" button now sorts by combination of Folder + Title instead of just Title (see [Issue 2](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/2)).
    - Fixed missing default Scroll (gdMenu) style theme (see [Issue 10](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/10)).
    - Fixed CD-ROM/XA format disc parsing issues (see [Issue 3](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/3)).
    - Fixed virtual folder path being ignored on disc images without an ID (see [Issue 1](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/1)).
    - Default release package now ships with an empty META.DAT (see [Issue 38](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/38)).
	- Changed automatic disc image renaming option behavior (see [Issue 40](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/40)).
	- Updated File/Directory Move operations to overwrite existing to prevent errors (see [Issue 41](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/41)).
- **Version 1.0-ateam (2026-01-07)**
  - Initial release.

## Credits

- **Programming**
  - Derek Pascarella (ateam)
- **Testing**
  - NO HXRO
  - SEGA RPG FAN
  - Kanji
  - agarpac
  - null
  - Ade
  - NeoGohan
  - pipporamone
  - Tongara
  - Chris Daioglou
  - Retro Repair Corner
- **Special Thanks**
  - mrneo240 for his original work on openMenu
  - megavolt85 for forking openMenu and adding new features
  - sbstnc for forking openMenu and adding a Docker development container
  - Sonik for his work on GD MENU Card Manager
  - RazorX and Chris Daioglou for some good feature ideas
  - All of the wonderful people in the Dreamcast scene

## Changes to openMenu and GD MENU Card Manager

Many components of both openMenu and GD MENU Card Manager have been drastically overhauled for this version. See below for a list of changes.

### Changes Since openMenu v1.3.0

- New "Folders" Style mode, plus custom themes:
  - Virtual folder support included eight levels of nested subfolders.
  - Complete subfolder path maximum length is 256 characters, including backslashes (e.g., `Games\JP\RPG`).
  - Maximum number of virtual folders supported per SD card is 1024.
  - L Trigger + R Trigger is now a shortcut to go back one directory
- Long entry names now supported (256 characters), along with marquee scrolling over those exceeding 49 characters.
- Scroll mode improvements:
  - Jumping cursor fixed
  - New setting to toggle index numbers in the game list (On/Off)
  - New setting to toggle artwork display (On/Off)
- Settings menu now has wrap-around cursor scrolling.
- GDEMU firmware version and openMenu build version now shown in the Settings menu.
- VMU configuration save file is no longer written on every openMenu startup, now only when saving settings or during save file migration.
- Non-game discs are mounted by GDEMU instantly, returning user to BIOS for things like playing audio CDs.
- Game ID is not sent to VM2/VMUPro/USB4MAPLE when a non-game disc is launched (e.g., an audio CD).
- VMU beep setting disabled (feature is not implemented yet).
- Credits updated to include additions from this author.

### Changes Since GD MENU Card Manager v2.0.1

- Game list column widths and window width adjusted for better visibility.
- Version check in About window removed (with possibility of returning in the future).
- A new "Folder" column added when building menu in openMenu mode, allowing users to specify virtual folder path for a disc image (e.g., `Games\RPGs\NTSC-U`).
- Non-game disc images now supported, with a new column named "Type" added where users can select from two drop-down options: `Game` and `Other`. Note that non-game disc images are automatically assigned the `Other` value, as openMenu will check this property to decide how to initialize the GDEMU.
- Disc image names no longer limited to 39 characters, increased to 256 characters.
- Disc numbers can now be customized by the user (e.g., `1/3`, `2/4`).
- Maximum length for Game IDs is now ten (instead of 9), but is customizable by changing the `ProductIdMaxLength` key inside `GDMENUCardManager.dll.config`.
- Latest version of [CodeBreaker cheats database](https://www.dreamcast-talk.com/forum/viewtopic.php?t=17078) included by default.

## Tutorial Video

A full video tutorial has been provided on how to use **openMenu Virtual Folder Bundle**. Note that this video was created using the initial `v1.0-ateam` release and thus users will notice several changes when using the latest release. Core functionality and workflow, however, remains the same.

<a href="https://youtu.be/YZtgtykCnM4"><img width="400" src="https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/blob/main/images/thumbnail.png?raw=true"></a>

## GD MENU Card Manager Usage

### Managing DAT Files (Artwork and Metadata)

#### Restoring a User's Personal DAT Files

New users of **openMenu Virtual Folder Bundle** may wish to use their existing DAT files containing artwork and metadata. To do so, navigate to the `tools\openMenu\menu_data` folder in the original GD MENU Card Manager application folder and copy `BOX.DAT`, `ICON.DAT`, and `META.DAT` to the `tools\openMenu\menu_data` folder inside the **openMenu Virtual Folder Bundle**'s GD MENU Card Manager application folder, overwriting the defaults.

#### Adding and Editing Artwork and Metadata

openMenu pulls artwork and metadata about games and disc images from DAT files found in the `tools\openMenu\menu_data` folder (`BOX.DAT`, `ICON.DAT`, and `META.DAT`). It's important to note that the new "Folders" mode only leverages artwork, not metadata, for its UI.

To add, edit, or remove artwork from either `BOX.DAT` (full-size cover art) or `ICON.DAT` (thumbnail previews), refer to the section of [this video](https://www.youtube.com/watch?v=QjUgXkdiVq4&t=6m45s) that starts at the six minute and 45 second mark. Consult the video's description for direct download links to the DAT Editor software itself.

### Managing SD Card

#### Initial Setup

1. Insert GDEMU SD card into computer.
2. Launch GD MENU Card Manager.
3. Select SD card from the **Drive** dropdown. The application will auto-detect drives containing `GDEMU.ini` or a folder named `01`.
4. Choose a **Menu Type**:
   - **gdMenu** - Traditional Scroll mode menu interface.
   - **openMenu** - Full-featured menu with virtual folder support.

If needed, click **Refresh Drive List** to rescan available drives after inserting an SD card.

#### Understanding the Interface

The games list displays all disc images currently on the SD card. Each column serves a specific purpose:

| Column | Description |
|--------|-------------|
| **Info** | Click the icon to view detailed file information, `IP.BIN` metadata, and `0GDTEX.PVR` artwork preview. |
| **Location** | Shows whether the game is on "SD Card" or "Other" (pending copy). |
| **SD #** | The numbered folder on the SD card (`01`, `02`, `03`, etc.). |
| **Size** | File size of the disc image. |
| **Title** | Game display name (editable, max 256 characters). |
| **Folder** | Virtual folder path for organization (openMenu only, e.g., `Games\RPGs`). |
| **Serial** | Product ID from `IP.BIN` (editable, max 12 characters). |
| **Disc** | Disc number for multi-disc games (editable in openMenu mode only). |
| **Type** | Disc type: `Game`, `Other`, or `PSX` (openMenu only). Note that type `Other` will exit directly to BIOS, useful for things like audio CDs. |

**Note**: The first entry in the list is reserved for the menu disc image itself and cannot be edited, moved, or deleted.

#### Adding Games

Games can be added in three ways:

- Click the **Add** button (plus icon) and select one or more files from the file browser.
- Drag and drop files directly onto the application window (Windows and MacOS only).
- Add compressed archives (7Z, RAR, ZIP) containing supported disc images.

Supported disc image formats: GDI, CDI, CCD, and MDS.

#### Removing Games

Select one or more games and either:

- Click the **Remove** button (minus icon).
- Press the **Delete** key.

Removed games are deleted from the SD card when **Save Changes** is clicked.

#### Reordering Games

Use the **Move Up** and **Move Down** buttons (arrow icons) to change the order of selected games, or drag them from place to place (Windows and MacOS only). The order determines how games appear in openMenu's "SD Card Order" sort mode and affects folder numbering on the SD card.

#### Editing Game Information

**Renaming a Single Game**

- Double-click the **Title** cell, or
- Right-click and select **Rename**.

**Batch Renaming**

1. Click the **Batch Rename** button.
2. Select which games to rename:
   - **On SD Card** - Games already in numbered folders.
   - **Other** - Games pending copy to SD card.
3. Choose the naming source:
   - **Folder** - Use the folder name.
   - **File** - Use the disc image filename.
4. Enable **Read TOSEC naming** to automatically parse and clean up [TOSEC naming convention](https://www.tosecdev.org/) filenames.

**Automatic Renaming** (Right-click menu → Automatically Rename Title)

- **Using IP.BIN info** - Uses the game name embedded in the disc image's `IP.BIN` header.
- **Using its folder name on computer** - Uses the containing folder's name, such as `Nakoruru - The Gift She Gave Me (T-En)` (disabled when disc image is already on SD card).
- **Using disc image's base file name** - Uses the disc image's filename, such as `Intrepid Izzy.cdi` or `D2.gdi` (disabled when disc image is already on SD card).

**Sentence Case**: Right-click selected games and choose **Sentence Case** to convert titles to Title Case formatting.

#### Virtual Folders (openMenu Only)

When using openMenu mode, the **Folder** column allows organizing games into a hierarchical folder structure:

- Use backslashes to create nested folders (e.g., `Games\Action\US`).
- Maximum folder path length is 256 characters.
- Up to eight levels of nested subfolders are supported.
- Maximum of 1024 virtual folders per SD card.

##### Batch Folder Move and Renaming (Windows Only)

By clicking the **Batch Folder Move and Rename** button at the top right of the **Games List** table, users can perform bulk folder rename and move operations, affecting all associated disc images.

##### Batch Folder Assignment

By selecting two or more rows in the **Games List** table, users can right-click and execute **Assign Folder Path** to update the virtual folder path definition for multiple disc images at once.

To assign multiple games to the same folder, edit each game's Folder cell with the identical path.

#### Multi-Disc Games

Edit the **Disc** column to specify disc numbering (e.g., `1/4` for disc one of four). openMenu's "Compact" multi-disc setting groups games with matching titles and sequential disc numbers into a single entry with a disc selection popup.

It's important to note that in order for openMenu to correctly identify multiple discs are part of a single set, **they must share the same unique Serial value** when the SD card was authored.

For arbitrary multi-disc collections, such as an MP3, Karaoke, or VCD player boot disc plus a collection of media discs, all associated discs must appear in sequential order so that the disc-swap button on the GDEMU PCB can cycle through them correctly (while also sharing the same Serial). Next, ensure they share a sequential disc number and count value, with 10 discs being the maximum. See the example below, where the first entry is the MP3 player boot disc itself and all subsequent entries are the discs containing MP3 files.

| Title | Folder | Serial | Disc | Type |
|-------|--------|--------|------|------|
| MP3 Discs | Multimedia\Audio | MP3 | 1/4 | Game |
| MP3 Discs (Rock) | Multimedia\Audio | MP3 | 2/4 | Other |
| MP3 Discs (Country) | Multimedia\Audio | MP3 | 3/4 | Other |
| MP3 Discs (Jazz) | Multimedia\Audio | MP3 | 4/4 | Other |

In the table above, notice how even when using openMenu's "Alphabetical" sort setting, the MP3 player boot disc will appear first on the list, followed by each disc containing MP3 files. See more in the [Multi-Disc Launcher](#multi-disc-launcher) and [Folders Mode Settings](#folders-mode-settings) sections.

#### Disc Types (openMenu Only)

The **Type** column categorizes disc images:

| Type | Description |
|------|-------------|
| **Game** | Standard Dreamcast game (default). |
| **Other** | Non-game discs (audio CDs, utilities). These skip the full boot sequence and return directly to the BIOS. |
| **PSX** | PlayStation disc images for use with either the Bleem! or Bloom emulators. |

#### Loading Metadata

Click **Preload** to scan all disc images on the SD card and load their `IP.BIN` metadata. This populates:

- Original game names
- Version information
- VGA compatibility
- Release dates
- Product serial numbers

After preloading, use **Automatically Rename → Using IP.BIN info** to rename games based on their embedded titles.

#### Searching

Enter a search term in the search box and click **Search** (or press Enter). The search checks:

- Game titles
- `IP.BIN` metadata (if loaded)

The first matching game is highlighted and scrolled into view. Click **Search** again to find the next match.

#### Sorting

Click **Sort List** to alphabetically sort all disc images. When in openMenu mode, this is a combination of Folder path and Title. In gdMenu mode, sorting is done by Title alone. This helps disc images games before saving.

#### Saving Changes

Click **Save Changes** to commit all modifications to the SD card. This operation:

- Renumbers and renames folders as needed.
- Creates virtual folder structure (`FOLDERS.INI`).
- Copies pending game files to the SD card.
- Generates `OPENMENU.INI` or `GDEMU.INI` configuration files.
- Copies menu disc image files to folder `01`.
- Copies theme files to `menu_data` folder.
- Generates `DISCLIST.TXT` containing a list of all disc titles.

A "Done!" message confirms successful completion.

#### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| **Delete** | Remove selected games from list. |
| **Enter** | Execute search (when search box is focused). |
| **Escape** | Close Info or About windows. |

## openMenu Usage

### VGA and Region Patching

openMenu's default behavior is to both VGA patch and region patch any and all launched disc images. As a result, there is no setting to toggle said behavior.

### Using Cheats

While hovered over a game in Folders view mode, press the X button to launch it with CodeBreaker for cheats. When in Grid, LineDesc, and Scroll view modes, press the B button.

The cheat database is stored in GD MENU Card Manager's `tools\openMenu\menu_data\CHEATS\FCDCHEATS.BIN` folder, which is the [latest available version](https://www.dreamcast-talk.com/forum/viewtopic.php?t=17078).

### Virtual VMU Device Support

openMenu supports sending Game IDs to virtual VMU devices like the VM2, VMUPro, and USB4MAPLE to achieve per-game VMUs. The presence of such a device affects some features, like "Exit to BIOS" (see more in the [Exiting to BIOS](#exiting-to-bios) section).

Note that unlike previous versions of openMenu, **openMenu Virtual Folder Bundle** transmits Game IDs to any and all virtual VMU devices connected to the console when launching disc images and when choosing "Send Game ID" when exiting to BIOS.

### PlayStation Emulator Feature

openMenu features a built-in version of the Bleemcast! emulator for launching PlayStation discs. While this emulator's performance and compatibility isn't perfect, it will typically be capable of at least booting most retail PlayStation discs as well as specially formatted self-booting Bleemcast! discs.

Support also exists for the work-in-progress [Bloom](https://github.com/pcercuei/bloom) emulator. Users can compile Bloom from source and then place `BLOOM.BIN` in the `tools\openMenu\menu_data` folder, after which launching a PlayStation disc will bring up a menu allowing for the selection betwen Bleemcast! and Bloom.

It's important to note that Bloom is early in its development, and while it impressively places heavy emphasis on accuracy and compatibility, most games are not currently in a playable state.

### Multi-Disc Launcher

openMenu automatically detects multi-disc games (or other non-game collections) by matching the Serials of two or more discs that share the same Serial and have matching disc number/count values (e.g., `1/2`, `2/2`). It's important to note that in order for openMenu to correctly identify multiple discs are part of a single set, **they must share the same unique Serial value** when the SD card was authored. See more in the [Multi-Disc Games](#multi-disc-games) section.

If the Multi-Disc setting is on "Compact", multiple discs part of the same entry will only appear once in the menu and users will be provided with a launcher menu to decide which disc to boot. There's an additional degree of control over how multi-disc entries are treated, the details of which can be read about in the [Folders Mode Settings](#folders-mode-settings) section.

### Exiting to BIOS

This build of openMenu has enhanced "Exit to BIOS" options with are dynamically displayed based on the disc image type and the presence of a virtual VMU device like VM2, VMUPro, and USB4MAPLE.

Furthermore, the "hidden 3D BIOS" can be enabled by turning the "Exit to 3D BIOS" setting on (see more in the [Available Options by Context](#available-options-by-context) section).

#### Available Options by Context

Context | Available Options |
|---------|-------------------|
| **Folder selected** | Exit to BIOS, Close |
| **Dreamcast/PlayStation disc** *(no virtual VMU)* | Mount disc + Exit to BIOS, Exit to BIOS, Close |
| **Dreamcast/PlayStation** *(with virtual VMU)* | Send Game ID + Mount disc + Exit to BIOS, Send Game ID + Exit to BIOS, Mount disc + Exit to BIOS, Exit to BIOS, Close |
| **Other disc types** *(e.g., audio CDs)* | Mount disc + Exit to BIOS, Exit to BIOS, Close |

#### Options Explanation

| Option | Mounts Disc | Sends Game ID | Exits to BIOS |
|--------|:-----------:|:-------------:|:-------------:|
| Send Game ID + Mount disc + Exit to BIOS | ✓ | ✓ | ✓ |
| Send Game ID + Exit to BIOS | | ✓ | ✓ |
| Mount disc + Exit to BIOS | ✓ | | ✓ |
| Exit to BIOS | | | ✓ |
| Close | | | |

**Note:** "Send Game ID" options only appear when a virtual VMU device (VM2, VMUPro, or USB4MAPLE) is detected and the selected item is a Dreamcast or PlayStation disc.

### Settings Menu

The Settings menu is accessed differently depending on the current view mode:
- **Grid/LineDesc**: Press Start button
- **Scroll**: Press X button
- **Folders**: Press Start button

Each view mode has its own set of available settings. Below is a breakdown of each.

#### Grid/LineDesc Mode Settings

| Setting | Options | Description |
|---------|---------|-------------|
| **Style** | LineDesc, Grid3, Scroll, Folders | Changes the visual layout/view mode of openMenu. |
| **Theme** | NTSC-U, NTSC-J, PAL, Custom... | Selects the color scheme and background artwork. Custom themes appear if present. |
| **Aspect** | 4:3, 16:9 | Adjusts background artwork for standard or widescreen displays. |
| **Exit to 3D BIOS** | Off, On | Enables 3D BIOS when selecting "Exit to BIOS". |
| **Sort** | Alphabetical, Name, Region, Genre, SD Card Order | Determines how the game list is sorted. "SD Card Order" matches physical folder order on SD card. |
| **Filter** | All, Action, Racing, Simulation, Sports, Lightgun, Fighting, Shooter, Survival, Adventure, Platformer, RPG, Shmup, Strategy, Puzzle, Arcade, Music | Filters the game list to show only titles matching the selected genre. |
| **Multi-Disc** | Show All, Compact | "Show All" displays every disc individually. "Compact" groups multi-disc games into a single entry. |
| **VMU Game ID** | Send to All, Send to First, Off | If virtual VMU device like VM2, VMUPro, or USB4MAPLE detected, send Game ID to the first one detected (starting at the first controller port and first VMU socket), all detected, or turn off ID tranmission altogether |
| **Boot Mode** | Full Boot, License Only, Animation Only, Fast Boot | Controls how the game is launched. "Full Boot" displays the complete boot sequence, "License Only" shows only the SEGA license screen, "Animation Only" plays only the Dreamcast boot animation, and "Fast Boot" skips all startup screens and launches the game immediately. Some titles may not function correctly in modes other than "Full Boot". DreamConn S users are generally advised to use "Full Boot" to ensure VMU synchronization completes before the game starts, which is required for certain games' automatic load features. |
| **Save / Apply** | N/A | "Save" writes settings to VMU. "Apply" applies settings for current session only. |

#### Scroll Mode Settings

| Setting | Options | Description |
|---------|---------|-------------|
| **Style** | LineDesc, Grid3, Scroll, Folders | Changes the visual layout/view mode of openMenu. |
| **Theme** | GDMENU, Custom... | Selects the theme. Custom themes appear if `SCROLL_X` folders exist. |
| **Exit to 3D BIOS** | Off, On | Enables 3D BIOS when selecting "Exit to BIOS". |
| **Sort** | Alphabetical, Name, Region, Genre, SD Card Order | Determines how the game list is sorted. "SD Card Order" matches physical folder order on SD card. |
| **Filter** | All, Action, Racing, Simulation, Sports, Lightgun, Fighting, Shooter, Survival, Adventure, Platformer, RPG, Shmup, Strategy, Puzzle, Arcade, Music | Filters the game list to show only titles matching the selected genre. |
| **Multi-Disc** | Show All, Compact | "Show All" displays every disc individually. "Compact" groups multi-disc games into a single entry. |
| **Artwork** | Off, On | Toggles display of box art/cover artwork. |
| **Display Index Numbers** | Off, On | Shows or hides the index number next to each game entry. |
| **Disc Details** | Show, Hide | Toggles display of disc information (region, VGA support, etc.) alongside game entries. |
| **Marquee Speed** | Slow, Medium, Fast | Controls how quickly long game names scroll horizontally. |
| **VMU Game ID** | Send to All, Send to First, Off | If virtual VMU device like VM2, VMUPro, or USB4MAPLE detected, send Game ID to the first one detected (starting at the first controller port and first VMU socket), all detected, or turn off ID tranmission altogether |
| **Boot Mode** | Full Boot, License Only, Animation Only, Fast Boot | Controls how the game is launched. "Full Boot" displays the complete boot sequence, "License Only" shows only the SEGA license screen, "Animation Only" plays only the Dreamcast boot animation, and "Fast Boot" skips all startup screens and launches the game immediately. Some titles may not function correctly in modes other than "Full Boot". DreamConn S users are generally advised to use "Full Boot" to ensure VMU synchronization completes before the game starts, which is required for certain games' automatic load features. |
| **Save / Apply** | N/A | "Save" writes settings to VMU. "Apply" applies settings for current session only. |

#### Folders Mode Settings

| Setting | Options | Description |
|---------|---------|-------------|
| **Style** | LineDesc, Grid3, Scroll, Folders | Changes the visual layout/view mode of openMenu. |
| **Theme** | FoldersDefault, Custom... | Selects the theme. Custom themes appear if `FOLDERS_X` folders exist. |
| **Exit to 3D BIOS** | Off, On | Enables 3D BIOS when selecting "Exit to BIOS". |
| **Sort** | Alphabetical, SD Card Order | Determines how the game list is sorted. "SD Card Order" uses a "seen first" routine that organizes disc images, virtual folders, and subfolders based on their SD card sequence. |
| **Multi-Disc** | Show All, Compact | "Show All" displays every disc individually. "Compact" groups multi-disc games into a single entry. |
| **Multi-Disc Grouping** | Anywhere, Same Folder Only | Only enabled if Multi-Disc option is set to "Compact". "Anywhere" will show all associated discs on SD card when opening multi-disc launcher. "Same Folder Only" will treat discs as part of their own multi-disc (or single disc) set when organized in different folders. |
| **Artwork** | Off, On | Toggles display of box art/cover artwork. |
| **Item Details** | Off, On | Toggles display of the item details read-out below the artwork. |
| **Clock** | On (12-Hour), On (24-Hour), Off | Toggles display of the BIOS clock |
| **Marquee Speed** | Slow, Medium, Fast | Controls how quickly long game names scroll horizontally. |
| **VMU Game ID** | Send to All, Send to First, Off | If virtual VMU device like VM2, VMUPro, or USB4MAPLE detected, send Game ID to the first one detected (starting at the first controller port and first VMU socket), all detected, or turn off ID tranmission altogether |
| **Boot Mode** | Full Boot, License Only, Animation Only, Fast Boot | Controls how the game is launched. "Full Boot" displays the complete boot sequence, "License Only" shows only the SEGA license screen, "Animation Only" plays only the Dreamcast boot animation, and "Fast Boot" skips all startup screens and launches the game immediately. Some titles may not function correctly in modes other than "Full Boot". DreamConn S users are generally advised to use "Full Boot" to ensure VMU synchronization completes before the game starts, which is required for certain games' automatic load features. |
| **Save / Apply** | N/A | "Save" writes settings to VMU. "Apply" applies settings for current session only. |

### Button Mapping

#### Grid and LineDesc Modes

| Button | Function |
|--------|----------|
| D-Pad | Navigate game list |
| A | Launch selected game |
| B | Run CodeBreaker (if present) |
| X | Show full-size boxart (hold) |
| Y | Exit to BIOS menu |
| Start | Open Settings |
| L/R Triggers | Jump to previous/next letter *(LineDesc only)* |

**Note:** In LineDesc mode, Up/Down navigate by half a page at a time, while Left/Right move one item at a time.

#### Scroll Mode

| Button | Function |
|--------|----------|
| D-Pad Up/Down | Navigate by 1 item |
| D-Pad Left/Right | Navigate by 5 items |
| L/R Triggers | Navigate by 5 items |
| A | Launch selected game |
| B | Run CodeBreaker |
| X | Open Settings |
| Y | Exit to BIOS menu |

#### Folders Mode

| Button | Function |
|--------|----------|
| D-Pad Up/Down | Navigate by 1 item |
| D-Pad Left/Right | Navigate by 5 items |
| L/R Triggers | Navigate by 5 items |
| A | Launch game / Enter folder |
| B | Go back to parent folder |
| X | Run CodeBreaker |
| Y | Exit to BIOS menu |
| Start | Open Settings |

#### Popup Menus (All Modes)

When a popup menu is displayed (Settings, Multi-Disc, Exit to BIOS, etc.):

| Button | Function |
|--------|----------|
| D-Pad Up/Down | Navigate options |
| D-Pad Left/Right | Change setting value *(Settings menu only)* |
| A | Confirm / Select |
| B | Cancel / Close |

### Keyboard Support

Thanks to [Xerxes3rd](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/compare/main...Xerxes3rd:openMenu-Virtual-Folder-Bundle:main), full keyboad support has been added to openMenu. See mapping below.

| Keyboard Key     | Dreamcast Button            |
|------------------|-----------------------------|
| Arrow Keys       | D-Pad (Up/Down/Left/Right)  |
| Z or Space       | A button                    |
| X or Escape      | B button                    |
| A                | X button                    |
| S                | Y button                    |
| Enter            | Start                       |
| Q or Page Up     | Left Trigger                |
| W or Page Down   | Right Trigger               |

## Theme Format

A separate openMenu theme format was designed for the new "Folders" Style mode.

To add a new theme, navigate to the `tools\openMenu\menu_data\theme` folder and create a folder named `FOLDERS_X`, where `X` is a number between zero and nine. Inside that folder, create a new file named `THEME.INI`.

See example and explanation of parameters below.

```
[THEME]
name=FoldersDefault
text_color=255,255,255
highlight_color=207,62,17
cursor_color=75,75,75
multidisc_color=207,62,17
menu_title_color=0,0,0
menu_text_color=255,255,255
menu_highlight_color=207,62,17
menu_bkg_color=0,0,0
menu_bkg_border_color=255,255,255
list_x=13
list_y=68
list_count=18
list_marquee_threshold=49
artwork_x=416
artwork_y=215
artwork_size=210
item_details_x=521
item_details_y=430
item_details_text_color=0,0,0
clock_x=623
clock_y=36
clock_text_color=255,255,255
```

### General

- **name**: Theme display name shown in settings menu (e.g., `FoldersDefault`)

### Color Styling

- **text_color**: Color for normal (non-selected) game names in the list

  _Example_: `255,255,255` (white)

- **highlight_color**: Color for the currently selected game name

  _Example_: `207,62,17` (orange-red)

- **cursor_color**: Background color for the selection cursor rectangle

  _Example_: `75,75,75` (dark gray)

- **multidisc_color**: Color for the multi-disc indicator icon/text

  _Example_: `207,62,17` (orange-red)

- **menu_title_color**: Color for popup titles (`Settings`, `Credits`, `Multi-Disc`, etc.)

  _Example_: `0,0,0` (black)

- **menu_text_color**: Color for text items in popup menus

  _Example_: `255,255,255` (white)

- **menu_highlight_color**: Color for highlighted/selected menu items

  _Example_: `207,62,17` (orange-red)

- **menu_bkg_color**: Background fill color for popup boxes

  _Example_: `0,0,0` (black)

- **menu_bkg_border_color**: Border outline color for popup boxes

  _Example_: `255,255,255` (white)

- **item_details_text_color**: Color for the item details text (region, date, etc.) below artwork

  _Example_: `0,0,0` (black)

### Game List Positioning and Sizing

- **list_x**: Horizontal position of game list (left edge)

  _Example_: `12` (12 pixels from left screen edge)

- **list_y**: Vertical position of game list (top of first item)

  _Example_: `68` (68 pixels from top screen edge)

- **list_count**: Number of game entries visible per page

  _Example_: `18` (18 items displayed at once)

- **list_marquee_threshold**: Maximum characters before name scrolls horizontally

  _Example_: `49` (names longer than 49 characters will marquee scroll)

### Artwork Positioning and Sizing

- **artwork_x**: Horizontal position of box art/artwork (left edge)

  _Example_: `415` (415 pixels from left screen edge)

- **artwork_y**: Vertical position of box art/artwork (top edge)

  _Example_: `215` (215 pixels from top screen edge)

- **artwork_size**: Width and height of artwork square in pixels

  _Example_: `210` (210x210 pixel square)

### Item Details Positioning

- **item_details_x**: Horizontal center point for item details text below artwork

  _Example_: `521` (text centered around 521 pixels from left)
  
- **item_details_y**: Vertical position of item details text

  _Example_: `430` (430 pixels from top screen edge)

### Optional Clock Positioning and Color Styling

- **clock_x**: Horizontal position of the clock's right edge (right-justified)

  _Example_: `628` (clock text ends at 628 pixels from left)

- **clock_y**: Vertical position of clock text

  _Example_: `24` (24 pixels from top screen edge)

- **clock_text_color**: RGB color for clock text

  _Example_: `255,255,255` (white text)

**Note**: All position values are absolute pixel coordinates for 640x480 resolution. If a positioning key is set to `0` or omitted, Folders mode uses built-in fallback defaults. The cursor width is calculated automatically based on `list_marquee_threshold`.

After making desited changes to `THEME.INI`, use a preferred image editor to create two PNGs: one with a resolution of 512x512px named `BG_L.PNG`, the other with a resolution of 128x512 pixels named `BG_R.PNG`. Templates for these can be found in the `tools\openMenu\menu_data\theme\FOLDERS` folder, and the [included templates](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/raw/refs/heads/main/theme_template/) for use with [The GIMP](https://www.gimp.org/) may prove useful to some users.

Split a 640x480px background image across these two PNGs, noting that the bottom row of 32 pixels will be ignored.

![theme_1.png](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/blob/main/images/theme_1.png?raw=true)

Next, open each PNG in [PVR Tool](https://www.romhacking.net/utilities/1463/).

For `BG_L.PNG`, navigate to **Tools → Set Global Index** and use the value `1025`. Next, Navigate to **File → Save** As, using `565` for **Format**, `no palette` for **Palette**, only `Twiddle` for **Options**, and `BG_L.PVR` as the destination file name.

For `BG_R.PNG`, navigate to **Tools → Set Global Index** and use the value `1026`. Next, Navigate to **File → Save As**, using `565` for **Format**, `no palette` for **Palette**, and `BG_R.PVR` as the destination file name.

To confirm newly generated PVRs are in the correct format, navigate to **Tools → Properties** and compare the texture settings for `BG_L.PVR` and `BG_R.PVR` match the default theme's textures in the `tools\openMenu\menu_data\theme\FOLDERS` folder.

After that, open GD MENU Card Manager with GDEMU SD card inserted, then click **Save Changes**. The new theme will be accessible from the Settings menu while **Style** is set to `Folders`.

## Legal and Licensing

### openMenu
This project includes a modified version of openMenu, which is:

**Copyright © 2021, Hayden Kowalchuk**

Licensed under the Modified BSD License (3-Clause BSD License)

The original openMenu project was created by Hayden Kowalchuk (mrneo240), with subsequent forks and contributions by megavolt85 and sbstnc.

**Modifications made by Derek Pascarella (ateam) in 2026:**
- See the [Changes Since openMenu v1.3.0](#changes-since-openmenu-v130) and [Changelog](#changelog) sections.

For the full openMenu license text, see `/openMenu/LICENSE.md`.

### GD MENU Card Manager
This project includes a modified version of GD MENU Card Manager by sonik-br:

**Original work**: Copyright (C) 2021, sonik-br

**Original repository**: https://github.com/sonik-br/GDMENUCardManager

Licensed under the GNU General Public License v3.0 (GPL-3.0)

**Modifications made by Derek Pascarella (ateam) in 2026:**
- See the [Changes Since GD MENU Card Manager v2.0.1](#changes-since-gd-menu-card-manager-v201) and [Changelog](#changelog) sections.

This modified version is also licensed under GPL-3.0. For the full license text, see `/GD Menu Card Manager/LICENSE`.

**Source Code Availability**: Per GPL-3.0 requirements, the complete source code for GD MENU Card Manager is included in this distribution at `/GD Menu Card Manager/src/`.

### Third-Party Components
Both projects include additional third-party components with their own licenses:
- DiscUtils (see `/GD Menu Card Manager/src/GDMENUCardManager.Core/DiscUtils/DiscUtils-LICENSE.txt`)
- SevenZip library (LGPL v3)
- Various libraries in `/openMenu/external/` with their respective licenses

All original copyright notices and licenses for these components have been preserved.
