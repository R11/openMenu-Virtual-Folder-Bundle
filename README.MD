# openMenu Virtual Folder Bundle

<img align="right" width="300" src="https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/blob/main/images/preview.gif?raw=true">

This is a new version of openMenu with virtual folder/subfolder support and other updates and fixes, including a modified version of GD MENU Card Manager to support it. It aims to be the ultimate menu software and SD card management solution for SEGA Dreamcast [GDEMU](https://gdemu.wordpress.com/about/) users.

openMenu was originally written by [mrneo240](https://github.com/mrneo240/openmenu/), later forked by [megavolt85](https://github.com/megavolt85/openmenu), and then finally [sbstnc](https://github.com/sbstnc/openmenu/). With each iteration, important updates have been made to this open source menu software for the GDEMU, a SEGA Dreamcast optical drive emulator.

However, due to the fragmented nature of these forks and the lack of time for each of their respective owners to contribute to them, the author of this new openMenu build has decided to create a brand-new fork, rather than contribute to any of the existing ones. As an active member of the Dreamcast community, this author will work hard to maintain, update, and fix this new version of openMenu.

For those unfamiliar with the branching history of openMenu, it's important to note that **openMenu Virtual Folder Bundle** was built on top of the latest public version of openMenu, meaning it has full support for per-game virtual VMUs with [VM2](https://www.dreammods.net/store/p6/VM2.html), [VMUPro](https://stoneagegamer.com/vmu-pro-handheld-visual-memory-card-for-sega-dreamcast.html), [USB4MAPLE](http://usb4maple.wikidot.com/), and [Pico2Maple](https://github.com/cluoma/Pico2Maple-fw).

To accommodate the new openMenu, a modified version of [GD MENU Card Manager](https://github.com/sonik-br/GDMENUCardManager) has also been developed and included in this bundle.

## Table of Contents

- [Current Version](#current-version)
- [Roadmap Ideas](#roadmap-ideas)
- [Changelog](#changelog)
- [Credits](#credits)
- [Changes to openMenu and GD MENU Card Manager](#changes-to-openmenu-and-gd-menu-card-manager)
  - [Changes Since openMenu v1.3.0](#changes-since-openmenu-v130)
  - [Changes Since GD MENU Card Manager v2.0.1](#changes-since-gd-menu-card-manager-v201)
- [Upgrading](#upgrading)
- [GD MENU Card Manager Usage](#gd-menu-card-manager-usage)
  - [Managing Artwork](#managing-artwork)
    - [Artwork Assignment](#artwork-assignment)
    - [DAT Tools](#dat-tools)
    - [Restoring DAT Files From Backup](#restoring-dat-files-from-backup)
    - [Restoring a New User's Personal DAT Files](#restoring-a-new-users-personal-dat-files)
    - [Cover Art Resources](#cover-art-resources)
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
    - [Searching](#searching)
    - [Sorting](#sorting)
    - [Undo and Redo](#undo-and-redo)
    - [Saving Changes](#saving-changes)
    - [Keyboard Shortcuts](#keyboard-shortcuts)
  - [Disc Image Options](#disc-image-options)
  - [Application Settings](#application-settings)
    - [Temporary Folder](#temporary-folder)
- [openMenu Usage](#openmenu-usage)
  - [VGA and Region Patching](#vga-and-region-patching)
  - [Using Cheats](#using-cheats)
    - [Per-Game Cheat Files](#per-game-cheat-files)
  - [Virtual VMU Device Support](#virtual-vmu-device-support)
  - [Save/Load Manager](#saveload-manager)
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
- [Theming](#theming)
  - [Installing Additional Themes](#installing-additional-themes)
  - [Theme Format](#theme-format)
    - [General](#general)
    - [Color Styling](#color-styling)
    - [Game List Positioning and Sizing](#game-list-positioning-and-sizing)
    - [Artwork Positioning and Sizing](#artwork-positioning-and-sizing)
    - [Item Details Positioning](#item-details-positioning)
    - [Optional Clock Positioning and Color Styling](#optional-clock-positioning-and-color-styling)
- [Legal and Licensing](#legal-and-licensing)
  - [openMenu](#openmenu)
  - [GD MENU Card Manager](#gd-menu-card-manager)
  - [Third-Party Components](#third-party-components)

## Current Version

The latest version of **openMenu Virtual Folder Bundle** is [1.2-ateam](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/releases/tag/1.2-ateam).

## Roadmap Ideas

- Extend GD MENU Card Manager's support of archive formats to CHD for purposes of automatically extracting and adding disc images to SD card.
- New metadata format with optional openMenu popup window when a disc image is selected, showing screenshots, description, and more.
- Support saving/loading openMenu configuration to/from serial SD reader.
- See [Dreamcast Now](https://dreamcast.online/now/) information directly from openMenu (DreamPi or BBA required).
- Implement a housekeeping function in GD MENU Card Manager to help users easily identify gaps or issues with their disc image collection (e.g., duplicate serials, missing artwork, missing discs in multi-disc sets).
- Add ability to use any arbitrary path in GD MENU Card Manager as if it were an SD card, useful for those who keep separate backups of their GDEMU SD card.
- Support artwork assignment for folders.
- Add drag-and-drop support to Linux version of GD MENU Card Manager.

## Changelog

- **Version 1.2-ateam (2026-02-05)**
  - **openMenu**
    - New VMU save/load functionality with live detection of VMU devices upon loading the save manager, plus ability to upgrade old save files (see [Issue 60](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/60)).
    - New themes added, with old themes cleaned up or purged (see [Issue 44](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/44)).
    - Updated cheat database to v2.7 (see [Issue 61](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/61)).
    - Fixed vertical alignment of title bar text in credits window (see [Issue 62](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/62)).
    - Game ID support was added for Pico2Maple (see [Issue 67](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/67)).
    - Rare double button press trigger bug likely fixed (see [Issue 73](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/73)).
    - Exit to BIOS without sending Game ID now handled properly with DreamConn S (see [Issue 75](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/75)).
    - Eliminated state-transition input delay and improved button-hold detection (see [Issue 76](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/76)).
  - **GD MENU Card Manager**
    - New streamlined artwork management: artwork management directly in GD MENU Card Manager (no need for external DAT Editor), auto-generates bare minimum DAT files when none exist, prompts to auto-fix `BOX.DAT`/`ICON.DAT` mismatches, creates automatic backups to `dat_backups` folder, and performs the same Serial ID translation as openMenu (see [Issue 49](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/49)).
    - New DAT Tools window featuring import/merge, PNG export, and clear functions (see [Issue 50](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/50)).
    - Redump CUE/BIN images of both GD-ROM and CD-ROM rips now automatically converted to GDI and CDI, respectively (see [Issue 47](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/47)).
    - Moved GDI Shrink function (Windows only) to new "Disc Image Options" window, along with new VGA flag force and region patching options to support launching out-of-region games and games requiring VGA flag adjustment when launched from BIOS after "Exit to BIOS" (see [Issue 42](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/42)).
    - Search now covers both disc image titles and their Serial IDs (see [Issue 51](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/51)).
    - Added check for SD card files/folders in use, giving users a chance to close locking processes to avoid SD card corruption (see [Issue 53](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/53)).
    - Moved "Sort List" button to the top and added confirmation dialog (see [Issue 56](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/56)).
    - Added required SD card space check with graceful exit when space is exceeded (see [Issue 57](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/57)).
    - Changes made to temporary folder path are remembered on next application launch (see [Issue 58](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/58)).
    - Added undo/redo buttons to the bottom left of the main window for quickly undoing the last 10 edits to the games list, covering all operations (see [Issue 59](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/59)).
    - Removed now unnecessary Preload button (see [Issue 54](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/54)).
    - Removed Batch Rename button and moved all functions into right-click context menu, where one or more rows can be selected for batch operations (see [Issue 55](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/55)).
    - Fixed partially broken "IP.BIN Info" display (see [Issue 52](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/52)).
    - Sentence Case rename changed to Title Case and improved, Uppercase and Lowercase added (see [Issue 64](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/64)).
    - New Help/About window to reflect current state of GD MENU Card Manager (see [Issue 65](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/65)).
    - Card Manager now automatically performs Serial translations of two varieties (see [Issue 68](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/68)).
    - In addition to `DISCLIST.TXT`, a `DISCLIST.XLSX` spreadsheet is now also generated with a full listing of all disc images on SD card (see [Issue 71](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/71)).
    - Text alignment fixes in SD Drive, Temporary Folder, and Search (see [Issue 72](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/72)).
    - Tooltips added to all buttons (see [Issue 74](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/74)).
- **Version 1.1-ateam (2026-01-16)**
  - **openMenu**
    - New "Exit to BIOS" options (see [Issue 32](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/32)).
    - Added keyboard support for navigation and input (see [Issue 27](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/27), thanks [Xerxes3rd](https://github.com/Xerxes3rd)).
    - New virtual VMU device Game ID options added (see [Issue 35](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/issues/35)).
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
  - **GD MENU Card Manager**
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
  - Initial release (see the [Changes to openMenu and GD MENU Card Manager](#changes-to-openmenu-and-gd-menu-card-manager) section).

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
  - Farkus
  - FMRad10
- **Special Thanks**
  - mrneo240 for his original work on openMenu
  - megavolt85 for forking openMenu and adding new features
  - sbstnc for forking openMenu and adding a Docker development container
  - Sonik for his work on GD MENU Card Manager
  - RazorX and Chris Daioglou for some good feature ideas
  - darc for [Redump2CDI](https://dreamcast.wiki/Redump2CDI), used for Redump CD-ROM CUE/BIN to CDI conversion
  - feyris-tan for [gdidrop](https://github.com/feyris-tan/gdidrop), code from which is used for Redump GD-ROM CUE/BIN to GDI conversion
  - All of the wonderful people in the Dreamcast scene

## Changes to openMenu and GD MENU Card Manager

Many components of both openMenu and GD MENU Card Manager have been drastically overhauled for this version. See below for a list of changes.

### Changes Since openMenu v1.3.0

- New "Folders" Style mode, plus custom themes:
  - Virtual folder support includes eight levels of nested subfolders.
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
- Non-game disc images now supported, with a new column named "Type" added where users can select from two dropdown options: `Game` and `Other`. Note that non-game disc images are automatically assigned the `Other` value, as openMenu will check this property to decide how to initialize the GDEMU.
- Disc image names no longer limited to 39 characters, increased to 256 characters.
- Disc numbers can now be customized by the user (e.g., `1/3`, `2/4`).
- Maximum length for Game IDs is now ten (instead of 9), but is customizable by changing the `ProductIdMaxLength` key inside `GDMENUCardManager.dll.config`.
- Latest version of [CodeBreaker cheats database](https://www.dreamcast-talk.com/forum/viewtopic.php?t=17078) included by default.

## Upgrading

The upgrade/migration process is very simple for:

- Upgrading from a previous version of **openMenu Virtual Folder Bundle** to the latest version.
- Upgrading from an old version of the openMenu/GD MENU Card Manager combination to the latest version of **openMenu Virtual Folder Bundle**.
- Migrating from legacy gdMenu to the latest version of **openMenu Virtual Folder Bundle**.

Regardless of scenario, users follow these steps:

1. Download the [latest release package](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/releases) for the appropriate platform (e.g., Windows, Linux).
2. Restore personal DATs if applicable (see the [Restoring a New User's Personal DAT Files](#restoring-a-new-users-personal-dat-files) section).
3. Insert GDEMU SD card into computer.
4. Launch GD MENU Card Manager.
5. Ensure "openMenu" is selected in the top right radio button, then click **Save Changes**.

## GD MENU Card Manager Usage

### Managing Artwork

GD MENU Card Manager provides a streamlined artwork management system for adding cover art to games and other disc images. Artwork is stored in DAT files (e.g., Folders mode uses `BOX.DAT` for full-size cover art) located in the `tools\openMenu\menu_data` folder.

#### Artwork Assignment

To assign artwork to a game:

1. In the games list, click the **Assign** or **Manage** button under the **Art** column in the row of the game to edit.
   - The button is labeled **Assign** if no artwork is presently tied to the Serial ID, and **Manage** if there is.
2. The Artwork window displays the game's Serial ID and current artwork (if any).
3. Click **Browse for Image** to select an image file (supported formats: PNG, JPG, JPEG, GIF, WebP, BMP, TIFF, TGA).
4. The application automatically generates two PVR textures:
   - 256x256 pixels for `BOX.DAT` (full-size artwork shown in openMenu)
   - 128x128 pixels for `ICON.DAT` (thumbnail preview used in Grid mode)
5. Preview the artwork in the window, then click **Save** to apply the changes.
6. To remove artwork, click **Delete Entry** (only available when the game has existing artwork and no pending changes).

**Important Notes:**
- Artwork changes are held in memory until the user clicks **Save Changes** in the main window to write them to the DAT files.
- The application automatically performs Serial ID translation to ensure artwork is filed under the correct serial that openMenu uses internally. This handles regional variants and problematic disc IDs automatically. Because of this, users may notice they're unable to store specific serials without GD MENU Card Manager automatically correcting them per openMenu's expected serials (e.g., NTSC-U "Nightmare Creatures 2" has ID `T9504M` but will be automatically changed to `T9504N`).
- If `BOX.DAT` and `ICON.DAT` don't exist in the `tools\openMenu\menu_data` folder, the application will auto-generate minimal starter files.
- If a mismatch is detected between `BOX.DAT` and `ICON.DAT` (mismatched entry counts), the user will be prompted to auto-fix the issue.
- All DAT modifications are backed up automatically to the `dat_backups` folder before changes are made.

#### DAT Tools

The **DAT Tools** button opens a window providing three functions for managing artwork data:

**Import DAT Entries**
- Merge artwork entries from another installation's DAT files into the current setup.
- Select a folder containing `BOX.DAT` and/or `META.DAT` files.
- Choose import mode:
  - **Import only missing entries**: Only adds artwork for serials not already in current DAT files.
  - **Import all entries (overwrite existing)**: Replaces all matching entries with the imported versions.
- `ICON.DAT` is automatically regenerated from the merged `BOX.DAT`.
- Original DAT files are backed up before the merge operation.

**Export Artwork to PNGs**
- Export all artwork from currently assigned artwork to PNG image files.
- Files are named using the format: `Title [Serial].png`
- Useful for backing up artwork or sharing with others.

**Clear All DAT Entries**
- Removes all entries from `BOX.DAT`, `ICON.DAT`, and `META.DAT`.
- Creates fresh, empty DAT files.
- Original files are backed up before clearing.
- **Warning**: This action cannot be undone except by restoring from the backup.

#### Restoring DAT Files From Backup

Automatic backups are saved to the `dat_backups` folder with timestamps. To restore:

1. Navigate to the `dat_backups` folder in the GD MENU Card Manager application directory.
2. Find the backup folder with the desired timestamp.
3. Copy the `BOX.DAT`, `ICON.DAT`, and `META.DAT` files from the backup, removing the timestamp from the file name.
4. Paste them into `tools\openMenu\menu_data`, overwriting the current files.

#### Restoring a New User's Personal DAT Files

New users of **openMenu Virtual Folder Bundle** may wish to use their existing DAT files containing artwork and metadata. To do so, navigate to the `tools\openMenu\menu_data` folder in the original GD MENU Card Manager application folder and copy `BOX.DAT`, `ICON.DAT`, and `META.DAT` to the `tools\openMenu\menu_data` folder inside the **openMenu Virtual Folder Bundle**'s GD MENU Card Manager application folder, overwriting the defaults.

Note that this restoration process is also applicable to users upgrading from a previous version of **openMenu Virtual Folder Bundle** to the latest version.

#### Cover Art Resources

A collection of Dreamcast cover art maintained by [RazorX](https://x.com/RazorX2017) is available for download:

**[Dreamcast Cover Art Repository](https://mega.nz/folder/td8wVDDZ#2R7lKrrF52MVKNDobxW7LQ)**

This collection contains high-quality cover images that can be used with the artwork assignment feature.

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
| **Art** | Click the **Assign** or **Manage** button to open the artwork editing window for that game. |
| **Disc** | Disc number for multi-disc games (editable in openMenu mode only). |
| **Type** | Disc type: `Game`, `Other`, or `PSX` (openMenu only). Note that type `Other` will exit directly to BIOS, useful for things like audio CDs. |

**Note**: The first entry in the list is reserved for the menu disc image itself and cannot be edited, moved, or deleted.

#### Adding Games

Games can be added in three ways:

- Click the **Add** button (plus icon) and select one or more files from the file browser.
- Drag and drop files directly onto the application window (Windows and MacOS only).
- Add compressed archives (7Z, RAR, ZIP) containing supported disc images.

Supported disc image formats: GDI, CDI, CCD, MDS, and CUE/BIN (Redump format).

**Automatic Redump CUE/BIN Conversion**

GD MENU Card Manager automatically detects and converts Redump-style CUE/BIN disc images:

- **GD-ROM CUE/BIN**: Automatically converted to GDI format using code borrowed from gdidrop. The converter detects GD-ROM images by the presence of a "HIGH-DENSITY AREA" comment in the CUE file and properly handles track indexing and LBA calculations.
- **CD-ROM CUE/BIN**: Automatically converted to CDI format using Redump2CDI.

This feature allows users to add Redump-format disc images directly without manual conversion.

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

Select multiple games by holding the Shift or Control keys, then right-click and choose a rename option:

- **Using IP.BIN info** - Uses the game name embedded in the disc image's `IP.BIN` header.
- **Using its folder name on computer** - Uses the containing folder's name (disabled when disc image is already on SD card).
- **Using disc image's base file name** - Uses the disc image's filename (disabled when disc image is already on SD card).

**Title Case**

Right-click selected games and choose **Title Case** to convert titles to Title Case formatting.

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

#### Searching

Enter a search term in the search box and click **Search** (or press Enter). The search checks:

- Game titles
- Serial IDs (product numbers)

The first matching game is highlighted and scrolled into view. Click **Search** again to find the next match. The search is case-insensitive.

#### Sorting

Click **Sort List** to alphabetically sort all disc images. When in openMenu mode, this is a combination of Folder path and Title. In gdMenu mode, sorting is done by Title alone. A confirmation dialog is shown before sorting is applied.

#### Undo and Redo

The **Undo** and **Redo** buttons at the bottom left of the main window allow users to quickly reverse or reapply changes to the games list. The system tracks up to 10 levels of history and supports all operations including:

- Adding and removing games
- Editing titles, serials, folders, disc numbers, and types
- Reordering games (drag-and-drop or move up/down)
- Batch operations (bulk rename, folder assignment)
- Artwork changes

#### Saving Changes

Click **Save Changes** to commit all modifications to the SD card. Before saving, the application performs several checks:

**SD Card Space Check**

The application calculates the required space for all operations:
- Space needed for new games (including decompression of archives)
- Space freed by removed games
- Space for menu files and metadata

If there isn't enough space, a warning dialog shows the shortfall. Users can choose to proceed at their own risk or cancel to free up space first.

**File Lock Check**

Before modifying files, the application checks if any SD card files or folders are in use by other programs. If locked files are detected:
1. A dialog lists the affected paths and error messages.
2. Close any programs that may be using the files (file explorers, media players, etc.).
3. Click **Retry** to check again, or **Cancel** to abort the save.

This prevents SD card corruption from concurrent file access.

Users can choose to forego this operation by unchecking the **File/Folder Lock Check** option, a setting that will be remembered the next time the application is launched.

**Save Operations**

When the save proceeds, it:
- Renumbers and renames folders as needed.
- Copies pending game files to the SD card.
- Generates `OPENMENU.INI` or `GDEMU.INI` configuration files, including a virtual folder structure where applicable.
- Copies menu disc image files to folder `01`.
- Copies theme files to `menu_data` folder.
- Generates `DISCLIST.TXT` and `DISCLIST.XLSX` containing a list of all disc titles.
- Writes artwork and metadata DAT files.

A "Done!" message confirms successful completion.

#### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| **Delete** | Remove selected item(s) |
| **Escape** | Close window |
| **F2** | Rename selected item |
| **Ctrl+Z/Y** | Undo/Redo |

### Disc Image Options

The **Disc Image Options** window provides settings that affect how disc images are processed when saved to the SD card.

**GDI Shrink (Windows Only)**
- Reduces the size of GDI disc images on the SD card by removing unused data.
- Options:
  - **Apply to newly added games**: Shrink games when they're first copied to the SD card.
  - **Apply to existing games**: Shrink games already on the SD card.
  - **Also shrink compressed**: Apply shrinking to games added from compressed archives.
  - **Use blacklist (recommended)**: Skip shrinking for games known to have issues with the process.

**VGA Patch**
- Forces the VGA flag on games launched via "Exit to BIOS" from openMenu.
- Useful for games that don't natively support VGA output but work with the flag forced.
- Options:
  - **Apply to newly added games**: Patch games when they're first copied.
  - **Apply to existing games**: Patch games already on the SD card.

**Region-Free Patch**
- Enables launching out-of-region games via "Exit to BIOS" from openMenu.
- Patches games to bypass region lockout when launched from the BIOS.
- Options:
  - **Apply to newly added games**: Patch games when they're first copied.
  - **Apply to existing games**: Patch games already on the SD card.

Click **Save** to apply changes or **Cancel** to discard them.

### Application Settings

#### Temporary Folder

GD MENU Card Manager uses a temporary folder for operations like decompressing archives and converting disc images. By default, it uses the system's temp folder, but users can specify a custom location:

1. Click the **Settings** button (gear icon) in the main window.
2. Browse to select a new temporary folder location.
3. The setting is saved to `GDMENUCardManager.TempFolder.config` in the application folder and remembered for future sessions.

This is useful if a user's system drive has limited space or if a user prefers to use a faster drive for temporary operations.

## openMenu Usage

### VGA and Region Patching

openMenu's default behavior is to both VGA patch and region patch any and all launched disc images. As a result, there is no setting to toggle said behavior.

### Using Cheats

While hovered over a game in Folders view mode, press the X button to launch it with CodeBreaker for cheats. When in Grid, LineDesc, and Scroll view modes, press the B button.

The cheat database is stored in GD MENU Card Manager's `tools\openMenu\menu_data\CHEATS\FCDCHEATS.BIN` folder, which is the [latest available version](https://www.dreamcast-talk.com/forum/viewtopic.php?t=17078).

#### Per-Game Cheat Files

openMenu supports per-game cheat files for customized cheat configurations. When launching a game with CodeBreaker:

1. openMenu first looks for a game-specific cheat file named after the game's Serial ID: `tools\openMenu\menu_data\CHEATS\[SERIAL].BIN` (e.g., `MK51049.BIN` for "ChuChu Rocket!").
2. If no per-game file exists, it falls back to the global `FCDCHEATS.BIN` database.

**Creating Per-Game Cheat Files**

To create a per-game cheat file, download the [XPDC Code Compiler](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/raw/refs/heads/main/tools/xpdc_code_compiler%20(Gent%202025).exe) from this repository.

1. Launch Code Compiler and click **Import Save**, select `Binary Files (*.bin)` for "Files of type", then navigate to the `tools\openMenu\menu_data\CHEATS` folder and select the example `MK51178.BIN` file.
2. In the tool's main window will be an example layout for an individual game's cheats. Use this format to compile a cheat code list for the desired game.
3. Once complete, click **Browse File**, select `Binary Files (*.bin)` for "Files of type", then navigate to the `tools\openMenu\menu_data\CHEATS` folder, enter the game's unique Serial ID plus the `.bin` file extension (e.g., `HDR0005.BIN`), then click **Save**. A game's Serial ID can be found under the "Serial" column in GD MENU Card Manager.
4. Click the **Compile Save** to write the new per-game cheat file to the `tools\openMenu\menu_data\CHEATS` folder.
5. Insert the GDEMU SD card into computer, launch GD MENU Card Manager, and click **Save Changes** to rebuild openMenu with the new cheat file.

Note that this same method can be used to modify the global `FCDCHEATS.BIN` database file, and new codes are sometimes posted to cheat collection's [Dreamcast-Talk.com Forum Thread](https://www.dreamcast-talk.com/forum/viewtopic.php?t=17078). Users should be warned that a cheat database file may have size limits, and after a certain length entries for one or more games may disappear from the CodeBreaker menu.

### Virtual VMU Device Support

openMenu supports sending Game IDs to virtual VMU devices like the VM2, VMUPro, USB4MAPLE, and Pico2Maple to achieve per-game VMUs. The presence of such a device affects some features, like "Exit to BIOS" (see more in the [Exiting to BIOS](#exiting-to-bios) section).

Note that unlike previous versions of openMenu, **openMenu Virtual Folder Bundle** has an option to transmit Game IDs to any and all virtual VMU devices connected to the console when launching disc images and when choosing "Send Game ID" when exiting to BIOS.

### Save/Load Manager

openMenu includes a Save/Load Manager for managing VMU configuration save files. Access it from the Credits screen by selecting "Save/Load Manager".

**Features:**

- **Live VMU Detection**: The manager scans all four controller ports and both VMU sockets, displaying detected devices in real time. Device types (standard VMU, VM2, VMUPro, USB4MAPLE) are identified and shown.

- **Save Status Display**: Each detected VMU shows its current save status:
  - **(no save)**: No openMenu save file exists on this VMU.
  - **(saved)**: A current-version save file exists.
  - **(loaded)**: This VMU's save was loaded at startup.
  - **(old vX)**: An older version save file exists (will be upgraded on load).
  - **(invalid)**: A corrupted or unrecognized save file exists.
  - **(full)**: The VMU doesn't have enough free space for a save file.
  - **(future)**: The save file is from a newer version of openMenu.

- **Save to Selected**: Writes the current openMenu settings to the selected VMU.

- **Load from Selected**: Loads settings from the selected VMU. If the save file is from an older version, it will be automatically upgraded to the current format.

- **Automatic Upgrade**: When loading an old save file, openMenu automatically upgrades it to the current format and offers to save the upgraded version back to the VMU.

**Navigation:**
- Use D-Pad Up/Down to navigate between VMU slots and actions.
- Press A to select a VMU or execute an action.
- Press B to close the manager.

### PlayStation Emulator Feature

openMenu features a built-in version of the Bleemcast! emulator for launching PlayStation discs. While this emulator's performance and compatibility aren't perfect, it will typically be capable of at least booting most retail PlayStation discs as well as specially formatted self-booting Bleemcast! discs.

Support also exists for the work-in-progress [Bloom](https://github.com/pcercuei/bloom) emulator. Users can compile Bloom from source and then place `BLOOM.BIN` in the `tools\openMenu\menu_data` folder, after which launching a PlayStation disc will bring up a menu allowing for the selection between Bleemcast! and Bloom.

It's important to note that Bloom is early in its development, and while it impressively places heavy emphasis on accuracy and compatibility, most games are not currently in a playable state.

### Multi-Disc Launcher

openMenu detects multi-disc sets by finding two or more entries with the same Serial and matching disc number/count values (e.g., `1/2`, `2/2`). It's important to note that in order for openMenu to correctly identify multiple discs are part of a single set, **they must share the same unique Serial value** when the SD card was authored. See more in the [Multi-Disc Games](#multi-disc-games) section.

If the Multi-Disc setting is on "Compact", multiple discs part of the same entry will only appear once in the menu and users will be provided with a launcher menu to decide which disc to boot. There's an additional degree of control over how multi-disc entries are treated, the details of which can be read about in the [Folders Mode Settings](#folders-mode-settings) section.

### Exiting to BIOS

This build of openMenu has enhanced "Exit to BIOS" options which are dynamically displayed based on the disc image type and the presence of a virtual VMU device like VM2, VMUPro, USB4MAPLE, and Pico2Maple.

Furthermore, the "hidden 3D BIOS" can be enabled by turning the "Exit to 3D BIOS" setting on (see more in the [Available Options by Context](#available-options-by-context) section).

Note that region and VGA patching is not automatically applied when launching games from the BIOS. Instead, use [Disc Image Options](#disc-image-options) in GD MENU Card Manager to apply these patches directly to disc images.

#### Available Options by Context

| Context | Available Options |
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

**Note:** "Send Game ID" options only appear when a virtual VMU device (VM2, VMUPro, USB4MAPLE, or Pico2Maple) is detected and the selected item is a Dreamcast or PlayStation disc.

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
| **VMU Game ID** | Send to All, Send to First, Off | If virtual VMU device like VM2, VMUPro, USB4MAPLE, or Pico2Maple detected, send Game ID to the first one detected (starting at the first controller port and first VMU socket), all detected, or turn off ID transmission altogether |
| **Boot Mode** | Full Boot, License Only, Animation Only, Fast Boot | Controls how the game is launched. "Full Boot" displays the complete boot sequence, "License Only" shows only the SEGA license screen, "Animation Only" plays only the Dreamcast boot animation, and "Fast Boot" skips all startup screens and launches the game immediately. Some titles may not function correctly in modes other than "Full Boot". DreamConn S users are generally advised to use "Full Boot" to ensure VMU synchronization completes before the game starts, which is required for certain games' automatic load features. |
| **Save/Load** | N/A | Opens the [Save/Load Manager](#saveload-manager). |
| **Apply** | N/A | Applies settings for current session only. |

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
| **VMU Game ID** | Send to All, Send to First, Off | If virtual VMU device like VM2, VMUPro, USB4MAPLE, or Pico2Maple detected, send Game ID to the first one detected (starting at the first controller port and first VMU socket), all detected, or turn off ID transmission altogether |
| **Boot Mode** | Full Boot, License Only, Animation Only, Fast Boot | Controls how the game is launched. "Full Boot" displays the complete boot sequence, "License Only" shows only the SEGA license screen, "Animation Only" plays only the Dreamcast boot animation, and "Fast Boot" skips all startup screens and launches the game immediately. Some titles may not function correctly in modes other than "Full Boot". DreamConn S users are generally advised to use "Full Boot" to ensure VMU synchronization completes before the game starts, which is required for certain games' automatic load features. |
| **Save/Load** | N/A | Opens the [Save/Load Manager](#saveload-manager). |
| **Apply** | N/A | Applies settings for current session only. |

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
| **VMU Game ID** | Send to All, Send to First, Off | If virtual VMU device like VM2, VMUPro, USB4MAPLE, or Pico2Maple detected, send Game ID to the first one detected (starting at the first controller port and first VMU socket), all detected, or turn off ID transmission altogether |
| **Boot Mode** | Full Boot, License Only, Animation Only, Fast Boot | Controls how the game is launched. "Full Boot" displays the complete boot sequence, "License Only" shows only the SEGA license screen, "Animation Only" plays only the Dreamcast boot animation, and "Fast Boot" skips all startup screens and launches the game immediately. Some titles may not function correctly in modes other than "Full Boot". DreamConn S users are generally advised to use "Full Boot" to ensure VMU synchronization completes before the game starts, which is required for certain games' automatic load features. |
| **Save/Load** | N/A | Opens the [Save/Load Manager](#saveload-manager). |
| **Apply** | N/A | Applies settings for current session only. |

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

Thanks to [Xerxes3rd](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/compare/main...Xerxes3rd:openMenu-Virtual-Folder-Bundle:main), full keyboard support has been added to openMenu. See mapping below.

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

## Theming

### Installing Additional Themes

[In this folder](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/raw/refs/heads/main/themes), a collection of additional **openMenu Virtual Folder Bundle** themes are available that have been created by the development and testing team.

Depending on the version of **openMenu Virtual Folder Bundle** one is using, it's important to note that these additional themes may already be installed by default.

To install a new theme, users must follow these simple steps.

1. Navigate to the `tools\openMenu\menu_data\theme` folder.
2. Identify the last `FOLDERS_X` theme folder.
3. Create a new folder, iterating to the next available number (e.g., `FOLDERS_3`).
4. Extract the contents of the theme ZIP file into the newly created `FOLDERS_X` folder.
5. Insert GDEMU SD card into computer, open GD MENU Card Manager, and click **Save Changes**.

Users may also be interested in [GDMenu & OpenMenu HD Theme Manager](https://segaxtreme.net/resources/gdmenu-openmenu-hd-theme-manager-all-in-one-package.61/).

### Theme Format

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

#### General

- **name**: Theme display name shown in settings menu (e.g., `FoldersDefault`)

#### Color Styling

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

#### Game List Positioning and Sizing

- **list_x**: Horizontal position of game list (left edge)

  _Example_: `12` (12 pixels from left screen edge)

- **list_y**: Vertical position of game list (top of first item)

  _Example_: `68` (68 pixels from top screen edge)

- **list_count**: Number of game entries visible per page

  _Example_: `18` (18 items displayed at once)

- **list_marquee_threshold**: Maximum characters before name scrolls horizontally

  _Example_: `49` (names longer than 49 characters will marquee scroll)

#### Artwork Positioning and Sizing

- **artwork_x**: Horizontal position of box art/artwork (left edge)

  _Example_: `415` (415 pixels from left screen edge)

- **artwork_y**: Vertical position of box art/artwork (top edge)

  _Example_: `215` (215 pixels from top screen edge)

- **artwork_size**: Width and height of artwork square in pixels

  _Example_: `210` (210x210 pixel square)

#### Item Details Positioning

- **item_details_x**: Horizontal center point for item details text below artwork

  _Example_: `521` (text centered around 521 pixels from left)

- **item_details_y**: Vertical position of item details text

  _Example_: `430` (430 pixels from top screen edge)

#### Optional Clock Positioning and Color Styling

- **clock_x**: Horizontal position of the clock's right edge (right-justified)

  _Example_: `628` (clock text ends at 628 pixels from left)

- **clock_y**: Vertical position of clock text

  _Example_: `24` (24 pixels from top screen edge)

- **clock_text_color**: RGB color for clock text

  _Example_: `255,255,255` (white text)

**Note**: All position values are absolute pixel coordinates for 640x480 resolution. If a positioning key is set to `0` or omitted, Folders mode uses built-in fallback defaults. The cursor width is calculated automatically based on `list_marquee_threshold`.

After making desired changes to `THEME.INI`, use a preferred image editor to create two PNGs: one with a resolution of 512x512px named `BG_L.PNG`, the other with a resolution of 128x512 pixels named `BG_R.PNG`. Templates for these can be found in the `tools\openMenu\menu_data\theme\FOLDERS` folder, and the [included templates](https://github.com/DerekPascarella/openMenu-Virtual-Folder-Bundle/raw/refs/heads/main/theme_template/) for use with [The GIMP](https://www.gimp.org/) may prove useful to some users.

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
This project includes a modified version of GD MENU Card Manager by Sonik:

**Original work**: Copyright (C) 2021, Sonik

**Original repository**: https://github.com/sonik-br/GDMENUCardManager

Licensed under the GNU General Public License v3.0 (GPL-3.0)

**Modifications made by Derek Pascarella (ateam) in 2026:**
- See the [Changes Since GD MENU Card Manager v2.0.1](#changes-since-gd-menu-card-manager-v201) and [Changelog](#changelog) sections.

This modified version is also licensed under GPL-3.0. For the full license text, see `/GD MENU Card Manager/LICENSE`.

**Source Code Availability**: Per GPL-3.0 requirements, the complete source code for GD MENU Card Manager is included in this distribution at `/GD MENU Card Manager/src/`.

### Third-Party Components
Both projects include additional third-party components with their own licenses:
- DiscUtils (see `/GD MENU Card Manager/src/GDMENUCardManager.Core/DiscUtils/DiscUtils-LICENSE.txt`)
- SevenZip library (LGPL v3)
- Various libraries in `/openMenu/external/` with their respective licenses

All original copyright notices and licenses for these components have been preserved.
